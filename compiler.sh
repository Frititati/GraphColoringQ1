#/bin/bash

rm Jones/test_multi_2
rm Jones/test_multi_3
rm Jones/test_multi_5
rm Ldf/test_multi_1

rm Jones/test_seq_1
rm Jones/test_seq_2
rm Jones/test_seq_2_spinoff
rm Ldf/test_seq_1

g++ -pthread Jones/multithreaded_v2.cpp -o Jones/test_multi_2
g++ -pthread Jones/multithreaded_v3.cpp -o Jones/test_multi_3
g++ -pthread Jones/multithreaded_v5.cpp -o Jones/test_multi_5
g++ -pthread Ldf/ldf_multithreaded.cpp -o Ldf/test_multi_1

g++ Jones/sequential_first_attempt.cpp -o Jones/test_seq_1
g++ Jones/sequential_second_attempt.cpp -o Jones/test_seq_2
g++ Jones/sequential_second_spinoff_attempt.cpp -o Jones/test_seq_2_spinoff
g++ Ldf/sequential_first_attempt.cpp -o Ldf/test_seq_1
