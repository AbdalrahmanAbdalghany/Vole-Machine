#pragma once
#include<string>
#include"clsCommonTypes.h"
#include <vector>
#include<algorithm>
#include<iomanip>
using namespace std;
class RegesterFile
{
private:
	inline static int Regester[16] = {};
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
		cout << "+---------+----------------------+\n";
		cout << "| Address | Value                |\n";
		cout << "+---------+----------------------+\n";

		for (int i = 0; i < 16; i++)
		{
			cout << "|   " << hex << uppercase << setfill('0') << setw(2) << i << "    | ";

			cout << dec << setfill(' ') << setw(20) << left << Regester[i] << " |\n";
		}

		cout << "+---------+----------------------+\n";
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

