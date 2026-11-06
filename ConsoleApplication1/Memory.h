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
    	const int AddressWidth = 12;
    	const int ValueWidth = 12;

    // Header
    	cout << setw(AddressWidth) << "Address"
         	<< " | "
         	<< setw(ValueWidth) << "Value"
         	<< '\n';

    // Separator
    	cout << string(AddressWidth, '-')
         	<< "-+-"
         	<< string(ValueWidth, '-')
         	<< '\n';

    // Print all 256 memory cells
    	for (int i = 0; i < 256; ++i)
    	{
        	string Value = memory[i];

        	if (Value.empty())
            	Value = "00";

        	cout << setw(AddressWidth) << hex  << uppercase << setw(2) << i
             	<< " | "
             	<< setw(ValueWidth) << Value
            	<< '\n';
    	}

		cout << dec << "\n";

	}
};
