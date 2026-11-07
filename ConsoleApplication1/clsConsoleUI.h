#pragma once
#include<iostream>
#include<iomanip>
#include"clsInputValidate.h"
#include"VoleMechine.h"
#include "clsCommonTypes.h"
#include "Cpu.h"
using namespace std;
class clsConsoleUI
{
private:
    enum eUserInput
    {
        eLoad_Program=1,
        eEdit_Register=2,
        eEdit_Memory=3,
        View_Stat=4,
        Run_Program=5,
        EXIT=6
	};
    
    static short _ReadMainMenueOption()
    {
        cout << setw(37) << left << "" << "Choose the filter you want ? [1 to 6]? ";
        short Choice = clsInputValidate::ReadShortNumberBetween(1, 6, "Enter Number between 1 to 6? ");
        return Choice;
    }
    static  void _GoBackToMainMenue()
    {
        cout << setw(37) << left << "" << "\n\tPress any key to go back to Main Menue...\n";

        system("pause>0");
        ShowMainScreen();
    }

private:
    static string _ReadPath()
    {
        cout << "Enter Program Path" << endl;
        return clsInputValidate::Readstring();
    }
    
    static void _UiLoadProgaram()
    {
        string path = _ReadPath();
        int StartingAddress = clsInputValidate::_ReadHexAddress("Enter address (2-digit hexadecimal): ");
        
        try
        {
         
            if (VoleMechine::PropramLoader(path, StartingAddress))
                cout << "Program Loaded Successfully\n";
            else
                cout << "Failed to Load Program! File Not Found.\n";
        }
        catch (const exception& Error)
        {
            
            cout << "Load failed: " << Error.what() << "\n";
        }
    }
    static MemoryEdit ReadMemoryEdit()
    {
        MemoryEdit Memory;
        cout << "Enter the Address you want edit" << endl;
        Memory.Address = clsInputValidate::ReadIntNumber();
        cout << "Enter the value" << endl;
        Memory.Value = clsInputValidate::Readstring();
        return Memory;
    }
    static RegisterEdit ReadRegesterEdit()
    {
        RegisterEdit Regester;
        cout << "Enter the Address you want edit" << endl;
        Regester.Index = clsInputValidate::ReadIntNumber();
        cout << "Enter the value" << endl;
        Regester.Value = clsInputValidate::ReadIntNumber();
        return Regester;
    }
    static void _UiEditMemory()
    {
        MemoryEdit Memory = ReadMemoryEdit();
        VoleMechine::WriteMemory(Memory);
    }
    static void _UiEditRegester()
    {
        RegisterEdit Regester = ReadRegesterEdit();
        VoleMechine::WriteRegester(Regester);
    }
    static void _UiShow_Status()
    {
        VoleMechine::SnapShot();
    }
    static void Run()
    {
       
        int PCStartingAddress = clsInputValidate::_ReadHexAddress("Enter the starting address (2-digit hexadecimal): ");
        CPU::Run(PCStartingAddress);
    }
    static void _PerfromMainMenueOption(eUserInput MainMenueOption)
    {
        switch (MainMenueOption)
        {
        case eUserInput::eLoad_Program:
        {
            system("cls");
            _UiLoadProgaram();
            _GoBackToMainMenue();
            break;
        }
        case eUserInput::eEdit_Register:
            system("cls");
            _UiEditRegester();
            _GoBackToMainMenue();
            break;

        case eUserInput::eEdit_Memory:
            system("cls");
            _UiEditMemory();
            _GoBackToMainMenue();
            break;

        case eUserInput::View_Stat:
            system("cls");
            _UiShow_Status();
            _GoBackToMainMenue();
            break;

        case eUserInput::Run_Program:
        {
            system("cls");
            Run();
            _GoBackToMainMenue();
            break;
        }
        case eUserInput::EXIT:
            system("cls");

            break;
        }
    }
public:
    static void ShowMainScreen()
    {
        system("cls");
        cout << setw(37) << left << "" << "===========================================\n";
        cout << setw(37) << left << "" << "\t\t     Main Menu\n";
        cout << setw(37) << left << "" << "===========================================\n";
        cout << setw(37) << left << "" << "\t [1] Load Program.\n";
        cout << setw(37) << left << "" << "\t [2] Edit Register.\n";
        cout << setw(37) << left << "" << "\t [3] Edit Memory.\n";
        cout << setw(37) << left << "" << "\t [4] View Stat.\n";
        cout << setw(37) << left << "" << "\t [5] Run Program.\n";
        cout << setw(37) << left << "" << "\t [6] Exit.\n";
        cout << setw(37) << left << "" << "===========================================\n";
        _PerfromMainMenueOption((eUserInput)_ReadMainMenueOption());
    }
};

