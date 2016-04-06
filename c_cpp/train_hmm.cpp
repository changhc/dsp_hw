#include "hmm.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
using namespace std;
int main(int argc, char* argv[])
{

	HMM hmms[5];
	load_models( argv[1], hmms, 5);
	dump_models( hmms, 5);
	
	//pi = hmm->initial[], A = hmm->transition[][], B = hmm->observation[][]
	HMM hmm_initial;
	loadHMM( &hmm_initial, argv[2] );
	dumpHMM( stderr, &hmm_initial );
	string data;
	int N = hmm_initial.state_num, observN = hmm_initial.observ_num;
	double sum_e[N][N] = {0}, sum_g[N] = {0}, sum_gcond[observN][N] = {0}, sum_gm1[N] = {0}, sum_pi[N] = {0};
	int count = 0, iter;
	string n(argv[1]);
	stringstream ss;
	ss << n; ss >> iter; cout<<iter<<endl;
	for(int x = 0; x < iter; x++){
		fstream file(argv[3]);
		while(file >> data){
			count++;
			double alpha[data.length()][N], beta[data.length()][N], epsilon[data.length()][N][N], gamma[data.length()][N];
			double sum = 0;
			for(int i = 0; i < N; i++){
				alpha[0][i] = hmm_initial.initial[i] * hmm_initial.observation[data[0]-65][i];
				beta[data.length()-1][i] = 1.0;
			}
			for(int t = 1; t < data.length(); t++){  
				for(int j = 0; j < N; j++){
					sum = 0; double sum2 = 0;
					for(int i = 0; i < N; i++){
						sum += alpha[i][t-1] * hmm_initial.transition[i][j];
						sum2 += hmm_initial.transition[j][i] * hmm_initial.observation[data[data.length()-t]-65][i] * beta[data.length()-t][i];
					}
					alpha[t][j] = sum * hmm_initial.observation[data[t]-65][j];
					beta[data.length()-1-t][j] = sum2;
				}
			}
			for(int t = 0; t < data.length(); t++){
				sum = 0;
				for(int i = 0; i < N; i++){
					sum += alpha[t][i] * beta[t][i];
				}
				for(int i = 0; i < N; i++){
					gamma[t][i] = alpha[t][i] * beta[t][i] / sum;
				}
				//cout<<sum<<endl;
				sum = 0;
				for(int j = 0; j < N; j++){
					for(int i = 0; i < N; i++){
						sum += alpha[t-1][i] * hmm_initial.transition[i][j] * hmm_initial.observation[data[t]-65][j] * beta[t][j];
					}
				}
				//cout<<sum<<endl;
				if(t == 0) continue;
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
	}
	
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			hmm_initial.transition[i][j] = sum_e[i][j] / sum_gm1[i];
			//hmm_initial.observation[i][j] = sum_gcond[i][j] / sum_g[i];
		}
		hmm_initial.initial[i] = sum_pi[i] / count;
	}
	for(int i = 0; i < hmm_initial.observ_num; i++){
		for(int j = 0; j < N; j++){
			hmm_initial.observation[i][j] = sum_gcond[i][j] / sum_g[j];
		}
	}
	dumpHMM(stderr, &hmm_initial);
	fstream file;
	file.open(argv[4], ios::out);
	file << "initial: " << N << endl;
	for(int i = 0; i < N; i++) file << hmm_initial.initial[i] << ' ';
	file << endl << endl;
	file << "transition: " << N << endl;
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			file << hmm_initial.transition[i][j] << ' ';
		}
		file << endl;
	}
	file << endl;
	file << "observation: " << hmm_initial.observ_num << endl;
	for(int i = 0; i < hmm_initial.observ_num; i++){
		for(int j = 0; j < N; j++){
			file << hmm_initial.observation[i][j] << ' ';
		}
		file << endl;
	}
	file.close();
	printf("%f\n", log(1.5) ); // make sure the math library is included
	return 0;
}