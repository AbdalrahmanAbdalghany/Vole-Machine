#pragma once
#include<string>

struct RegisterEdit
{
    int Index;
    int Value;
};
struct MemoryEdit
{
    string Address;
    string Value;
};
enum ExecutionMood {
    Run_At_Ounce = 1,
    Run_Step_By_Step = 2
};
enum Opcode
{
    Load_Memory = 1,
    Load_Immdiate = 2,
    Store = 3,
    Move = 4,
    Add_intger = 5,
    Add_Float = 6,
    OR = 7,
    And = 8,
    XOR = 9,
    Rotate_Right = 10,
    Jump_Equal_R0 = 11,
    Halt = 12
};

struct ValidationResult
{
	bool IsValid;
	string ErrorMessage;
};


