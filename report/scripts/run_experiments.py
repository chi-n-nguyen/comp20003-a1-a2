#!/usr/bin/env python3
"""
Generates synthetic datasets/queries, runs dict1 (linked list) and dict2
(Patricia tree) against them, and aggregates the bit/node/string comparison
counts each program reports on stdout into summary CSVs for the report's
pgfplots figures.

Every generated key set is checked for uniqueness (N = U, asserted, not
assumed) and every modified query is checked against the full key set it
was drawn from so a "mismatch" query never accidentally lands on a
different existing key. Each (size, density) condition is repeated across
several independently generated datasets so both query-sampling and
dataset-generation variability are captured in the reported mean/std.

Not part of the A2 code submission - this is report tooling only.
"""
import csv
import itertools
import random
import re
import statistics
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
REPORT = ROOT / "report"
DATA = REPORT / "data"
DICT1 = ROOT / "dict1"
DICT2 = ROOT / "dict2"

KEY_LEN = 32
HEADER = ("KEY,Taxa,Kingdom,Phylum,Class,Order,Family,Genus,Species,"
          "Common_Name,Identificat,Data_Resour,Sighting_Da,Latitude,"
          "Longitude,EZI_ADD")
FILLER = "X,X,X,X,X,X,X,X,X,X,X,2020-01-01,0.0,0.0,X"

RESULT_RE = re.compile(
    r"^(.*) --> (\d+) records found - comparisons: b(\d+) n(\d+) s(\d+)$")

SPARSE_ALPHABET = "abcdefghijklmnopqrstuvwxyz"
DENSE_ALPHABET = "bcdefghijklm"  # excludes 'a', the base filler

# Every unordered pair of distinct character positions, fixed once so the
# mapping from an integer index to a dense key is a genuine bijection (no
# two indices can ever produce the same string).
DENSE_POSITION_PAIRS = list(itertools.combinations(range(KEY_LEN), 2))
DENSE_CAPACITY = len(DENSE_POSITION_PAIRS) * len(DENSE_ALPHABET) ** 2

SEEDS_PER_CONDITION = 5
QUERIES_PER_SEED = 20


def sparse_key(rng):
    return "".join(rng.choice(SPARSE_ALPHABET) for _ in range(KEY_LEN))


