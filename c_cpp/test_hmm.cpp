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
	//while(file >> data){
	file >> data;
	//vector<vector<double> > alpha(N);
	//vector<vector<double> > beta(N);
	double alpha[data.length()][N], beta[data.length()][N], epsilon[data.length()][N][N], gamma[data.length()][N];
	for(int i = 0; i < N; i++){
		double tmp = hmm_initial.initial[i] * hmm_initial.observation[data[0]-65][i];
		//alpha[i].push_back(tmp);
		//beta[i].push_back(1.0);
		alpha[0][i] = tmp;
		beta[data.length()-1][i] = 1.0;
	}
	
	
	for(int t = 1; t < data.length(); t++){  
		for(int j = 0; j < N; j++){
			double sum = 0, sum2 = 0;
			for(int i = 0; i < N; i++){
				sum += alpha[i][t-1] * hmm_initial.transition[i][j];
				sum2 += hmm_initial.transition[j][i] * hmm_initial.observation[data[t]-65][i] * beta[i][t-1];
			}
			double tmp = sum * hmm_initial.observation[data[t]-65][j];
			alpha[t][j] = tmp;
			beta[data.length()-t][j] = sum2;
			//alpha[j].push_back(tmp);
			//beta[j].push_back(sum2);
		}

	}
	//}
	file.close();
	printf("%f\n", log(1.5) ); // make sure the math library is included
	return 0;
}