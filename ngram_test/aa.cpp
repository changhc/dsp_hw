
//#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <map>
#include <string>
//#include <fstream>
#include <vector>
//#include <algorithm>
#include <iostream>
#include "Ngram.h"
using namespace std;

//static Vocab vocab;
//static vector< vector<string> > VecSentence;
//static vector< vector<double> > delta;
//static vector< vector<string> > psi;
//static map<string, vector<string> > table;
//static Ngram* lm = NULL;
//void viterbi(unsigned int num, vector< vector<double> > &delta, vector< vector<string> > &psi);
int main(int argc, char** argv){
	/*
	if(argc != 5){
		cerr << "Incorrect Arguments.\n./mydisambig <Input> <Map> <LM> <Order>\n";
		exit(-1);
	}
	char *input = argv[1], *map_file = argv[2], *lm_file = argv[3];
	int Norder = atoi(argv[4]);
	*/
	//char *input = "corpus_seg.txt", *map_file = "a.txt", *lm_file = "a.txt";
	int Norder = 2;
	//load lm
	/*
	lm = new Ngram(vocab, Norder);
	File f(lm_file, "r");
	lm->read(f);
	f.close();
	*/
	//load map
	FILE *file;
	file = fopen("a.txt", "r");
	while(!feof(file)){
		string tmp;
		char sentence[100];
		fgets(sentence, 100, file);
		cout<<sentence<<endl;
		//seg = strtok(sentence," ");

	}
	fclose(file);
	/*
	//load sentences
	file = fopen(input, "r");
	while(!feof(file)){
		char sentence[5000], *seg;
		vector<string> vecWord;
		fgets(sentence, 100000, file);
		printf( "%s\n", sentence);
		seg = strtok(sentence," ");
        //cout<<seg<<endl;
        while(seg != NULL){
            //printf("%s", seg);
            string tmp(seg);
            vecWord.push_back(tmp);
            seg = strtok(NULL," ");
        }
        //printf("\n");
        VecSentence.push_back(vecWord);
	}
	fclose(file);

	//viterbi
	vector<char> result;
	for(unsigned int i = 0; i < VecSentence.size(); ++i){
		viterbi(i, delta, psi);
	}
	*/
	return 0;
}
/*
double UniProb(string word){
	VocabIndex word1 = vocab.getIndex(word.c_str());
	VocabIndex word2 = vocab.getIndex("<s>");
	if(word1 == Vocab_None)
		word1 = vocab.getIndex(Vocab_Unknown);
	VocabIndex context[] = {word2, Vocab_None};
	return lm->wordProb(word1, context);
}

double BiProb(string w1, string w2){
	VocabIndex word1 = vocab.getIndex(w1.c_str());
	VocabIndex word2 = vocab.getIndex(w2.c_str());
	if(word1 == Vocab_None)
		word1 = vocab.getIndex(Vocab_Unknown);
	if(word2 == Vocab_None)
		word2 = vocab.getIndex(Vocab_Unknown);
	VocabIndex context[] = {word1, Vocab_None};
	return lm->wordProb(word2, context);
}

void viterbi(unsigned int num, vector< vector<double> > &delta, vector< vector<string> > &psi){
	map<unsigned short, vector<unsigned short> >::iterator iter;
	char *str;
	string key = VecSentence[num][0];
	vector<string> word1, word2;
	
	//init
	iter = table.find(key);
	word1 = (*iter).second;
	for( unsigned int i = 0; i < word1.size(); ++i){
		double prob = UniProb(word1[i]);
		delta[0].push_back(prob);
	}

	//loop
	double prob = 0;
	for(unsigned int t = 1; t < VecSentence[num].size(); ++t){
		key = VecSentence[num][t];
		iter = table.find(key);
		word1 = (*iter).second;

		ket = VecSentence[num][t-1];
		iter = table.find(key);
		word2 = (*iter).second;

		double max;
		string maxWord;
		for(unsigned int j = 0; j < word2.size(); ++j){
			max = -1;
			maxWord = "\0";
			for(unsigned int i = 0; i < word1.size(); ++i){
				double aij = BiProb(word1[i], word2[j]);
				prob = delta[i - 1][i] + aij;
				if(prob > max){
					max = prob; 
					maxWord = word1;
				}
			}
			delta[t].push_back(max);
			psi[t].push_back(maxWord);
		}
	}
	
	//termination
	unsigned int T = VecSentence.size() - 1;
	key = VecSentence[num][T];
	iter = table.find(key);
	word1 = (*iter).second;
	double max = -1;
	string maxWord = "\0";
	for( unsigned int i = 0; i < delta[T].size(); ++i){
		if(delta[T][i] > max){
			max = delta[T][i];
			maxWord = word1[i];
		}
	}
	delta[T + 1].push_back(max);
	psi[T + 1].push_back(maxWord);
}

vector<string> TrackBack(){
	vector<string> result;
	for(unsigned int t = delta.size() - 1; t > 0; --t){
		double max = -1;
		string maxWord = "\0";
		for(unsigned int i = 0; i < delta[t].size(); ++i){
			if(delta[t][i] > max){
				max = delta[t][i];
				maxWord = psi[t][i];
			}
		}
		result.push_back(maxWord);
	}
	reverse(result.begin(), result.end());
	return result;
}
*/