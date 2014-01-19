// Doremi.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include "SoundManager.h"
using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{
	SoundManager sm;

	cout << "Hello ADX2LE MML" << endl;
	cout << "example : l4cderl8gec4d2 (enter)" << endl;

	for (;;){
		string input;
		cin >> input;
		cout << input << endl;
		sm.Play(input);
	}
	return 0;
}

