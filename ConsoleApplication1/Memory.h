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

		for (int c = 0; c < 16; ++c)
		{
			cout << hex << uppercase << setw(3) << setfill(' ') << c;
		}

		for (int i = 0; i < 256; i += 16)
		{
			cout << hex << uppercase << setw(2) << setfill('0') << i << "   | ";

			for (int j = 0; j < 16; ++j)
			{
				int address = i + j;
				string val = memory[address];

				if (val.empty())
					cout << setw(3) << setfill(' ') << "--";
				else
					cout << setw(3) << setfill(' ') << val;
			}

			cout << dec << "\n";
		}
	}
};