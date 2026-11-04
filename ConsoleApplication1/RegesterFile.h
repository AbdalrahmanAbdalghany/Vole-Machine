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
		if (Index < 0 || Index >= 16)
		{
			return 0;
		}

		return Regester[Index];
	}
	static bool Write(RegisterEdit Register)
	{
		if(Register.Index < 0 || Register.Index >=16)
		{
			return false;
		}

		Regester[Register.Index] = Register.Value;

		return true;
	}
	static void Reset()
	{
		fill(Regester, Regester + 16, 0);
	}
	static void SnapShot()
	{
		for (int i = 0; i < 16; i++)
		{
			cout <<"| " << hex << uppercase << i << " :: " << Regester[i]<<" |"<< endl;
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

