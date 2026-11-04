#pragma once
#include<string>
#include"clsCommonTypes.h"
#include <vector>
#include<algorithm>
using namespace std;
class RegesterFile
{
private:
	static int Regester[16];
public:
	static int Read(int Index)
	{
		return Regester[Index];
	}
	static void Write(RegisterEdit Register)
	{
		Regester[Register.Index] = Register.Value;
	}
	static void Reset()
	{
		fill(Regester, Regester + 16, 0);
	}
	static void SnapShot()
	{
		for (int i = 0; i < 16; i++)
		{
			cout <<"| " << hex << i << " :: " << Regester[i]<<" |"<< endl;
		}
	}
	static vector<string> ReturnSnapShot()
	{
		vector<string> snapshot;
		for (int i = 0; i < 16; i++)
		{
			snapshot.push_back(to_string(Regester[i]));
		}
		return snapshot;
	}

};

