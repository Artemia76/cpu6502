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

    Word ReadWord(
		s32& Cycles,
		Word Address,
		const Mem& memory )
	{
		Byte LoByte = ReadByte( Cycles, Address, memory );
		Byte HiByte = ReadByte( Cycles, Address + 1, memory );
		return LoByte | (HiByte << 8);
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

	/** Push the PC+1 onto the stack */
	void PushPCPlusOneToStack( s32& Cycles, Mem& memory )
	{
		PushWordToStack( Cycles, memory, PC + 1 );
	}

	/** Push the PC onto the stack */
	void PushPCToStack( s32& Cycles, Mem& memory )
	{
		PushWordToStack( Cycles, memory, PC );
	}

	void PushByteOntoStack( s32& Cycles, Byte Value, Mem& memory )
	{
		const Word SPWord = SPToAddress();
		memory[SPWord] = Value;
		Cycles--;
		SP--;
		Cycles--;
	}

	Byte PopByteFromStack( s32& Cycles, Mem& memory )
	{
		SP++;
		Cycles--;
		const Word SPWord = SPToAddress();
		Byte Value = memory[SPWord];
		Cycles--;
		return Value;
	}

	/** Pop a 16-bit value from the stack */
	Word PopWordFromStack( s32& Cycles, Mem& memory )
	{
		Word ValueFromStack = ReadWord( Cycles, SPToAddress()+1, memory );
		SP += 2;
		Cycles--;
		return ValueFromStack;
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
		INS_LDA_ABS = 0xAD,
		INS_LDA_ABSX = 0xBD,
		INS_LDA_ABSY = 0xB9,
		INS_LDA_INDX = 0xA1,
		INS_LDA_INDY = 0xB1,
		//LDX
		INS_LDX_IM = 0xA2,
		INS_LDX_ZP = 0xA6,
		INS_LDX_ZPY = 0xB6,
		INS_LDX_ABS = 0xAE,
		INS_LDX_ABSY = 0xBE,
		//LDY
		INS_LDY_IM = 0xA0,
		INS_LDY_ZP = 0xA4,
		INS_LDY_ZPX = 0xB4,
		INS_LDY_ABS = 0xAC,
		INS_LDY_ABSX = 0xBC,
        //STA
		INS_STA_ZP = 0x85,
		INS_STA_ZPX = 0x95,
		INS_STA_ABS = 0x8D,
		INS_STA_ABSX = 0x9D,
		INS_STA_ABSY = 0x99,
		INS_STA_INDX = 0x81,
		INS_STA_INDY = 0x91,
		//STX
		INS_STX_ZP = 0x86,
		INS_STX_ZPY = 0x96,
		INS_STX_ABS = 0x8E,
		//STY
		INS_STY_ZP = 0x84,
		INS_STY_ZPX = 0x94,
		INS_STY_ABS = 0x8C,

        //Jumps and calls
		INS_JMP_ABS = 0x4C,
		INS_JMP_IND = 0x6C,
		INS_JSR = 0x20,
		INS_RTS = 0x60,

        //Increments, Decrements
		INS_INX = 0xE8,
		INS_INY = 0xC8,
		INS_DEY = 0x88,
		INS_DEX = 0xCA,
		INS_DEC_ZP = 0xC6,
		INS_DEC_ZPX = 0xD6,
		INS_DEC_ABS = 0xCE,
		INS_DEC_ABSX = 0xDE,
		INS_INC_ZP = 0xE6,
		INS_INC_ZPX = 0xF6,
		INS_INC_ABS = 0xEE,
		INS_INC_ABSX = 0xFE;

   /** Sets the correct Process status after a load register instruction
	*	- LDA, LDX, LDY
	*	@Register The A,X or Y Register */
	void SetZeroAndNegativeFlags( Byte Register )
	{
		Flag.Z = (Register == 0);
		Flag.N = (Register & NegativeFlagBit) > 0;
	}
    /** @return the address that the program was loading into, or 0 if no program */
	Word LoadPrg( const Byte* Program, u32 NumBytes, Mem& memory ) const;

    s32 Execute( s32 Cycles, Mem& memory);

    /** Addressing mode - Zero page */
	Word AddrZeroPage( s32& Cycles, const Mem& memory );

	/** Addressing mode - Zero page with X offset */
	Word AddrZeroPageX( s32& Cycles, const Mem& memory );

    /** Addressing mode - Zero page with Y offset */
    Word AddrZeroPageY( s32& Cycles, const Mem& Memory );

    /** Addressing mode - Absolute */
    Word AddrAbsolute( s32& Cycles, const Mem& Memory );

    /** Addressing mode - Absolute with X offset */
    Word AddrAbsoluteX( s32& Cycles, const Mem& Memory );

    /** Addressing mode - Absolute with X offset
     *      - Always takes a cycle for the X page boundary)
     *      - See "STA Absolute,X" */
    Word AddrAbsoluteX_5( s32& Cycles, const Mem& memory );

    /** Addressing mode - Absolute with Y offset */
    Word AddrAbsoluteY( s32& Cycles, const Mem& Memory );

	/** Addressing mode - Absolute with Y offset
	*	- Always takes a cycle for the Y page boundary)
	*	- See "STA Absolute,Y" */
	Word AddrAbsoluteY_5( s32& Cycles, const Mem& memory );

    /** Addressing mode - Indirect X | Indexed Indirect */
	Word AddrIndirectX( s32& Cycles, const Mem& memory );

	/** Addressing mode - Indirect Y | Indirect Indexed */
	Word AddrIndirectY( s32& Cycles, const Mem& memory );

	/** Addressing mode - Indirect X | Indirect Indexed
	*	- Always takes a cycle for the Y page boundary)
	*	- See "STA (Indirect,Y) */
	Word AddrIndirectX_6( s32& Cycles, const Mem& memory );

    /** Addressing mode - Indirect Y | Indirect Indexed
	*	- Always takes a cycle for the Y page boundary)
	*	- See "STA (Indirect,Y) */
	Word AddrIndirectY_6( s32& Cycles, const Mem& memory );
};