#include "Reg_Exp.h"
#include<iostream>

using namespace std;

int main() {
	string s = "\\d{1,3}";
	RegExp myreg(s);
	string s1 = "2395@qq.com";
	bool result = myreg.RegExp_match(s1);
	cout << result;

	return 0;
}