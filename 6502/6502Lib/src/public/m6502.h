#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <iostream>

namespace m6502
{
    using SByte = std::int8_t;
    using Byte = std::uint8_t;
    using Word = std::uint16_t;

    using u32 = std::uint32_t;
	using s32 = std::int32_t;

    using u64 = std::uint64_t;
	using s64 = std::int64_t;

    class Mem;
    class CPU;
    struct StatusFlags;
	class Registers;
	enum class Ins : std::uint8_t;
}

class m6502::Mem
{
	public:
static constexpr u32 MAX_MEM = 1024*64;
    Byte Data[MAX_MEM];

	Mem();
	~Mem();

    void Initialise ()
    {
        for ( u32 i = 0; i < MAX_MEM; i++)
        {
            Data[i] = 0;
        }
    }

    /** read 1 byte */
    Byte operator[]( u32 Address) const;

    /** write 1 byte */
    Byte& operator[]( u32 Address);
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

class m6502::Registers
{
	public:
		Word 		PC;        //program counter
    	Byte 		SP;        //stack pointer
		Byte 		A, X, Y;   //registers
  		Byte 		PS;
    	StatusFlags	Flags;

		    // Process status bits
	static constexpr Byte
		NegativeFlagBit = 0b10000000,
		OverflowFlagBit = 0b01000000,
		BreakFlagBit = 0b000010000,
		UnusedFlagBit = 0b000100000,
		InterruptDisableFlagBit = 0b000000100,
		ZeroBit = 0b00000001;
};

 // opcodes
enum class m6502::Ins : m6502::Byte
{	
	//LDA
	LDA_IM = 0xA9,
	LDA_ZP = 0xA5,
	LDA_ZPX = 0xB5,
	LDA_ABS = 0xAD,
	LDA_ABSX = 0xBD,
	LDA_ABSY = 0xB9,
	LDA_INDX = 0xA1,
	LDA_INDY = 0xB1,
	//LDX
	LDX_IM = 0xA2,
	LDX_ZP = 0xA6,
	LDX_ZPY = 0xB6,
	LDX_ABS = 0xAE,
	LDX_ABSY = 0xBE,
	//LDY
	LDY_IM = 0xA0,
	LDY_ZP = 0xA4,
	LDY_ZPX = 0xB4,
	LDY_ABS = 0xAC,
	LDY_ABSX = 0xBC,
    //STA
	STA_ZP = 0x85,
	STA_ZPX = 0x95,
	STA_ABS = 0x8D,
	STA_ABSX = 0x9D,
	STA_ABSY = 0x99,
	STA_INDX = 0x81,
	STA_INDY = 0x91,
	//STX
	STX_ZP = 0x86,
	STX_ZPY = 0x96,
	STX_ABS = 0x8E,
	//STY
	STY_ZP = 0x84,
	STY_ZPX = 0x94,
	STY_ABS = 0x8C,

    //Jumps and calls
	JMP_ABS = 0x4C,
	JMP_IND = 0x6C,
	JSR = 0x20,
	RTS = 0x60,

    //Increments, Decrements
	INX = 0xE8,
	INY = 0xC8,
	DEY = 0x88,
	DEX = 0xCA,
	DEC_ZP = 0xC6,
	DEC_ZPX = 0xD6,
	DEC_ABS = 0xCE,
	DEC_ABSX = 0xDE,
	INC_ZP = 0xE6,
	INC_ZPX = 0xF6,
	INC_ABS = 0xEE,
	INC_ABSX = 0xFE
};

/// @brief 
class m6502::CPU : public Registers
{
	public:
				CPU(Mem& memory);
				~CPU();
    	void	Reset( );
    	void	Reset( Word ResetVector );
		/** @return the address that the program was loading into, or 0 if no program */
		Word 	LoadPrg( const Byte* Program, u32 NumBytes) const;
    	s64 	Execute( s64 Cycles);

	private:
		Mem& m_memory;

		std::int64_t m_cycles;

    	Byte FetchByte();
    	Word FetchWord();
    	Byte ReadByte( Word Address );
    	Word ReadWord( Word Address );

		/** write 1 byte to memory */
		void WriteByte( Byte Value, Word Address );

		/** write 2 bytes to memory */
		void WriteWord(	Word Value, Word Address );

	/** @return the stack pointer as a full 16-bit address (in the 1st page) */
		Word SPToAddress() const;

		void PushWordToStack( Word Value );

		/** Push the PC-1 onto the stack */
		void PushPCMinusOneToStack();

		/** Push the PC+1 onto the stack */
		void PushPCPlusOneToStack();

		/** Push the PC onto the stack */
		void PushPCToStack();

		void PushByteOntoStack( Byte Value );

		Byte PopByteFromStack();

		/** Pop a 16-bit value from the stack */
		Word PopWordFromStack();

	/** Sets the correct Process status after a load register instruction
		*	- LDA, LDX, LDY
		*	@Register The A,X or Y Register */
		void SetZeroAndNegativeFlags( Byte Register );

		/** Addressing mode - Zero page */
		Word AddrZeroPage();

		/** Addressing mode - Zero page with X offset */
		Word AddrZeroPageX();

		/** Addressing mode - Zero page with Y offset */
		Word AddrZeroPageY();

		/** Addressing mode - Absolute */
		Word AddrAbsolute();

		/** Addressing mode - Absolute with X offset */
		Word AddrAbsoluteX();

		/** Addressing mode - Absolute with X offset
		 *      - Always takes a cycle for the X page boundary)
		 *      - See "STA Absolute,X" */
		Word AddrAbsoluteX_5();

		/** Addressing mode - Absolute with Y offset */
		Word AddrAbsoluteY();

		/** Addressing mode - Absolute with Y offset
		*	- Always takes a cycle for the Y page boundary)
		*	- See "STA Absolute,Y" */
		Word AddrAbsoluteY_5();

		/** Addressing mode - Indirect X | Indexed Indirect */
		Word AddrIndirectX();

		/** Addressing mode - Indirect Y | Indirect Indexed */
		Word AddrIndirectY();

		/** Addressing mode - Indirect X | Indirect Indexed
		*	- Always takes a cycle for the Y page boundary)
		*	- See "STA (Indirect,Y) */
		Word AddrIndirectX_6();

		/** Addressing mode - Indirect Y | Indirect Indexed
		*	- Always takes a cycle for the Y page boundary)
		*	- See "STA (Indirect,Y) */
		Word AddrIndirectY_6();
};