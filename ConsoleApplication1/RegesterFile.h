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
	inline static string HEX[16] = { "0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F" };
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

