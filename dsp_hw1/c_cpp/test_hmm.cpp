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
	//dump_models( hmms, 5);
	
	//pi = hmm->initial[], A = hmm->transition[][], B = hmm->observation[][]
	fstream file(argv[2]), output(argv[3], ios::out);
	string data;
	int N = hmms[0].state_num;
	while(file >> data){
		double prob[5] = {0};
		for(int z = 0; z < 5; z++){
			double delta[data.length()][N] = {0};
			for(int i = 0; i < N; i++){
				delta[0][i] = hmms[z].initial[i] * hmms[z].observation[data[0]-65][i];
			}
			for(int t = 1; t < data.length(); t++){
				for(int j = 0; j < N; j++){
					double max = 0;
					for(int i = 0; i < N; i++){
						double tmp = delta[t-1][i] * hmms[z].transition[i][j];
						if(tmp > max) max =  tmp;
					}
					delta[t][j] = max * hmms[z].observation[data[t]-65][j];
					if(t != data.length()-1) continue;
					if(delta[t][j] > prob[z]) prob[z] = delta[t][j];
				}
			}
		}
		double max = 0; int id;
		for(int i = 0; i < 5; i++){
			if(prob[i] > max){
				max = prob[i]; id = i;
			}
		}
		output << hmms[id].model_name << endl;//"\t" << max <<endl;
	}
	file.close();
	output.close();
	//printf("%f\n", log(1.5) ); // make sure the math library is included
	return 0;
}