#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <string>

using namespace std;

void SegProcess(char*, char*, vector< vector<char> > &table);
void ExportZhuYin(vector< vector<char> > &table, FILE* file);

int main(int argc, char** argv){
    if(argc != 3){
        cerr<<"Input error!\n";
        exit(-1);
    }
    char word[10], zhuyin[50], prevword[10] = {0};
    vector< vector<char> > table;
    FILE* file, *filew;
    file = fopen(argv[1], "r");
    if(file == NULL) {
        cerr<<"Input file error!\n";
        exit(-1);
    }
    filew = fopen(argv[2], "w");
    while(!feof(file)){
        char *seg, prev[3];
        prev[0] = 0;
        fscanf(file, "%s", word);
        if(strcmp(word, prevword) == 0) continue;
        strcpy(prevword, word);
        fprintf(filew, "%s\t%s\n", word, word);
        fscanf(file, "%s", zhuyin);
        seg = strtok(zhuyin,"/");
        while(seg != NULL){
            char tmp[3];
            strncpy(tmp, seg, 2);
            tmp[2] = '\0';
            if(prev[0] == 0){
                strcpy(prev, tmp);
                SegProcess(word, tmp, table);
            }
            else if(strcmp(prev, tmp) != 0)
                SegProcess(word, tmp, table);
            seg = strtok(NULL,"/");
        }
    }
    ExportZhuYin(table, filew);
    
    fclose(file);
    fclose(filew);
    return 0;

}

void SegProcess(char* word, char* seg, vector< vector<char> > &table){
    if(table.size() == 0){
        vector<char> zhuyin;
        zhuyin.push_back(seg[0]);
        zhuyin.push_back(seg[1]);
        zhuyin.push_back(word[0]);
        zhuyin.push_back(word[1]);
        table.push_back(zhuyin);
        return;
    }
    for(int i = 0, size = table.size(); i < size; ++i){
        char head[2]; head[0] = table[i][0]; head[1] = table[i][1];
        if(strcmp(seg, head) == 0) {
            table[i].push_back(word[0]);
            table[i].push_back(word[1]);
            break;
        }
        else if(i == size - 1){
            vector<char> zhuyin;
            zhuyin.push_back(seg[0]);
            zhuyin.push_back(seg[1]);
            zhuyin.push_back(word[0]);
            zhuyin.push_back(word[1]);
            table.push_back(zhuyin);
        }
    }
}

void ExportZhuYin(vector< vector<char> > &table, FILE* file){
    for(int i = table.size() - 1; i >= 0; --i){
        fprintf(file, "%c%c\t", table[i][0], table[i][1]);
        for(int j = 2; j < table[i].size(); j += 2){
            fprintf(file, "%c%c ", table[i][j], table[i][j+1]);
        }
        fprintf(file, "\n");
    }
}