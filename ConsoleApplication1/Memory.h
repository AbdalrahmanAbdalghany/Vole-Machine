#pragma once
#include<iostream>
#include<string>
#include"clsCommonTypes.h"
#include<vector>
using namespace std;
class Memory
{
private:
	string memory[256] = {};
	
public:
	static string Read(string Address)
	{
		//Write the logic to read information from memory and return it
	}
	static void Write(MemoryEdit Memory)
	{
		//Write the logic to write this value into the address
	}
	static void LoadByBytes(string StartingAddress,string Bytes[])
	{
		//write the logic to insert this Bytes values from Array Starting with Starting Address
	}
	static void SnapShot()
	{
		//write the logic that print all memory
	}
};

