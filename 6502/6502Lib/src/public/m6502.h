#pragma once
#include <stdio.h>
#include <stdlib.h>

namespace m6502
{
    using SByte = char;
    using Byte = unsigned char;
    using Word = unsigned short;

    using u32 = unsigned int;
	using s32 = signed int;

    struct Mem;
    struct CPU;
    struct StatusFlags;
}

struct m6502::Mem
{
    static constexpr u32 MAX_MEM = 1024*64;
    Byte Data[MAX_MEM];

     void Initialise ()
    {
        for ( u32 i = 0; i < MAX_MEM; i++)
        {
            Data[i] = 0;
        }
    }

    /** read 1 byte */
    Byte operator[]( u32 Address) const
    {
        // assert here Address is < MAX_MEM
        return Data[Address];
    }

    /** write 1 byte */
    Byte& operator[]( u32 Address)
    {
        // assert here Address is < MAX_MEM
        return Data[Address];
    }
};

struct m6502::StatusFlags
{	
	Byte C : 1;	//0: Carry Flag	
	Byte Z : 1;	//1: Zero Flag
	Byte I : 1; //2: Interrupt disable
	Byte D : 1; //3: Decimal mode
	Byte B : 1; //4: Break
	Byte Unused : 1; //5: Unused
	Byte V : 1; //6: Overflow
	Byte N : 1; //7: Negative
};

struct m6502::CPU
{
    Word PC;        //program counter
    Byte SP;        //stack pointer

    Byte A, X, Y;   //registers

    union // Processor status
    {
        Byte PS;
        StatusFlags Flag;
    };

    void Reset( Mem& memory)
    {
		Reset( 0xFFFC, memory );
    }

    void Reset( Word ResetVector, Mem& memory)
    {
        PC = ResetVector;
        SP = 0xFF;
		Flag.C = Flag.Z = Flag.I = Flag.D = Flag.B = Flag.V = Flag.N = 0;
		A = X = Y = 0;
		memory.Initialise();
    }

    Byte FetchByte( s32& Cycles, const Mem& memory)
    {
        Byte Data = memory[PC];
        PC++;
        Cycles--;
        return Data;
    }

    Word FetchWord( s32& Cycles, const Mem& memory)
    {
        // 6502 is little endian
        Word Data = memory[PC];
        PC++;
    
        Data |= (memory[PC] << 8 );
        PC++;
    
        Cycles-=2;
        return Data;
    }

    Byte ReadByte(
        s32& Cycles,
        Word Address,
        const Mem& memory)
    {
        Byte Data = memory[Address];
        Cycles--;
        return Data;
    }

    /** write 1 byte to memory */
	void WriteByte( Byte Value, s32& Cycles, Word Address, Mem& memory )
	{
		memory[Address] = Value;
		Cycles--;
	}

	/** write 2 bytes to memory */
	void WriteWord(	Word Value, s32& Cycles, Word Address, Mem& memory )
	{
		memory[Address] = Value & 0xFF;
		memory[Address + 1] = (Value >> 8);
		Cycles -= 2;
	}

/** @return the stack pointer as a full 16-bit address (in the 1st page) */
	Word SPToAddress() const
	{
		return 0x100 | SP;
	}

	void PushWordToStack( s32& Cycles, Mem& memory, Word Value )
	{
		WriteByte( Value >> 8, Cycles, SPToAddress(), memory );
		SP--;
		WriteByte( Value & 0xFF, Cycles, SPToAddress(), memory );
		SP--;
	}

    /** Push the PC-1 onto the stack */
	void PushPCMinusOneToStack( s32& Cycles, Mem& memory )
	{
		PushWordToStack( Cycles, memory, PC - 1 );
	}

    // Process status bits
	static constexpr Byte
		NegativeFlagBit = 0b10000000,
		OverflowFlagBit = 0b01000000,
		BreakFlagBit = 0b000010000,
		UnusedFlagBit = 0b000100000,
		InterruptDisableFlagBit = 0b000000100,
		ZeroBit = 0b00000001;

    // opcodes
    static constexpr Byte
        //LDA
        INS_LDA_IM = 0xA9,
        INS_LDA_ZP = 0xA5,
        INS_LDA_ZPX = 0xB5,
    
        INS_JSR = 0x20;

   /** Sets the correct Process status after a load register instruction
	*	- LDA, LDX, LDY
	*	@Register The A,X or Y Register */
	void SetZeroAndNegativeFlags( Byte Register )
	{
		Flag.Z = (Register == 0);
		Flag.N = (Register & NegativeFlagBit) > 0;
	}

    s32 Execute( s32 Cycles, Mem& memory);

    /** Addressing mode - Zero page */
	Word AddrZeroPage( s32& Cycles, const Mem& memory );

	/** Addressing mode - Zero page with X offset */
	Word AddrZeroPageX( s32& Cycles, const Mem& memory );
};