def dense_key_from_index(index, alphabet=DENSE_ALPHABET):
    """A key that differs from an all-'a' baseline at exactly two distinct
    character positions, generalising the spec's Extra Insertion Example
    ("aaaa...b...", "aaaa...d...", ...) to arbitrarily many keys while
    keeping almost the entire string shared across the whole dataset.

    index -> (position pair, character pair) is a genuine bijection: the
    position pair is looked up directly from a precomputed list (no
    modulo wraparound that could alias two different indices onto the
    same pair), and the two positions are always used in the same
    (low, high) order, so no two indices can ever yield the same string.
    """
    a = len(alphabet)
    pair_idx, char_idx = divmod(index, a * a)
    pos_lo, pos_hi = DENSE_POSITION_PAIRS[pair_idx]
    ch_lo = alphabet[char_idx // a]
    ch_hi = alphabet[char_idx % a]
    chars = ["a"] * KEY_LEN
    chars[pos_lo] = ch_lo
    chars[pos_hi] = ch_hi
    return "".join(chars)


def make_keys(n, density, seed):
    rng = random.Random(seed)
    if density == "sparse":
        keys = set()
        while len(keys) < n:
            keys.add(sparse_key(rng))
        keys = list(keys)
    else:
        assert n <= DENSE_CAPACITY, "n exceeds dense keyspace capacity"
        indices = rng.sample(range(DENSE_CAPACITY), n)
        keys = [dense_key_from_index(i) for i in indices]
    assert len(set(keys)) == n, (
        f"generator produced duplicate keys: requested n={n}, "
        f"got {len(set(keys))} distinct strings")
    rng.shuffle(keys)
    return keys


def write_dataset(path, keys):
    with open(path, "w", newline="") as f:
        f.write(HEADER + "\n")
        for k in keys:
            f.write(k + "," + FILLER + "\n")


def flip_char(s, pos, rng, alphabet):
    other = rng.choice([c for c in alphabet if c != s[pos]])
    return s[:pos] + other + s[pos + 1:]


def make_mismatch_query(key, pos, rng, existing_keys, alphabet, max_tries=200):
    """Flips one character of key at pos, retrying until the result does
    not collide with any key already in the dataset - a query meant to
    exercise the mismatch/spellcheck path must not accidentally be an
    exact match against a *different* stored key."""
    for _ in range(max_tries):
        candidate = flip_char(key, pos, rng, alphabet)
        if candidate not in existing_keys:
            return candidate
    raise RuntimeError(f"could not find a non-colliding mismatch query for {key!r}")


def make_queries(keys, kind, m, seed, density):
    rng = random.Random(seed)
    existing = set(keys)
    alphabet = SPARSE_ALPHABET if density == "sparse" else DENSE_ALPHABET
    sample = rng.sample(keys, min(m, len(keys)))
    if kind == "exact":
        return sample
    if kind == "late_mismatch":
        queries = [make_mismatch_query(k, len(k) - 1, rng, existing, alphabet)
                   for k in sample]
    elif kind == "early_mismatch":
        queries = [make_mismatch_query(k, 0, rng, existing, alphabet)
                   for k in sample]
    else:
        raise ValueError(kind)
    assert all(q not in existing for q in queries), \
        "a mismatch query collided with an existing key"
    return queries


def run_program(binary, stage, dataset_path, query_lines, out_path):
    stdin_text = "\n".join(query_lines) + "\n"
    proc = subprocess.run(
        [str(binary), str(stage), str(dataset_path), str(out_path)],
        input=stdin_text, capture_output=True, text=True, check=True)
    rows = []
    for line in proc.stdout.splitlines():
        m = RESULT_RE.match(line)
        if not m:
            continue
        rows.append({
            "found": int(m.group(2)),
            "bit": int(m.group(3)),
            "node": int(m.group(4)),
            "string": int(m.group(5)),
        })
    return rows


def mean_std(values):
    if not values:
        return 0.0, 0.0
    mean = statistics.mean(values)
    std = statistics.pstdev(values) if len(values) > 1 else 0.0
    return mean, std


def pooled_growth_point(n, density, seed_base):
    """Runs SEEDS_PER_CONDITION independently generated datasets at this
    (n, density), each queried with QUERIES_PER_SEED exact-match queries,
    and pools every (dataset, query) sample together before taking
    mean/std - so the reported spread reflects both dataset-generation
    and query-sampling variability, not just the latter."""
    pooled = {"ll_bit": [], "ll_node": [], "ll_string": [],
              "pt_bit": [], "pt_node": [], "pt_string": []}
    for trial in range(SEEDS_PER_CONDITION):
        seed = seed_base + n * 100 + trial
        keys = make_keys(n, density, seed)
        ds_path = DATA / f"tmp_growth_{density}_{n}_{trial}.csv"
        write_dataset(ds_path, keys)
        queries = make_queries(keys, "exact", QUERIES_PER_SEED, seed + 1, density)

        rows1 = run_program(DICT1, 1, ds_path, queries, DATA / "tmp_out1.txt")
        rows2 = run_program(DICT2, 2, ds_path, queries, DATA / "tmp_out2.txt")
        for r in rows1:
            pooled["ll_bit"].append(r["bit"])
            pooled["ll_node"].append(r["node"])
            pooled["ll_string"].append(r["string"])
        for r in rows2:
            pooled["pt_bit"].append(r["bit"])
            pooled["pt_node"].append(r["node"])
            pooled["pt_string"].append(r["string"])
        ds_path.unlink()

    result = {"n": n}
    for key, values in pooled.items():
        mean, std = mean_std(values)
        result[key] = mean
        result[key + "_std"] = std
    return result


def experiment_growth(sizes, density, seed_base):
    results = []
    for n in sizes:
        point = pooled_growth_point(n, density, seed_base)
        results.append(point)
        print(f"  growth {density} n={n} done "
              f"(pooled {SEEDS_PER_CONDITION}x{QUERIES_PER_SEED} samples): "
              f"LL(b={point['ll_bit']:.1f}+-{point['ll_bit_std']:.1f}, "
              f"n={point['ll_node']:.1f}) "
              f"PT(b={point['pt_bit']:.1f}+-{point['pt_bit_std']:.1f}, "
              f"n={point['pt_node']:.1f}+-{point['pt_node_std']:.1f})")
    return results


def experiment_querytype(n, density, seed_base):
    """Fixed n, fixed density, vary query type. Each query type is
    evaluated against SEEDS_PER_CONDITION independently generated
    datasets, pooling samples the same way as the growth experiment."""
    results = []
    for kind in ("exact", "late_mismatch", "early_mismatch"):
        pooled = {"ll_bit": [], "ll_node": [], "ll_string": [],
                  "pt_bit": [], "pt_node": [], "pt_string": [], "pt_found": []}
        for trial in range(SEEDS_PER_CONDITION):
            seed = seed_base + hash(kind) % 10000 + trial
            keys = make_keys(n, density, seed)
            ds_path = DATA / f"tmp_qtype_{density}_{n}_{kind}_{trial}.csv"
            write_dataset(ds_path, keys)
            queries = make_queries(keys, kind, QUERIES_PER_SEED, seed + 1, density)

            rows1 = run_program(DICT1, 1, ds_path, queries, DATA / "tmp_out1.txt")
            rows2 = run_program(DICT2, 2, ds_path, queries, DATA / "tmp_out2.txt")
            for r in rows1:
                pooled["ll_bit"].append(r["bit"])
                pooled["ll_node"].append(r["node"])
                pooled["ll_string"].append(r["string"])
            for r in rows2:
                pooled["pt_bit"].append(r["bit"])
                pooled["pt_node"].append(r["node"])
                pooled["pt_string"].append(r["string"])
                pooled["pt_found"].append(r["found"])
            ds_path.unlink()

        row = {"kind": kind}
        for key, values in pooled.items():
            mean, std = mean_std(values)
            row[key] = mean
            row[key + "_std"] = std
        results.append(row)
        print(f"  qtype {density} n={n} {kind} done "
              f"(pooled {SEEDS_PER_CONDITION}x{QUERIES_PER_SEED} samples)")
    return results


def write_csv_rows(path, fieldnames, rows):
    with open(path, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=fieldnames)
        w.writeheader()
        for r in rows:
            w.writerow(r)


def main():
    if not DICT1.exists() or not DICT2.exists():
        sys.exit("dict1/dict2 not built - run `make all` first")

    sizes = [50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000, 40000]
    growth_fields = ["n", "ll_bit", "ll_bit_std", "ll_node", "ll_node_std",
                      "ll_string", "ll_string_std", "pt_bit", "pt_bit_std",
                      "pt_node", "pt_node_std", "pt_string", "pt_string_std"]
    qtype_fields = ["kind"] + growth_fields[1:] + ["pt_found", "pt_found_std"]

    print(f"Experiment 1: growth vs n, sparse keys, exact match "
          f"({SEEDS_PER_CONDITION} datasets x {QUERIES_PER_SEED} queries per point)")
    sparse_growth = experiment_growth(sizes, "sparse", seed_base=1000)
    write_csv_rows(DATA / "growth_sparse.csv", growth_fields, sparse_growth)

    print("Experiment 2: growth vs n, dense keys, exact match")
    dense_growth = experiment_growth(sizes, "dense", seed_base=2000)
    write_csv_rows(DATA / "growth_dense.csv", growth_fields, dense_growth)

    print("Experiment 3: query type, fixed n=5000, sparse and dense")
    qtype_sparse = experiment_querytype(5000, "sparse", seed_base=3000)
    write_csv_rows(DATA / "qtype_sparse.csv", qtype_fields, qtype_sparse)

    qtype_dense = experiment_querytype(5000, "dense", seed_base=4000)
    write_csv_rows(DATA / "qtype_dense.csv", qtype_fields, qtype_dense)

    print("Fitting log2(n) model to sparse PT node comparisons")
    import math
    xs = [math.log2(r["n"]) for r in sparse_growth]
    ys = [r["pt_node"] for r in sparse_growth]
    mean_x = sum(xs) / len(xs)
    mean_y = sum(ys) / len(ys)
    slope = (sum((x - mean_x) * (y - mean_y) for x, y in zip(xs, ys)) /
             sum((x - mean_x) ** 2 for x in xs))
    intercept = mean_y - slope * mean_x
    print(f"  fit: pt_node ~= {slope:.3f} * log2(n) + {intercept:.3f}")

    fit_rows = []
    for s, d in zip(sparse_growth, dense_growth):
        n = s["n"]
        fit_rows.append({
            "n": n,
            "pt_node_sparse": s["pt_node"],
            "pt_node_sparse_std": s["pt_node_std"],
            "pt_node_dense": d["pt_node"],
            "pt_node_dense_std": d["pt_node_std"],
            "log_fit": slope * math.log2(n) + intercept,
        })
    write_csv_rows(DATA / "growth_fit.csv",
                    ["n", "pt_node_sparse", "pt_node_sparse_std",
                     "pt_node_dense", "pt_node_dense_std", "log_fit"],
                    fit_rows)

    print("All experiments complete. Summaries written to report/data/*.csv")
    print(f"Dense keyspace capacity: {DENSE_CAPACITY} "
          f"(max n used: {max(sizes)}, utilisation "
          f"{max(sizes) / DENSE_CAPACITY:.0%})")


if __name__ == "__main__":
    main()
