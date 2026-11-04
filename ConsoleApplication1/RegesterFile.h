#pragma once
#include<string>
#include"clsCommonTypes.h"
#include <vector>
#include<algorithm>
using namespace std;
class RegesterFile
{
private:
	static string Regester[16];
public:
	static string Read(string Index)
	{
		return Regester[stoi(Index)];
	}
	static void Write(RegisterEdit Register)
	{
		Regester[stoi(Register.Index)] = Register.Value;
	}
	static void Reset()
	{
		fill(Regester, Regester + 16, "0000");
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
			snapshot.push_back(Regester[i]);
		}
		return snapshot;
	}

};

