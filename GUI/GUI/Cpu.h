#pragma once
#include<iostream>
#include"clsCommonTypes.h"
#include"RegesterFile.h"
#include"Memory.h"
#include"Instructions.h"
#include"clsInputValidate.h"
#include<string>
using namespace std;
class CPU
{
private:
	inline static int startingAddress = 0;
	inline static string IR = "";
	inline static int PC = 0;
	inline static int executedInstructionsCount = 0;
	inline static bool IsHalted = false;

public:
	static void setStartingAddress(int StartingAddress)
	{
		startingAddress = StartingAddress;
	}
	
	static int getStartingAddress()
	{
		return startingAddress;
	}

	static void SetPC(int StartingAddress)
	{
		PC = StartingAddress;
	}

	static void IncrementPC()
	{
		PC += 2;
	}

	static int GetPC()
	{
		return PC;
	}

	static void SetIR(int PC)
	{
		IR = Memory::Read(PC);
		IR += Memory::Read(PC + 1);
	}
	static string GetIR()
	{
		return IR;
	}

	static void SetHalted(bool Halted)
	{
		IsHalted = Halted;
	}

	static bool GetHalted()
	{
		return IsHalted;
	}



	static int ReadExecutionMood()
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

	static void GetScreen() {
		cout << setw(37) << left << "" << "===========================================\n";
		cout << setw(37) << left << "" << "\t\t    Status\n";
		cout << setw(37) << left << "" << "===========================================\n";
		cout << setw(37) << left << "" << "\t Program Counter : " << hex << uppercase << CPU::GetPC() << endl;
		cout << setw(37) << left << "" << "\t Instruction : " << CPU::GetIR() << endl;
		cout << setw(37) << left << "" << "\t Number of executed instructions : " << dec << executedInstructionsCount << "\n";
		cout << setw(37) << left << "" << "\t Status: " << (CPU::GetHalted() ? "Halted" : "Running") << "\n";
		cout << setw(37) << left << "" << "===========================================\n";
		cout << setw(37) << left << "" << "===========================================\n";
		cout << setw(37) << left << "" << "\t\t    Screen\n";
		cout << setw(37) << left << "" << "===========================================\n";
		cout << setw(37) << left << "" <<  Memory::Read(255) << "\n";
		cout << setw(37) << left << "" << "===========================================\n";

	}


	

	//run the program from the starting address
	static void Run(int StartingAddress)
	{
		CPU::SetHalted(false);
		CPU::executedInstructionsCount = 0;
		Memory::Write({ 255, "" }); 
		//Set the PC to the starting address 
		CPU::SetPC(StartingAddress);
		CPU::setStartingAddress(StartingAddress);

		//Read execution mood from user
		int ExecutionMood = CPU::ReadExecutionMood();
		
		while (!CPU::GetHalted()) {
			try {
				CPU::SetIR(CPU::GetPC());
				CPU::IncrementPC();

				Instruction::Decode();
				auto Result = Instruction::Validate();

				// Invalid instruction is reported through the same Faulted path
				// by throwing, so there is only one place that prints the error.
				if (!Result.IsValid)
					throw runtime_error(Result.ErrorMessage);

				Instruction::Execute();
				executedInstructionsCount++;

				if (ExecutionMood == 2 && !CPU::GetHalted()) {
					CPU::GetScreen();
					cout << setw(37) << left << "" << "\n\tPress any key to continue...\n";
					system("pause>0");
				}
			}
			catch (const exception& Error) {
				cout << setw(37) << left << "" << "===========================================\n";
				cout << setw(37) << left << "" << "\t Number of executed instructions : " << dec << executedInstructionsCount << "\n";
				cout << setw(37) << left << "" << "\t Status: Faulted.\n";
				cout << setw(37) << left << "" << "\t Error: " << Error.what() << "\n";
				cout << setw(37) << left << "" << "===========================================\n";

				// break is required: without it the loop would keep running
				// with the same bad instruction and never end.
				break;

			}

		}
		CPU::GetScreen();
		cout << setw(37) << left << "" << "\n\tPress any key to continue...\n";
		system("pause>0");

	}
};