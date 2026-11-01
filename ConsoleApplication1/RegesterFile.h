#pragma once
#include<string>
#include"clsCommonTypes.h"
#include <vector>
using namespace std;
class RegesterFile
{
private:
	string Regester[16];
public:
	static string Read(string Index)
	{
		//Write the logic that returns the value of this index

	}
	static void Write(RegisterEdit Register)
	{
		//Write the logic that insert that value into that indax
	}
	static void Reset()
	{
		//write the logic that resets the register
	}
	static void PrintSnapShot()
	{
		//write the logic to print all regester it table
	}
	static vector<string> ReturnSnapShot()
	{
		//Wtrite the logic that returns the all ragester by vector
	}

};

