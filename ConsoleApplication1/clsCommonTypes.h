#pragma once
#include<string>

struct RegisterEdit
{
    string Index;
    string Value;
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
    Load_Memory,
    Load_Immdiate,
    Store,
    Move,
    Add_intger,
    Add_Float,
    OR,
    And,
    XOR,
    Rotate_Right,
    Jump_Equal_R0,
    Halt
};
struct Enstructions
{
    //Write the Variables you want stor the decoder result
};

