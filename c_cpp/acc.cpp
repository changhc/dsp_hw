#include <string>
#include <fstream>
#include <iostream>
using namespace std;
#ifndef MAX_LINE
#	define MAX_LINE 	256
#endif

int main(){
	fstream test("result.txt"), answer("testing_answer.txt");
	string s, ans;
	double count = 0, correct = 0;
	while(test >> s, answer >> ans){
		count++;
		if(s.compare(ans) == 0) correct++;
	}
	test.close(); answer.close();
	test.open("acc.txt", ios::out);
	test << correct/count;
	test.close();
	cout<<correct/count<<endl;
	return 0;
}