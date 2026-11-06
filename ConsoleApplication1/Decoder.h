#pragma once
#include<iostream>
#include<sstream>
#include<iomanip>
#include<string>
#include <cmath>
class Decoder
{
public: 
	static int HexToInt(string Hex)
	{
		return stoi(Hex, nullptr, 16);
	}
	static string IntToHex(int num)
	{
		ostringstream HexValue;
		HexValue << uppercase << hex << setw(2) << setfill('0') << (num & 0xFF);
		return HexValue.str();
	}
	static int DecodeOpcode(string Instruction) {
		int opcode = HexToInt(Instruction.substr(0, 1));
		return opcode;
	}
	static int DecodeOperand1(string Instruction) {
		int operand1 = HexToInt(Instruction.substr(1, 1));
		return operand1;
	}
	static int DecodeOperand2(string Instruction) {
		int operand2 = HexToInt(Instruction.substr(2, 1));
		return operand2;
	}
	static int DecodeOperand3(string Instruction) {
		int operand3 = HexToInt(Instruction.substr(3, 1));
		return operand3;
	}

	static int GetXY(int a, int b) {
		return (a << 4) | b;
	}

	static double HexToFloat(int a) {
		int sign = (a & 0x80) ? -1 : 1;
		int exponent = (a >> 4) & 0x7;
		exponent -= 4; 
		int mantissa = (a & 0xF);
		double floatValue = sign * (mantissa / 16.0) * pow(2.0, exponent);
		return floatValue;
	}

	static int floatToHex(double a) {
		int sign = (a < 0) ? 1 : 0;
		a = abs(a);
		int exponent = 4;
		while (a >= 1.0 && exponent < 7) {
			a /= 2.0;
			exponent++;
		}
		while (a < .0625 && exponent > 0) {
			a *= 2.0;
			exponent--;
		}
		int mantissa = static_cast<int>(a * 16.0);
		if (mantissa > 15) {
			mantissa = 15;
		}
		if (mantissa < 0) {
			mantissa = 0;
		}
		int hexValue = (sign << 7) | ((exponent) << 4) | mantissa;
		return hexValue & 0xFF;
	}

};

