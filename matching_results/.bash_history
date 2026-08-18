make dict1
./dict1 1 dataset_1.csv output.txt
./dict1 1 tests/dataset_1.csv output.txt
./dict1 1 tests/dataset_1.csv output.txt
make dict1
./dict1 1 tests/dataset_1.csv output.txt
gdb dict1
make dict1
./dict1 1 tests/dataset_1.csv output.txt
./dict1 1 tests/dataset_1000.csv output.txt
./dict1 1 tests/dataset_1000.csv output.txt
./dict1 1 tests/dataset_1000.csv output.txt
./dict1 1 tests/dataset_1.csv tests/test1.out < tests/test1.in > tests/test1.stdout.out
./dict1 1 tests/dataset_2.csv tests/test2.out < tests/test2.in > tests/test2.stdout.out
./dict1 1 tests/dataset_3.csv tests/test3.out < tests/test3.in > tests/test3.stdout.out
./dict1 1 tests/dataset_20.csv tests/test20.out < tests/test20.in > tests/test20.stdout.out
./dict1 1 tests/dataset_100.csv tests/test100.out < tests/test100.in > tests/test100.stdout.out
valgrind ./dict1 1 tests/dataset_100.csv tests/test100.out < tests/test100.in > tests/test100.stdout.out
LD_PRELOAD=
valgrind ./dict1 1 tests/dataset_100.csv tests/test100.out < tests/test100.in > tests/test100.stdout.out
valgrind ./dict1 1 tests/dataset_1000.csv tests/test1000.out < tests/test1000.in > tests/test1000.stdout.out
valgrind ./dict1 1 tests/dataset_1000.csv tests/test1000.out < tests/test1000.in > tests/test1000.stdout.out
valgrind ./dict1 1 tests/dataset_1000.csv tests/test1000.out < tests/test1000.in > tests/test1000.stdout.out
valgrind ./dict1 1 tests/dataset_1000.csv tests/test1000.out < tests/test1000.in > tests/test1000.stdout.out
./dict1 1 tests/dataset_1000.csv tests/test1000.out < tests/test1000.in > tests/test1000.stdout.out
./dict1 1 tests/dataset_1000.csv tests/test1000.out < tests/test1000.in > tests/test1000.stdout.out
exit
