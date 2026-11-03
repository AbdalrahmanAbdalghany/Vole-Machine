#pragma once
#include<iostream>
#include"clsCommonTypes.h"
#include"Instruction validator.h"
#include"Decoder.h"
#include"Memory.h"
#include"RegesterFile.h"
#include<string>
class Instruction 
{
private:
	string CurrentInstruction ;
	struct DecodedInstruction
	{
		int opcode;
		int operand1;
		int operand2;
		int operand3;
	}Parts;

public:
	Instruction(string IR)
	{
		CurrentInstruction = IR;
	}
	void Decode()
	{
		Parts.opcode = Decoder::DecodeOpcode(CurrentInstruction);
		Parts.operand1 = Decoder::DecodeOperand1(CurrentInstruction);
		Parts.operand2 = Decoder::DecodeOperand2(CurrentInstruction);
		Parts.operand3 = Decoder::DecodeOperand3(CurrentInstruction);
	}

	ValidationResult Validate()
	{
		InstructionValidator validate;
		return validate.Validator(Parts.opcode, Parts.operand1, Parts.operand2, Parts.operand3);
	}
	
	void Execute()
	{
		if (Parts.opcode == 1) // Load_Memory
		{
			
		}
		else if (Parts.opcode == 2) // Load_Immdiate
		{
			//Write the logic to load immediate
		}
		else if (Parts.opcode == 3) // Store
		{
			//Write the logic to store
		}
		else if (Parts.opcode == 4) // Move
		{
			//Write the logic to move
		}
		else if (Parts.opcode == 5) // Add_intger
		{
			//Write the logic to add integer
		}
		else if (Parts.opcode == 6) // Add_Float
		{
			//Write the logic to add float
		}
		else if (Parts.opcode == 7) // OR
		{
			//Write the logic to OR
		}
		else if (Parts.opcode == 8) // And
		{
			//Write the logic to And
		}
		else if (Parts.opcode == 9) // XOR
		{
			//Write the logic to XOR
		}
		else if (Parts.opcode == 10) // Rotate_Right
		{
			//Write the logic to Rotate_Right
		}
		else if (Parts.opcode == 11) // Jump_Equal_R0
		{
			//Write the logic to Jump_Equal_R0
		}
		else if (Parts.opcode == 12) // Halt
		{
			//Write the logic to Halt
		}
	}

};