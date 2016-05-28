#include "./srlim-1.5.10/lm/src/Ngram.h"
#include "./srlim-1.5.10/lm/src/Vocab.h"
#include <sstream>
#include <cstdio>

static Vocab voc;
static vector< vector<string> > seq;
static vector< vector<double> > delta;
static vector< vector<unsigned short> > psi;
static map<unsigned short, vector<unsigned short> > table;
static Ngram* lm = NULL;

int main(int argc, char** argv){
	char *input, *map_file, *lm_file, *order;
	int Norder;
	stringstream ss; ss << order; ss >> Norder;

	lm = new Ngram(voc, Norder);
	FILE *file;
	file = fopen(lm_file, "r");
	lm->read(file);
	file.close();
}