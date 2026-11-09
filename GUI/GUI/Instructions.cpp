#include"Instructions.h"
#include"Decoder.h"
#include"Memory.h"
#include"RegesterFile.h"
#include"Cpu.h"


	void Instruction::Decode()
	{
		CurrentInstruction = CPU::GetIR();
		Parts.opcode = Decoder::DecodeOpcode(CurrentInstruction);
		Parts.operand1 = Decoder::DecodeOperand1(CurrentInstruction);
		Parts.operand2 = Decoder::DecodeOperand2(CurrentInstruction);
		Parts.operand3 = Decoder::DecodeOperand3(CurrentInstruction);
	}

	ValidationResult Instruction::Validate()
	{
		InstructionValidator validate;
		return validate.Validator(Parts.opcode, Parts.operand1, Parts.operand2, Parts.operand3);
	}

	void Instruction::Execute()
	{
		switch (Parts.opcode)
		{
		case 1: // Load_Memory
		{
			int XY = Decoder::GetXY(Parts.operand2, Parts.operand3);

			RegisterEdit Regester;
			Regester.Index = Parts.operand1;
			Regester.Value = Decoder::HexToInt(Memory::Read(XY));

			RegesterFile::Write(Regester);
			break;
		}

		case 2: // Load_Immediate
		{
			RegisterEdit Regester;
			Regester.Index = Parts.operand1;
			Regester.Value = Decoder::GetXY(Parts.operand2, Parts.operand3);
			RegesterFile::Write(Regester);
			break;
		}

		case 3: // Store
		{
			MemoryEdit memory;
			memory.Address = Decoder::GetXY(Parts.operand2, Parts.operand3);
			memory.Value = Decoder::IntToHex(RegesterFile::Read(Parts.operand1));
			Memory::Write(memory);
			break;
		}

		case 4: // Move
		{
			RegisterEdit Regester;
			Regester.Index = Parts.operand3;
			Regester.Value = RegesterFile::Read(Parts.operand2);
			RegesterFile::Write(Regester);
			break;
		}

		case 5: // Add_Integer
		{
			int result = RegesterFile::Read(Parts.operand2) + RegesterFile::Read(Parts.operand3);
			result = result & 0xFF;
			RegisterEdit Regester;
			Regester.Index = Parts.operand1;
			Regester.Value = result;
			RegesterFile::Write(Regester);
			break;
		}

		case 6: // Add_Float
		{
			int RS = RegesterFile::Read(Parts.operand2) & 0xFF;
			int RT = RegesterFile::Read(Parts.operand3) & 0xFF;
			double result = Decoder::HexToFloat(RS) + Decoder::HexToFloat(RT);
			RegisterEdit Regester;
			Regester.Index = Parts.operand1;
			Regester.Value = Decoder::floatToHex(result);
			RegesterFile::Write(Regester);
			break;
		}

		case 7: // OR
		{
			int result = RegesterFile::Read(Parts.operand2) | RegesterFile::Read(Parts.operand3);
			RegisterEdit Regester;
			Regester.Index = Parts.operand1;
			Regester.Value = result;
			RegesterFile::Write(Regester);
			break;
		}

		case 8: // AND
		{
			int result = RegesterFile::Read(Parts.operand2) & RegesterFile::Read(Parts.operand3);
			RegisterEdit Regester;
			Regester.Index = Parts.operand1;
			Regester.Value = result;
			RegesterFile::Write(Regester);
			break;
		}

		case 9: // XOR
		{
			int result = RegesterFile::Read(Parts.operand2) ^ RegesterFile::Read(Parts.operand3);
			RegisterEdit Regester;
			Regester.Index = Parts.operand1;
			Regester.Value = result;
			RegesterFile::Write(Regester);
			break;
		}

		case 10: // Rotate_Right
		{
			int Register = RegesterFile::Read(Parts.operand1);
			Register &= 0xFF;
			int x = Parts.operand3 % 8;
			int result = (Register >> x) | (Register << (8 - x)) & 0xFF;
			RegisterEdit Regester;
			Regester.Index = Parts.operand1;
			Regester.Value = result;
			RegesterFile::Write(Regester);
			break;
		}

		case 11: // Jump_Equal_R0
		{
			bool isEqual = (RegesterFile::Read(Parts.operand1) == RegesterFile::Read(0));
			if (isEqual)
			{
				CPU::SetPC(Decoder::GetXY(Parts.operand2, Parts.operand3));
			}
			break;
		}

		case 12: // Halt
		{
			CPU::SetHalted(true);
			break;
		}

		default:
		{
			cout << "Invalid Opcode!" << endl;
			break;
		}
		}
	}

