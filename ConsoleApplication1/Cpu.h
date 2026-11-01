#pragma once
#include<iostream>
#include"clsCommonTypes.h"
#include"RegesterFile.h"
#include"Memory.h"
#include<string>
using namespace std;
class CPU
{
private:
	string IR;
	string PC;
public:
	string GetPC()
	{
		return PC;
	}
	string GetIR()
	{
		return IR;
	}
	void SetRegester(RegisterEdit Regester)
	{
		RegesterFile::Write(Regester);
	}
	vector<string> GetRegester()
	{
		return RegesterFile::ReturnSnapShot();
	}
	void Run()
	{
		//?????????????
	}
};

