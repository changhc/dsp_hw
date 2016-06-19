all: train test acc

train:	train_hmm.o
	g++ -o train train_hmm.o
# type make/make all to compile test_hmm
train_hmm.o: train_hmm.cpp hmm.h
	g++ -c -Wall train_hmm.cpp
test:	test_hmm.o
	g++ -o test test_hmm.o
test_hmm.o: test_hmm.cpp hmm.h
	g++ -c -Wall test_hmm.cpp
acc:	acc.o
	g++ -o acc acc.o
acc.o: acc.cpp
	g++ -c acc.cpp

clean:
	rm train_hmm.o test_hmm.o acc.o # type make clean to remove the compiled file
