#include "hmm.h"
#include <math.h>
#include <string.h>
#include <cstdlib>
#include <cstdio>

using namespace std;

#define MAX_NUM_MODEL 10

double alpha[MAX_SEQ][MAX_STATE];
double beta[MAX_SEQ][MAX_STATE];
double gam[MAX_SEQ][MAX_STATE];
double epsilon[MAX_SEQ][MAX_STATE][MAX_STATE];
double pi[MAX_STATE];
double sum_gam[MAX_STATE][MAX_OBSERV];
double sum_gam_tail[MAX_STATE];
double sum_epsilon[MAX_STATE][MAX_STATE];

int main(int argc, char* argv[])
{
    HMM model;
    int num_models = 0;
    char line[2000];
    int T, N, K = 0;

    //check input 
    if(argc != 5)
    {
        fprintf(stderr, "error input\n");
        exit(1);
    }

    FILE *seq_model = open_or_die(argv[3], "r");
    FILE *result_model = open_or_die(argv[4], "w+");

    // Initialize from arguments
    int num_iterations = strtol(argv[1], NULL, 10);
    loadHMM(&model, argv[2]);

    N = model.state_num;

    // Process 
    for(int num_it = 0; num_it < num_iterations; ++num_it)
    {
        rewind(seq_model);
        K = 0;
        for (int i = 0; i < N; ++i) 
        {
            pi[i] = sum_gam_tail[i] = 0;
            for (int j = 0; j < N; ++j)
                sum_epsilon[i][j] = 0;
            for (int j = 0; j < model.observ_num; ++j)
                sum_gam[i][j] = 0;
        }

        while(fscanf(seq_model, "%s", line) != EOF)
        {
            T = strlen(line);

            /* Forward Algorithm */
            for(int i = 0; i < N; i++) // initialization
                alpha[0][i] = model.initial[i] * model.observation[line[0] - 'A'][i];
            
            for(int t = 1; t < T; t++) // induction 
            {
                for(int j = 0; j < N; j++)
                {
                    double sum = 0;
                    for(int i = 0; i < N; i++)
                        sum += alpha[t-1][i] * model.transition[i][j];
                    
                    
                    alpha[t][j] = sum * model.observation[line[t] - 'A'][j];
                }
            }


            /* Backward Algorithm */
            for(int i = 0; i < N; i++) // initialization 
                beta[T - 1][i]=1;
        
            for(int t = T - 2; t >= 0; t--)    // induction 
            {
                for(int i = 0; i < N; i++)
                {
                    beta[t][i]=0;
                    for(int j = 0; j < N; j++)
                    {
                        beta[t][i] += model.transition[i][j] * model.observation[line[t + 1]-'A'][j]
                                   * beta[t + 1][j];
                    }
                }
            }
            

            /* gam */
            for(int t = 0; t < T; t++)
            {
                double sum = 0;
                for(int i = 0; i < N; i++)
                    sum += alpha[t][i] * beta[t][i];
                
                for(int i = 0; i < N; i++)
                    gam[t][i] = alpha[t][i] * beta[t][i]/sum;
            }


            /* Epsilon */
            for(int t = 0; t < T - 1; t++)
            {
                double sum = 0;
                for(int i = 0; i < N; i++)
                    for(int j = 0; j < N; j++)                   
                        sum += alpha[t][i] * model.transition[i][j] *
                                model.observation[line[t + 1]-'A'][j] * beta[t + 1][j];

                for(int i = 0; i < N; i++)
                    for(int j = 0; j < N; j++)
                    {
                        epsilon[t][i][j] = alpha[t][i] * model.transition[i][j]
                                           * model.observation[line[t + 1] - 'A'][j]
                                           * beta[t + 1][j] / sum;
                    }
            }

            for (int i = 0; i < N; ++i)
                pi[i] += gam[0][i];


            /* Accumulate */
            for (int i = 0; i < N; ++i) // gam
            {
                for (int t = 0; t < T; ++t) 
                {
                    sum_gam[i][line[t] - 'A'] += gam[t][i];
                }
                sum_gam_tail[i] += gam[T - 1][i];
            }

            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j) 
                {
                    double sum = 0;
                    for (int t = 0; t < T - 1; ++t)
                        sum += epsilon[t][i][j];

                    sum_epsilon[i][j] += sum;
                }
            
            K++;
        }

        /* Model */ 
        /*initial*/
        for (int i = 0; i < N; ++i)
            model.initial[i] = pi[i] / K;

        /*transition*/
        for (int i = 0; i < N; ++i) 
        {
            double sum = 0;

            for (int j = 0; j < model.observ_num; ++j)
                sum += sum_gam[i][j];
            sum -= sum_gam_tail[i];
            
            for (int j = 0; j < N; ++j)
                model.transition[i][j] = sum_epsilon[i][j] / sum;
        }

        /*observation*/
        for (int i = 0; i < N; ++i) 
        {
            double sum = 0;

            for (int j = 0; j < model.observ_num; ++j)
                sum += sum_gam[i][j];

            for (int j = 0; j < model.observ_num; ++j)
                model.observation[j][i] = sum_gam[i][j] / sum;
        }   


    }

    dumpHMM(result_model, &model);

    fclose(result_model);
}