#pragma once
#include<iostream>
#include <string>
using namespace std;
class clsInputValidate
{
public:

	static bool IsNumperBetween(int num, int low, int high)
	{
		if (num > low && num < high)
		{
			return true;
		}
		return false;
	}
	static bool IsNumperBetween(double num, double low, double high)
	{
		if (num > low && num < high)
		{
			return true;
		}
		return false;
	}
	static int ReadIntNumber(string massage = "invalid numper , Enter valid one ")
	{
		int num;
		cin >> num;
		while (cin.fail())
		{
			cin.clear();
			cin.ignore(numeric_limits<std::streamsize>::max(), '\n');
			cout << massage << endl;
			cin >> num;
		}
		return num;
	}
	static double ReadDbNumber(string massage = "invalid numper , Enter valid one ")
	{
		double num;
		cin >> num;
		while (cin.fail())
		{
			cin.clear();
			cin.ignore(numeric_limits<std::streamsize>::max(), '\n');
			cout << massage << endl;
			cin >> num;
		}
		return num;
	}
	static int ReadIntNumberBetween(int LRange, int HRange, string massage = "invalid numper , Enter valid one ")
	{
		int num = ReadIntNumber("invalid numper , Enter valid one ");
		while (num <LRange || num>HRange)
		{
			cout << massage << endl;
			num = ReadIntNumber("invalid numper , Enter valid one ");
		}
		return num;
	}
	static int ReadShortNumberBetween(int LRange, int HRange, string massage)
	{
		short num = ReadIntNumber("invalid numper , Enter valid one ");
		while (num <LRange || num>HRange)
		{
			cout << massage << endl;
			num = ReadIntNumber("invalid numper , Enter valid one ");
		}
		return num;
	}
	static double ReadDbNumberBetween(int LRange, int HRange, string massage)
	{
		double num = ReadDbNumber("invalid numper , Enter valid one ");
		while (num <LRange || num>HRange)
		{
			cout << massage << endl;
			num = ReadDbNumber("invalid numper , Enter valid one ");
		}
		return num;
	}
	static string Readstring()
	{
		string  S1 = "";
		// Usage of std::ws will extract allthe whitespace character
		getline(cin >> ws, S1);
		return S1;
	}

	static int _ReadHexAddress(const string& Message)
	{
		int Address = 0;
		bool IsValid = false;

		do {
			cout << Message;

			string Input = clsInputValidate::Readstring();

			try {
				size_t AddressLength = 0;
				
				Address = stoi(Input, &AddressLength, 16);

				if (AddressLength != Input.size())
					throw invalid_argument("Extra characters");

				if (Address < 0 || Address > 254)
					throw out_of_range("Address out of memory range");

				IsValid = true;
			}
			catch (const exception&) {
				cout << "Invalid input, please enter a hex number between 00 and FE.\n";
			}
		} while (!IsValid);

		return Address;
	}
};

