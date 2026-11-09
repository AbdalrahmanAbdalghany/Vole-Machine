#pragma once
#include<iostream>
#include"clsCommonTypes.h"
#include"Instruction validator.h"
#include<string>
class Instruction 
{
private:
	inline static string CurrentInstruction = "";
	struct DecodedInstruction
	{
		int opcode;
		int operand1;
		int operand2;
		int operand3;
	};

	inline static DecodedInstruction Parts;

public:
	
    static void Decode();
    static ValidationResult Validate();
	static void Execute();

};