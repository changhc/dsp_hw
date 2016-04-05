#include "hmm.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
int main()
{
/*
	HMM hmms[5];
	load_models( "modellist.txt", hmms, 5);
	dump_models( hmms, 5);
*/	
	//pi = hmm->initial[], A = hmm->transition[][], B = hmm->observation[][]
	int N = 6;
	HMM hmm_initial;
	loadHMM( &hmm_initial, "model_init.txt" );
	dumpHMM( stderr, &hmm_initial );
	fstream file("seq_model_01.txt");
	string data;
	double sum_e[N][N] = {0}, sum_g[N] = {0}, sum_gcond[N][N] = {0}, sum_gm1[N] = {0}, sum_pi[N] = {0};
	int count = 0;
	while(file >> data){
		count++;
	file >> data;
	//vector<vector<double> > alpha(N);
	//vector<vector<double> > beta(N);
	double alpha[data.length()][N], beta[data.length()][N], epsilon[data.length()][N][N], gamma[data.length()][N];
	double sum = 0;
	for(int i = 0; i < N; i++){
		double tmp = hmm_initial.initial[i] * hmm_initial.observation[data[0]-65][i];
		//alpha[i].push_back(tmp);
		//beta[i].push_back(1.0);
		alpha[0][i] = tmp;
		beta[data.length()-1][i] = 1.0;
	}
	for(int i = 0; i < N; i++){
		sum += alpha[0][i] * beta[0][i];
	}
	for(int i = 0; i < N; i++){
		gamma[0][i] = alpha[0][i] * beta[0][i] / sum;
	}
	
	
	for(int t = 1; t < data.length(); t++){  
		for(int j = 0; j < N; j++){
			sum = 0; double sum2 = 0;
			for(int i = 0; i < N; i++){
				sum += alpha[i][t-1] * hmm_initial.transition[i][j];
				sum2 += hmm_initial.transition[j][i] * hmm_initial.observation[data[t]-65][i] * beta[i][t+1];
			}
			double tmp = sum * hmm_initial.observation[data[t]-65][j];
			alpha[t][j] = tmp;
			beta[data.length()-1-t][j] = sum2;
			//alpha[j].push_back(tmp);
			//beta[j].push_back(sum2);
		}
		sum = 0;
		for(int i = 0; i < N; i++){
			sum += alpha[t][i] * beta[t][i];
		}
		for(int i = 0; i < N; i++){
			gamma[t][i] = alpha[t][i] * beta[t][i] / sum;
		}
		cout<<sum<<endl;
		sum = 0;
		for(int j = 0; j < N; j++){
			for(int i = 0; i < N; i++){
				sum += alpha[t-1][i] * hmm_initial.transition[i][j] * hmm_initial.observation[data[t]-65][j] * beta[t][j];
			}
		}
		//cout<<sum<<endl;
		for(int j = 0; j < N; j++){
			for(int i = 0; i < N; i++){
				epsilon[t-1][i][j] = alpha[t-1][i] * hmm_initial.transition[i][j] * hmm_initial.observation[data[t]-65][j] * beta[t][j] / sum;
			}
		}
	}

	//iteration result
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			for(int t = 0; t < data.length(); t++){
				if((data[t]-65) == i) sum_gcond[i][j] += gamma[t][i];
				if(j == 0) sum_g[i] += gamma[t][i];
				if(t == data.length()-1) continue;
				sum_e[i][j] += epsilon[t][i][j];
				sum_gm1[i] = sum_g[i];
			}
		}
		sum_pi[i] += gamma[0][i];
	}
	
	}
	file.close();
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			hmm_initial.transition[i][j] = sum_e[i][j] / sum_gm1[i];
			hmm_initial.observation[i][j] = sum_gcond[i][j] / sum_g[i];
		}
		hmm_initial.initial[i] = sum_pi[i];
	}
	dumpHMM(stderr, &hmm_initial);
	printf("%f\n", log(1.5) ); // make sure the math library is included
	return 0;
}