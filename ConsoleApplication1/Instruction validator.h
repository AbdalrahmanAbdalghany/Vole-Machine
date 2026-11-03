#pragma once
#include<iostream>
#include"clsCommonTypes.h"
#include<string>

class InstructionValidator
{
public :
		ValidationResult Validator(const int& opcode, const int& operand1, const int& operand2, const int& operand3) {
		if (opcode < 1 || opcode > 12)
		{
			return { false, "Invalid Opcode" };
		}
		if (operand1 < 0 || operand1 > 15)
		{
			return { false, "Invalid Operand1" };
		}
		if (operand2 < 0 || operand2 > 15)
		{
			return { false, "Invalid Operand2" };
		}
		if (operand3 < 0 || operand3 > 15)
		{
			return { false, "Invalid Operand3" };
		}
		if (opcode == 4 && operand1 != 0)
		{
			return { false, "Invalid Operand1 must be 0 for Move" };
		}
		
		if (opcode == 10 && operand2 != 0)
		{
			return { false, "Invalid Operand2 must be 0 for Rotate_Right" };
		}
		if (opcode == 12 && (operand1 != 0 || operand2 != 0 || operand3 != 0))
		{
			return { false, "All operands must be 0 for Halt" };
		}

		return { true, "Instruction is valid" };
	}
};