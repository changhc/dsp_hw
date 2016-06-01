#include <stdio.h>
#include <iostream>
#include <string>
#include "Ngram.h"

int main(int argc, char *argv[])
{
    FILE *file;
    file = fopen("a.txt", "r");
    while(!feof(file)){
        string tmp;
        char sentence[10000];
        fgets(sentence, 10000, file);
        cout<<sentence<<endl;
        //seg = strtok(sentence," ");

    }
    fclose(file);
    return 0;
}

