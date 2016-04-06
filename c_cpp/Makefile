all: train test

train:	train_hmm.o
	g++ -o train train_hmm.o
# type make/make all to compile test_hmm
train_hmm.o: train_hmm.cpp hmm.h
	g++ -c train_hmm.cpp
test:	test_hmm.o
	g++ -o test test_hmm.o
test_hmm.o: test_hmm.cpp hmm.h
	g++ -c test_hmm.cpp

clean:
	rm train_hmm.o test_hmm.o  # type make clean to remove the compiled file
