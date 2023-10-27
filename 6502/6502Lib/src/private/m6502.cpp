#include "m6502.h"

#define ASSERT( Condition, Text ) { if ( !Condition ) { throw -1; } }

m6502::s32 m6502::CPU::Execute( s32 Cycles, Mem & memory )
{
    /** Load a Register with the value from the memory address */
    auto LoadRegister = 
        [&Cycles,&memory,this]
        ( Word Address, Byte& Register )
    {
        Register = ReadByte ( Cycles, Address, memory);
        SetZeroAndNegativeFlags( Register );
    };

	const s32 CyclesRequested = Cycles;
    while ( Cycles > 0)
    {
        Byte Ins = FetchByte( Cycles, memory);
        switch (Ins)
        {
            case INS_LDA_IM:
            {
                A = FetchByte (Cycles, memory);
                SetZeroAndNegativeFlags(A);
            } break;
            case INS_LDA_ZP:
            {
                Word Address = AddrZeroPage( Cycles, memory );
                LoadRegister ( Address, A);
            } break;
            case INS_LDA_ZPX:
            {
                Word Address = AddrZeroPageX( Cycles, memory );
                LoadRegister ( Address, A);
            } break;
            case INS_JSR:
            {
                Word SubAddr = FetchWord( Cycles, memory);
                PushPCMinusOneToStack( Cycles, memory);
                PC = SubAddr;
                Cycles--;
            } break;
            default:
            {
                printf("Instruction %d not handled\n", Ins);
                throw - 1;
            } break;
        }
    }
    const s32 NumCyclesUsed = CyclesRequested - Cycles;
	return NumCyclesUsed;
}

m6502::Word m6502::CPU::AddrZeroPage( s32& Cycles, const Mem& memory )
{
	Byte ZeroPageAddr = FetchByte( Cycles, memory );
	return ZeroPageAddr;
}

m6502::Word m6502::CPU::AddrZeroPageX( s32& Cycles, const Mem& memory )
{
	Byte ZeroPageAddr = FetchByte( Cycles, memory );
	ZeroPageAddr += X;
	Cycles--;
	return ZeroPageAddr;
}