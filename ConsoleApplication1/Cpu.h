#pragma once
#include<iostream>
#include"clsCommonTypes.h"
#include"RegesterFile.h"
#include"Memory.h"
#include"Instructions.h"
#include"Instruction validator.h"
#include<string>
using namespace std;
class CPU
{
private:
	int startingAddress;
	string IR;
	int PC;
	bool IsHalted;

public:
	void setStartingAddress(int StartingAddress)
	{
		startingAddress = StartingAddress;
	}
	
	int getStartingAddress()
	{
		return startingAddress;
	}

	void SetPC(int StartingAddress)
	{
		PC = StartingAddress;
	}

	void IncrementPC()
	{
		PC += 2;
	}

	int GetPC()
	{
		return PC;
	}

	void SetIR(int PC)
	{
		IR = Memory::Read(PC);
		IR += Memory::Read(PC + 1);
	}
	string GetIR()
	{
		return IR;
	}

	void SetHalted(bool Halted)
	{
		IsHalted = Halted;
	}

	bool GetHalted()
	{
		return IsHalted;
	}




	void SetRegester(RegisterEdit Regester)
	{
		RegesterFile::Write(Regester);
	}

	vector<string> GetRegester()
	{
		return RegesterFile::ReturnSnapShot();
	}



	int ReadExecutionMood()
	{
		cout << setw(37) << left << "" << "===========================================\n";
		cout << setw(37) << left << "" << "\t\t     Choose Execution Mood\n";
		cout << setw(37) << left << "" << "===========================================\n";
		cout << setw(37) << left << "" << "\t [1] Run at once.\n";
		cout << setw(37) << left << "" << "\t [2] Run Step by Step.\n";
		cout << setw(37) << left << "" << "===========================================\n";

		cout << setw(37) << left << "" << "Choose the filter you want ? [1 or 2]? ";
		short Choice = clsInputValidate::ReadShortNumberBetween(1, 2, "Enter Number between 1 to 2? ");
		return Choice;
	}


	

	//run the program from the starting address
	static void Run(int StartingAddress)
	{

		//Set the PC to the starting address 
		CPU cpu;
		cpu.SetPC(StartingAddress);
		cpu.setStartingAddress(StartingAddress);

		//Read execution mood from user
		int ExecutionMood = cpu.ReadExecutionMood();
		
		while (cpu.GetIR() != "C000") {

			cpu.SetIR(cpu.GetPC());
			cpu.IncrementPC();

			// Decode the instruction
			Instruction instruction(cpu.GetIR());
			instruction.Decode();
			instruction.Validate();
			if (!instruction.Validate().IsValid) {
				cout << setw(37) << left << "" << "===========================================\n";
				cout << setw(37) << left << "" << "\t Number of executed instructions : " << cpu.GetPC() - cpu.getStartingAddress() - 2 << "\n";
				cout << setw(37) << left << "" << "\t Status: Faulted.\n";
				cout << setw(37) << left << "" << "\t Error: " << instruction.Validate().ErrorMessage << "\n";
				cout << setw(37) << left << "" << "===========================================\n";
				
				cout << setw(37) << left << "" << "\n\tPress any key to go back to Main Menue...\n";

				system("pause>0");
				break;
			}


		}

	}
};