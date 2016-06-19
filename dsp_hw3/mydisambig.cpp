#include "Ngram.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <map>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

static Vocab vocab;
static vector< vector<string> > VecSentence;
static map<string, vector<string> > table;
static Ngram* lm = NULL;
static double negInf = -1.0/0.0; 
void viterbi(unsigned int num, vector< vector<double> > &delta, vector< vector<int> > &from);
vector<string> TrackBack(int num, vector< vector<double> > &delta, vector< vector<int> > &from);
int main(int argc, char** argv){
	
	if(argc != 5){
		cerr << "Incorrect Arguments.\n./mydisambig <Input> <Map> <LM> <Order>\n";
		exit(-1);
	}
	char *input = argv[1], *map_file = argv[2], *lm_file = argv[3];
	int Norder = atoi(argv[4]);
	//char *input = "./testdata/1.txt", *map_file = "ZhuYin-Big5.map", *lm_file = "bigram.lm";
	//int Norder = 2;
	//load lm
	
	lm = new Ngram(vocab, Norder);
	File f(lm_file, "r");
	lm->read(f);
	f.close();
	
	//load map
	map<string, vector<string> >::iterator it;
	FILE *file;
	file = fopen(map_file, "r");
	char sentence[10000];
	while(fgets(sentence, 10000, file)){
		string key, tmp;
		char *seg;
		vector<string> list;
		seg = strtok(sentence," \t\n");
		key.assign(seg);
		seg = strtok(NULL," \t\n");
		while(seg != NULL){
			tmp.assign(seg);
			list.push_back(tmp);
		    seg = strtok(NULL," \t\n");
		}
		table[key] = list;
	}
	fclose(file);
	
	//load sentences
	file = fopen(input, "r");
	while(fgets(sentence, 10000, file)){
		char *seg;
		vector<string> vecWord;
		seg = strtok(sentence," \n");
        while(seg != NULL){
        	string tmp(seg);
        	for(int i = 0; i < tmp.size() - 1; i += 2){
        		string small_seg = tmp.substr(i, 2);
        		vecWord.push_back(small_seg);
        	}
            seg = strtok(NULL," \n");
        }
        VecSentence.push_back(vecWord);
	}
	fclose(file);

	//viterbi
	for(unsigned int i = 0; i < VecSentence.size(); ++i){
		vector<string> result;
		vector< vector<double> > delta;
		vector< vector<int> > from;
		viterbi(i, delta, from);
		result = TrackBack(i, delta, from);
		cout<<"<s> ";
		for(unsigned int j = 0; j < result.size(); ++j)
			cout<<result[j]<<' ';
		cout<<"</s>"<<endl;
	}
	
	
	return 0;
}

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

void viterbi(unsigned int num, vector< vector<double> > &delta, vector< vector<int> > &from){
	map<string, vector<string> >::iterator iter;
	string key = VecSentence[num][0];
	vector<string> word1, word2;
	
	delta.resize(VecSentence[num].size() + 1);
    from.resize(VecSentence[num].size() + 1);

	//init
	iter = table.find(key);
	word1 = (*iter).second;
	for( unsigned int i = 0; i < word1.size(); ++i){
		double prob = UniProb(word1[i]);
		delta[0].push_back(prob);
		from[0].push_back(-1);
	}

	//loop
	double prob = 0;
	for(unsigned int t = 1; t < VecSentence[num].size(); ++t){
		key = VecSentence[num][t];
		iter = table.find(key);
		word2 = (*iter).second;

		key = VecSentence[num][t-1];
		iter = table.find(key);
		word1 = (*iter).second;

		double max;
		string maxWord;
		int index;
		for(unsigned int j = 0; j < word2.size(); ++j){
			max = negInf;
			maxWord = "\0";
			for(unsigned int i = 0; i < word1.size(); ++i){
				double aij = BiProb(word1[i], word2[j]);
				prob = delta[t - 1][i] + aij;
				if(prob > max){
					max = prob; 
					index = i;
				}
			}
			delta[t].push_back(max);
			from[t].push_back(index);
		}
	}
	
	//termination
	unsigned int T = VecSentence[num].size() - 1;
	key = VecSentence[num][T];
	iter = table.find(key);
	word1 = (*iter).second;
	double max = negInf;
	string s = "</s>";
	int index;
	for( unsigned int i = 0; i < delta[T].size(); ++i){
		delta[T][i] += BiProb(word1[i], s);
	}
}

vector<string> TrackBack(int num, vector< vector<double> > &delta, vector< vector<int> > &from){
	vector<string> result;
	double max = negInf;
	map<string, vector<string> >::iterator iter;
	string key = VecSentence[num][0];
	vector<string> word1, word2;
	
	int T = VecSentence[num].size() - 1, index;
	key = VecSentence[num][T];
	iter = table.find(key);
	word1 = (*iter).second;
	for(unsigned int i = 0; i < delta[T].size(); ++i){
		if(delta[T][i] > max){
			max = delta[T][i];
			index = i;
		}
	}
	result.push_back(word1[index]);
	index = from[T][index];
	for(int t = T - 1; t >= 0; --t){
		key = VecSentence[num][t];
		iter = table.find(key);
		word1 = (*iter).second; 
		result.push_back(word1[index]);
		index = from[t][index];
	}
	reverse(result.begin(), result.end());
	return result;
}
