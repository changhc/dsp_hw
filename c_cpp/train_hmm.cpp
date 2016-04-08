#include "hmm.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
using namespace std;
double alpha[MAX_LINE][MAX_STATE], beta[MAX_LINE][MAX_STATE], epsilon[MAX_LINE][MAX_STATE][MAX_STATE], gam[MAX_LINE][MAX_STATE];
double sum_e[MAX_STATE][MAX_STATE], sum_g[MAX_OBSERV][MAX_STATE], sum_gm1[MAX_STATE], sum_pi[MAX_STATE];
void init(void);
void init_sum(void);
int main(int argc, char* argv[])
{
	//pi = hmm->initial[], A = hmm->transition[][], B = hmm->observation[][]
	HMM hmm;
	loadHMM( &hmm, argv[2] );
	//dumpHMM( stderr, &hmm );
	string data;
	int N = hmm.state_num, observN = hmm.observ_num;
	int iter;
	string n(argv[1]);
	stringstream ss;
	ss << n; ss >> iter; //cout<<iter<<endl;
	for(int x = 0; x < iter; x++){
		int count = 0;
        init_sum();
		fstream file(argv[3]);
		while(file >> data){
			count++;
			init();
			double sum = 0;
			for(int i = 0; i < N; i++){
				alpha[0][i] = hmm.initial[i] * hmm.observation[data[0]-65][i];
				beta[data.length()-1][i] = 1.0;
			}
			for(int t = 1; t < data.length(); t++){  
				for(int j = 0; j < N; j++){
					sum = 0; double sum2 = 0;
					for(int i = 0; i < N; i++){
						sum += alpha[t-1][i] * hmm.transition[i][j];
						sum2 += hmm.transition[j][i] * hmm.observation[data[data.length()-t]-65][i] * beta[data.length()-t][i];
					}
					alpha[t][j] = sum * hmm.observation[data[t]-65][j];
					beta[data.length()-1-t][j] = sum2;
				}
			}
			//gamma
			for(int t = 0; t < data.length(); t++){
				sum = 0;
				for(int i = 0; i < N; i++){
					sum += alpha[t][i] * beta[t][i];	//P(O|lambda)
				}
				for(int i = 0; i < N; i++){
					gam[t][i] = alpha[t][i] * beta[t][i] / sum;
				}
				//cout<<sum<<endl;
				if(t == 0) continue;
				sum = 0;
				for(int i = 0; i < N; i++){
					for(int j = 0; j < N; j++){
						sum += alpha[t-1][i] * hmm.transition[i][j] * hmm.observation[data[t]-65][j] * beta[t][j];
					}
				}
				//cout<<sum<<endl;
				
				for(int i = 0; i < N; i++){
					for(int j = 0; j < N; j++){
						epsilon[t-1][i][j] = alpha[t-1][i] * hmm.transition[i][j] * hmm.observation[data[t]-65][j] * beta[t][j] / sum;
					}
				}
			}
			for (int i = 0; i < N; i++){	//gamma
			    for (int t = 0; t < data.length(); t++){
			        sum_g[data[t]-65][i] += gam[t][i];
			    }
			    sum_gm1[i] += gam[data.length()-1][i];
			    sum_pi[i] = sum_pi[i] + gam[0][i];
			}

			for (int i = 0; i < N; i++)
			    for (int j = 0; j < N; j++) 
			    {
			        double sum = 0;
                    for (int t = 0; t < data.length() - 1; ++t)
                        sum += epsilon[t][i][j];

                    sum_e[i][j] += sum;
			    }
		}
		file.close();
		for(int i = 0; i < N; i++){
			double tmp = 0;
			for(int j = 0; j < observN; j++){
				tmp += sum_g[j][i];
			}
			tmp -= sum_gm1[i];
			for(int j = 0; j < N; j++){
				hmm.transition[i][j] = sum_e[i][j] / tmp;
				//hmm.observation[i][j] = sum_gcond[i][j] / sum_g[i];
			}
			hmm.initial[i] = sum_pi[i] / count;
		}
		for(int i = 0; i < N; i++){
			double tmp = 0;
			for(int j = 0; j < observN; j++)
				tmp += sum_g[j][i];
			for(int j = 0; j < observN; j++){
				hmm.observation[j][i] = sum_g[j][i] / tmp;
			}
		}
	}
	
	//dumpHMM(stderr, &hmm);
	fstream file;
	file.open(argv[4], ios::out);
	file << "initial: " << N << endl;
	for(int i = 0; i < N; i++) file << hmm.initial[i] << ' ';
	file << endl << endl;
	file << "transition: " << N << endl;
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			file << hmm.transition[i][j] << ' ';
		}
		file << endl;
	}
	file << endl;
	file << "observation: " << hmm.observ_num << endl;
	for(int i = 0; i < hmm.observ_num; i++){
		for(int j = 0; j < N; j++){
			file << hmm.observation[i][j] << ' ';
		}
		file << endl; 
	}
	file.close();
	//printf("%f\n", log(1.5) ); // make sure the math library is included
	return 0;
}

void init(void){
	for(int i = 0; i < MAX_LINE; i++){
		for(int j = 0; j < MAX_STATE; j++){
			alpha[i][j] = 0.0;
			beta[i][j] = 0.0;
			gam[i][j] = 0.0;
			for(int t = 0; t < MAX_STATE; t++)
				epsilon[j][i][t] = 0.0;
		}
	}
}
void init_sum(void){
	for(int i = 0; i < MAX_STATE; i++){
		for(int j = 0; j < MAX_STATE; j++){
			sum_e[i][j] = 0.0;
		}
		
		sum_gm1[i] = 0.0;
		sum_pi[i] = 0.0;
		for(int j = 0; j < MAX_OBSERV; j++){
			sum_g[j][i] = 0.0;
		}
	}	
}