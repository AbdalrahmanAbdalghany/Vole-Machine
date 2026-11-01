#pragma once
#include <iostream>
#include<string>
#include"Memory.h"
#include"clsCommonTypes.h"
#include"RegesterFile.h"
using namespace std;
class VoleMechine
{
public:
	static void PropramLoader(string Path , string StartingAddress)
	{
		//write the logic to load program commands from .txt file,and stor it into array 
		//sent the Array And Stating Address to
		// 
		// 
		//Memory::LoadByBytes(StartingAddress,Bytes);
	}
	static void WriteMemory(MemoryEdit Byte)
	{
		Memory::Write(Byte);
	}
	static void WriteRegester(RegisterEdit Byte)
	{
		RegesterFile::Write(Byte);
	}
	static void SnapShot()
	{
		int Answer;
		do
		{
			system("cls");
			cout << "Choose what you want to Show" << endl;
			cout << "[1]Memory SnapShot" << endl;
			cout << "[2]Regrster Snapshot" << endl;
			cout << "[3]Back to main menue" << endl;
			cout << "Enter numper Between 1 to 3" << endl;
			Answer = clsInputValidate::ReadIntNumberBetween(1, 3);

			if (Answer == 1)
			{
				system("cls");
				Memory::SnapShot();
				cout << setw(37) << left << "" << "\n\tPress any key to go back to SnapShot Menue...\n";

				system("pause>0");
			}
			else if (Answer==2)
			{
				system("cls");
				RegesterFile::SnapShot();
				cout << setw(37) << left << "" << "\n\tPress any key to go back to SnapShot Menue...\n";

				system("pause>0");
			}
			else
			{
				system("cls");
				break;
			}

		} while (true);
	}
};

