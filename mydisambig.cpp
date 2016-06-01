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
void viterbi(unsigned int num, vector< vector<double> > &delta, vector< vector<string> > &psi);
vector<string> TrackBack(vector< vector<double> > &delta, vector< vector<string> > &psi);
int main(int argc, char** argv){
	
	if(argc != 5){
		cerr << "Incorrect Arguments.\n./mydisambig <Input> <Map> <LM> <Order>\n";
		exit(-1);
	}
	char *input = argv[1], *map_file = argv[2], *lm_file = argv[3];
	int Norder = atoi(argv[4]);
	
	//char *input = "seg.txt", *map_file = "ZhuYin-Big5.map", *lm_file = "bigram.lm";
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
		//fgets(sentence, 10000, file);
		//cout<<sentence<<endl;
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
		//fgets(sentence, 10000, file);
		seg = strtok(sentence," \n");
        while(seg != NULL){
            string tmp(seg);
            vecWord.push_back(tmp);
            seg = strtok(NULL," \n");
        }
        //printf("\n");
        VecSentence.push_back(vecWord);
	}
	fclose(file);
	//cout<<VecSentence.size();
	//viterbi
	for(unsigned int i = 0; i < VecSentence.size(); ++i){
		vector<string> result;
		vector< vector<double> > delta;
		vector< vector<string> > psi;
		viterbi(i, delta, psi);
		result = TrackBack(delta, psi);
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

double TriProb(string w1, string w2, string w3) 
{
    VocabIndex wid1 = voc.getIndex(w1.c_str());
    VocabIndex wid2 = voc.getIndex(w2.c_str());
    VocabIndex wid3 = voc.getIndex(w3.c_str());

    if(wid1 == Vocab_None)  //OOV
        wid1 = voc.getIndex(Vocab_Unknown);
    if(wid2 == Vocab_None)  //OOV
        wid2 = voc.getIndex(Vocab_Unknown);
    if(wid3 == Vocab_None)  //OOV
        wid3 = voc.getIndex(Vocab_Unknown);

    VocabIndex context[] = { wid2, wid1, Vocab_None };
    return lm.wordProb( wid3, context );
}

void viterbi(unsigned int num, vector< vector<double> > &delta, vector< vector<string> > &psi){
	map<string, vector<string> >::iterator iter;
	char *str;
	string key = VecSentence[num][0];
	vector<string> word1, word2;
	
	delta.resize(VecSentence[num].size() + 1);
    psi.resize(VecSentence[num].size() + 1);

	//init
	iter = table.find(key);
	word1 = (*iter).second;
	for( unsigned int i = 0; i < word1.size(); ++i){
		double prob = UniProb(word1[i]);
		delta[0].push_back(prob);
	}
	//cout<<VecSentence[num].size()<<endl;
	//loop
	double prob = 0;
	for(unsigned int t = 1; t < VecSentence[num].size(); ++t){
		key = VecSentence[num][t];
		iter = table.find(key);
		word2 = (*iter).second;
		//for(int i = 0; i < word2.size(); ++i){
		//	cout<<word2[i];
		//}
		key = VecSentence[num][t-1];
		iter = table.find(key);
		word1 = (*iter).second;
		//for(int i = 0; i < word1.size(); ++i){
		//	cout<<word1[i];
		//}
		//cout<<endl;
		double max;
		string maxWord;
		for(unsigned int j = 0; j < word2.size(); ++j){
			max = negInf;
			maxWord = "\0";
			for(unsigned int i = 0; i < word1.size(); ++i){
				double aij = BiProb(word1[i], word2[j]);
				prob = delta[t - 1][i] + aij;
				if(prob > max){
					max = prob; 
					maxWord = word1[i];
				}
			}
			delta[t].push_back(max);
			psi[t].push_back(maxWord);
		}
	}
	
	//termination
	unsigned int T = VecSentence[num].size() - 1;
	key = VecSentence[num][T];
	iter = table.find(key);
	word1 = (*iter).second;
	//cout<<key<<endl;
	double max = negInf;
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

vector<string> TrackBack(vector< vector<double> > &delta, vector< vector<string> > &psi){
	vector<string> result;
	for(unsigned int t = delta.size() - 1; t > 0; --t){
		double max = negInf;
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
