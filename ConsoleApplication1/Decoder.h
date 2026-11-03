#pragma once
#include<iostream>
#include"Cpu.h"
#include<string>
class Decoder
{
public: 
	static int DecodeOpcode(string Instruction) {
		int opcode = stoi(Instruction.substr(0, 1));
		return opcode;
	}
	static int DecodeOperand1(string Instruction) {
		int operand1 = stoi(Instruction.substr(1, 1));
		return operand1;
	}
	static int DecodeOperand2(string Instruction) {
		int operand2 = stoi(Instruction.substr(2, 1));
		return operand2;
	}
	static int DecodeOperand3(string Instruction) {
		int operand3 = stoi(Instruction.substr(3, 1));
		return operand3;
	}

};

