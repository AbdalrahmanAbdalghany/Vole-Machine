#pragma once
#include <iostream>
#include <string>
#include "clsCommonTypes.h"
#include <vector>
#include <stdexcept>
#include <iomanip>

using namespace std;

class Memory
{
private:
	inline static string memory[256] = {};

public:
	static string Read(int Address)
	{
		if (Address < 0 || Address >= 256)
		{
			throw out_of_range("Invalid Memory Address");
		}

		return memory[Address];
	}

	static void Write(MemoryEdit Edit)
	{
		int Address = Edit.Address;
		string Value = Edit.Value;

		if (Address < 0 || Address >= 256)
		{
			throw out_of_range("Invalid Memory Address");
		}

		memory[Address] = Value;
	}

	static void LoadByBytes(int StartingAddress, vector<string> Bytes)
	{
		if (StartingAddress < 0 || (StartingAddress + Bytes.size()) > 256)
		{
			throw out_of_range("Memory out of bounds or invalid starting address.");
		}
		int Address = StartingAddress;

		for (const string& Byte : Bytes)
		{
			Write({ Address, Byte });
			Address++;
		}
	}

	static void SnapShot()
	{
		cout << "+---------+----------------------+\n";
		cout << "| Address | Value                |\n";
		cout << "+---------+----------------------+\n";

		for (int i = 0; i < 256; ++i)
		{
			string Value = memory[i];

			if (Value.empty())
				Value = "00";

			cout << "|   " << hex << uppercase << setfill('0') << setw(2) << i << "    | ";

			cout << dec << setfill(' ') << setw(20) << left << Value << " |\n";
		}

		cout << "+---------+----------------------+\n";
	}
};
