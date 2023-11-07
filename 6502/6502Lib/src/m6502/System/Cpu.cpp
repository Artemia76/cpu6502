/**
 * @file Cpu.cpp
 * @author Gianni Peschiutta
 * @brief M6502Lib - Motorola 6502 CPU Emulator
 * @version 0.1
 * @date 2023-11-05
 * 
 * @copyright Copyright (c) 2023
 * Based on davepoo work: https://github.com/davepoo/6502Emulator
 *
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *    This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 *    You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>
 */

#include <m6502/System/Cpu.hpp>

#define ASSERT( Condition, Text ) { if ( !Condition ) { throw -1; } }

namespace m6502
{

/**
 * @brief Construct a new m6502::C P U::C P U object
 * 
 * @param pMem 
 */
CPU::CPU(Mem& pMem) : m_memory(pMem)
{
    Reset();
}

CPU::~CPU()
{

}

void CPU::Reset( )
{
    Reset( 0xFFFC );
}

void CPU::Reset( Word ResetVector )
{
    PC = ResetVector;
    SP = 0xFF;
    Flags.C = Flags.Z = Flags.I = Flags.D = Flags.B = Flags.V = Flags.N = 0;
    A = X = Y = 0;
    m_memory.Initialise();
}

Byte CPU::FetchByte()
{
    Byte Data = m_memory[PC];
    PC++;
    m_cycles--;
    return Data;
}

SByte CPU::FetchSByte()
{
    return FetchByte();
}

/**
 * @brief Fetch 1 Word from Memory
 * 
 * @return Word 
 */
Word CPU::FetchWord()
{
    // 6502 is little endian
    Word Data = m_memory[PC];
    PC++;

    Data |= (m_memory[PC] << 8 );
    PC++;

    m_cycles-=2;
    return Data;
}

Byte CPU::ReadByte( Word Address )
{
    Byte Data = m_memory[Address];
    m_cycles--;
    return Data;
}

Word CPU::ReadWord( Word Address )
{
    Byte LoByte = ReadByte( Address );
    Byte HiByte = ReadByte( Address + 1 );
    return LoByte | (HiByte << 8);
}

/**
 * @brief Write 1 Byte in Memory
 * 
 * @param Value 
 * @param Address 
 */
void CPU::WriteByte( Byte Value, Word Address )
{
    m_memory[Address] = Value;
    m_cycles--;
}

/**
 * @brief Write a word in memory
 * 
 * @param Value 
 * @param Address 
 */
void CPU::WriteWord( Word Value, Word Address )
{
    m_memory[Address] = Value & 0xFF;
    m_memory[Address + 1] = (Value >> 8);
    m_cycles -= 2;
}

/** @return the stack pointer as a full 16-bit address (in the 1st page) */
Word CPU::SPToAddress() const
{
    return 0x100 | SP;
}

void CPU::PushWordToStack( Word Value )
{
    WriteByte( Value >> 8, SPToAddress());
    SP--;
    WriteByte( Value & 0xFF, SPToAddress());
    SP--;
}

/** Push the PC-1 onto the stack */
void CPU::PushPCMinusOneToStack()
{
    PushWordToStack( PC - 1 );
}

/** Push the PC+1 onto the stack */
void CPU::PushPCPlusOneToStack()
{
    PushWordToStack( PC + 1 );
}

/** Push the PC onto the stack */
void CPU::PushPCToStack()
{
    PushWordToStack( PC );
}

void CPU::PushByteOntoStack( Byte Value )
{
    const Word SPWord = SPToAddress();
    m_memory[SPWord] = Value;
    m_cycles--;
    SP--;
    m_cycles--;
}

Byte CPU::PopByteFromStack()
{
    SP++;
    m_cycles--;
    const Word SPWord = SPToAddress();
    Byte Value = m_memory[SPWord];
    m_cycles--;
    return Value;
}

/** Pop a 16-bit value from the stack */
Word CPU::PopWordFromStack()
{
    Word ValueFromStack = ReadWord( SPToAddress()+1 );
    SP += 2;
    m_cycles--;
    return ValueFromStack;
}

/** Sets the correct Process status after a load register instruction
    *	- LDA, LDX, LDY
    *	@Register The A,X or Y Register */
void CPU::SetZeroAndNegativeFlags( Byte Register )
{
    Flags.Z = (Register == 0);
    Flags.N = (Register & NegativeFlagBit) > 0;
}

s64 CPU::Execute( s64 Cycles )
{
    /** Load a Register with the value from the memory address */
    auto LoadRegister = [ this ]
        ( Word Address, Byte& Register )
    {
        Register = ReadByte ( Address );
        SetZeroAndNegativeFlags( Register );
    };

    /** And the A Register with the value from the memory address */
    auto And = [ this ] ( Word Address )
    {
        A &= ReadByte( Address );
        SetZeroAndNegativeFlags( A );
    };

    /** Or the A Register with the value from the memory address */
    auto Ora = [ this ] ( Word Address )
    {
        A |= ReadByte( Address );
        SetZeroAndNegativeFlags( A );
    };

    /** Eor the A Register with the value from the memory address */
    auto Eor = [ this ]	( Word Address )
    {
        A ^= ReadByte( Address );
        SetZeroAndNegativeFlags( A );
    };

    /* Conditional branch */
    auto BranchIf = [ this ] ( bool Test, bool Expected )
    {
        SByte Offset = FetchSByte();
        if ( Test == Expected )
        {
            const Word PCOld = PC;
            PC += Offset;
            m_cycles--;

            const bool PageChanged = (PC >> 8) != (PCOld >> 8);
            if ( PageChanged )
            {
                m_cycles--;
            }
        }
    };

    /** Push Processor status onto the stack
    *	Setting bits 4 & 5 on the stack */
    auto PushPSToStack = [ this ] ()
    {
        Byte PSStack = PS | BreakFlagBit | UnusedFlagBit;		
        PushByteOntoStack( PSStack );
    };

    /** Pop Processor status from the stack
    *	Clearing bits 4 & 5 (Break & Unused) */
    auto PopPSFromStack = [ this ] ()
    {
        bool B = Flags.B;
        bool Unused = Flags.Unused;
        PS = PopByteFromStack();
        Flags.B = B;
        Flags.Unused = Unused;
    };
    s64 CyclesRequested = Cycles;
    m_cycles = Cycles;
    while ( m_cycles > 0)
    {
        Byte Instr = FetchByte();
        switch (ins(Instr))
        {
            case Ins::AND_IM:
            {
                A &= FetchByte();
                SetZeroAndNegativeFlags(A);
            } break;
            case Ins::ORA_IM:
            {
                A |= FetchByte();
                SetZeroAndNegativeFlags(A);
            } break;
            case Ins::EOR_IM:
            {
                A ^= FetchByte();
                SetZeroAndNegativeFlags(A);
            } break;
            case Ins::AND_ZP:
            {
                Word Address = AddrZeroPage();
                And( Address );
            } break;
            case Ins::ORA_ZP:
            {
                Word Address = AddrZeroPage();
                Ora( Address );
            } break;
            case Ins::EOR_ZP:
            {
                Word Address = AddrZeroPage();
                Eor( Address );
            } break;
            case Ins::AND_ZPX:
            {
                Word Address = AddrZeroPageX();
                And( Address );
            } break;
            case Ins::ORA_ZPX:
            {
                Word Address = AddrZeroPageX();
                Ora( Address );
            } break;
            case Ins::EOR_ZPX:
            {
                Word Address = AddrZeroPageX();
                Eor( Address );
            } break;
            case Ins::AND_ABS:
            {
                Word Address = AddrAbsolute();
                And( Address );
            } break;
            case Ins::ORA_ABS:
            {
                Word Address = AddrAbsolute();
                Ora( Address );
            } break;
            case Ins::EOR_ABS:
            {
                Word Address = AddrAbsolute();
                Eor( Address );
            } break;
            case Ins::AND_ABSX:
            {
                Word Address = AddrAbsoluteX();
                And( Address );
            } break;
            case Ins::ORA_ABSX:
            {
                Word Address = AddrAbsoluteX();
                Ora( Address );
            } break;
            case Ins::EOR_ABSX:
            {
                Word Address = AddrAbsoluteX();
                Eor( Address );
            } break;
            case Ins::AND_ABSY:
            {
                Word Address = AddrAbsoluteY();
                And( Address );
            } break;
            case Ins::ORA_ABSY:
            {
                Word Address = AddrAbsoluteY();
                Ora( Address );
            } break;
            case Ins::EOR_ABSY:
            {
                Word Address = AddrAbsoluteY();
                Eor( Address );
            } break;
            case Ins::AND_INDX:
            {
                Word Address = AddrIndirectX();
                And( Address );
            } break;
            case Ins::ORA_INDX:
            {
                Word Address = AddrIndirectX();
                Ora( Address );
            } break;
            case Ins::EOR_INDX:
            {
                Word Address = AddrIndirectX();
                Eor( Address );
            } break;
            case Ins::AND_INDY:
            {
                Word Address = AddrIndirectY();
                And( Address );
            } break;
            case Ins::ORA_INDY:
            {
                Word Address = AddrIndirectY();
                Ora( Address );
            } break;
            case Ins::EOR_INDY:
            {
                Word Address = AddrIndirectY();
                Eor( Address );
            } break;
            case Ins::BIT_ZP:
            {
                Word Address = AddrZeroPage();
                Byte Value = ReadByte(Address);
                Flags.Z = ! (A & Value);
                Flags.N = (Value & NegativeFlagBit) != 0;
                Flags.V = (Value & OverflowFlagBit) != 0;
            } break;
            case Ins::BIT_ABS:
            {
                Word Address = AddrAbsolute();
                Byte Value = ReadByte(Address);
                Flags.Z = ! (A & Value);
                Flags.N = (Value & NegativeFlagBit) != 0;
                Flags.V = (Value & OverflowFlagBit) != 0;
            } break;
            case Ins::LDA_IM:
            {
                A = FetchByte ();
                SetZeroAndNegativeFlags(A);
            } break;
            case Ins::LDX_IM:
            {
                X = FetchByte ();
                SetZeroAndNegativeFlags(X);
            } break;
            case Ins::LDY_IM:
            {
                Y = FetchByte ();
                SetZeroAndNegativeFlags(Y);
            } break;
            case Ins::LDA_ZP:
            {
                Word Address = AddrZeroPage();
                LoadRegister ( Address, A );
            } break;
            case Ins::LDX_ZP:
            {
                Word Address = AddrZeroPage();
                LoadRegister ( Address, X );
            } break;
            case Ins::LDY_ZP:
            {
                Word Address = AddrZeroPage();
                LoadRegister ( Address, Y );
            } break;
            case Ins::LDA_ZPX:
            {
                Word Address = AddrZeroPageX();
                LoadRegister ( Address, A );
            } break;
            case Ins::LDX_ZPY:
            {
                Word Address = AddrZeroPageY();
                LoadRegister ( Address, X );
            } break;
            case Ins::LDY_ZPX:
            {
                Word Address = AddrZeroPageX();
                LoadRegister ( Address, Y );
            } break;
            case Ins::LDA_ABS:
            {
                Word Address = AddrAbsolute();
                LoadRegister ( Address, A );
            } break;
            case Ins::LDX_ABS:
            {
                Word Address = AddrAbsolute();
                LoadRegister ( Address, X );
            } break;
            case Ins::LDY_ABS:
            {
                Word Address = AddrAbsolute();
                LoadRegister ( Address, Y );
            } break;
            case Ins::LDA_ABSX:
            {
                Word Address = AddrAbsoluteX();
                LoadRegister ( Address, A );
            } break;
            case Ins::LDA_ABSY:
            {
                Word Address = AddrAbsoluteY();
                LoadRegister ( Address, A );
            } break;
            case Ins::LDX_ABSY:
            {
                Word Address = AddrAbsoluteY();
                LoadRegister ( Address, X );
            } break;
            case Ins::LDY_ABSX:
            {
                Word Address = AddrAbsoluteX();
                LoadRegister ( Address, Y );
            } break;
            case Ins::LDA_INDX:
            {
                Word Address = AddrIndirectX();
                LoadRegister ( Address, A );
            } break;
            case Ins::LDA_INDY:
            {
                Word Address = AddrIndirectY();
                LoadRegister ( Address, A );
            } break;
            case Ins::STA_ZP:
            {
                Word Address = AddrZeroPage();
                WriteByte ( A , Address );
            } break;
            case Ins::STX_ZP:
            {
                Word Address = AddrZeroPage();
                WriteByte ( X , Address );
            } break;
            case Ins::STY_ZP:
            {
                Word Address = AddrZeroPage();
                WriteByte ( Y , Address );
            } break;
            case Ins::STA_ABS:
            {
                Word Address = AddrAbsolute();
                WriteByte ( A , Address );
            } break;
            case Ins::STX_ABS:
            {
                Word Address = AddrAbsolute();
                WriteByte ( X , Address );
            } break;
            case Ins::STY_ABS:
            {
                Word Address = AddrAbsolute();
                WriteByte ( Y , Address );
            } break;
            case Ins::STA_ZPX:
            {
                Word Address = AddrZeroPageX();
                WriteByte ( A , Address );
            } break;
            case Ins::STY_ZPX:
            {
                Word Address = AddrZeroPageX();
                WriteByte ( Y , Address );
            } break;
            case Ins::STA_ABSX:
            {
                Word Address = AddrAbsoluteX_5();
                WriteByte ( A , Address );
            } break;
            case Ins::STA_ABSY:
            {
                Word Address = AddrAbsoluteY_5();
                WriteByte ( A , Address );
            } break;
            case Ins::STX_ZPY:
            {
                Word Address = AddrZeroPageY();
                WriteByte ( X , Address );
            } break;
            case Ins::STA_INDX:
            {
                Word Address = AddrIndirectX_6();
                WriteByte ( A , Address );
            } break;
            case Ins::STA_INDY:
            {
                Word Address = AddrIndirectY_6();
                WriteByte ( A , Address );
            } break;
            case Ins::JSR:
            {
                Word SubAddr = FetchWord();
                PushPCMinusOneToStack();
                PC = SubAddr;
                m_cycles--;
            } break;
            case Ins::RTS:
            {
                Word ReturnAddress = PopWordFromStack();
                PC = ReturnAddress + 1;	
                m_cycles -= 2;
            } break;
            //TODO:
            //An original 6502 has does not correctly fetch the target 
            //address if the indirect vector falls on a page boundary
            //( e.g.$xxFF where xx is any value from $00 to $FF ).
            //In this case fetches the LSB from $xxFF as expected but 
            //takes the MSB from $xx00.This is fixed in some later chips 
            //like the 65SC02 so for compatibility always ensure the 
            //indirect vector is not at the end of the page.
            case Ins::JMP_ABS:
            {
                Word Address = AddrAbsolute();
                PC = Address;
            } break;
            case Ins::JMP_IND:
            {
                Word Address = AddrAbsolute();
                Address = ReadWord( Address );
                PC = Address;
            } break;
            case Ins::TSX:
            {
                X = SP;
                m_cycles--;
                SetZeroAndNegativeFlags( X );
            } break;
            case Ins::TXS:
            {
                SP = X;
                m_cycles--;
            } break;
            case Ins::PHA:
            {
                PushByteOntoStack( A );
            } break;
            case Ins::PLA:
            {
                A = PopByteFromStack();
                SetZeroAndNegativeFlags( A );
                m_cycles--;
            } break;
            case Ins::PHP:
            {
                PushPSToStack();
            } break;
            case Ins::PLP:
            {
                PopPSFromStack();
                m_cycles--;
            } break;
            case Ins::TAX:
            {
                X = A;
                m_cycles--;
                SetZeroAndNegativeFlags( X );
            } break;
            case Ins::TAY:
            {
                Y = A;
                m_cycles--;
                SetZeroAndNegativeFlags( Y );
            } break;
            case Ins::TXA:
            {
                A = X;
                m_cycles--;
                SetZeroAndNegativeFlags( A );
            } break;
            case Ins::TYA:
            {
                A = Y;
                m_cycles--;
                SetZeroAndNegativeFlags( A );
            } break;
            case Ins::INX:
            {
                X++;
                m_cycles--;
                SetZeroAndNegativeFlags( X );
            } break;
            case Ins::INY:
            {
                Y++;
                m_cycles--;
                SetZeroAndNegativeFlags( Y );
            } break;
            case Ins::DEX:
            {
                X--;
                m_cycles--;
                SetZeroAndNegativeFlags( X );
            } break;
            case Ins::DEY:
            {
                Y--;
                m_cycles--;
                SetZeroAndNegativeFlags( Y );
            } break;
            case Ins::DEC_ZP:
            {
                Word Address = AddrZeroPage();
                Byte Value = ReadByte( Address );
                Value--;
                m_cycles--;
                WriteByte( Value, Address );
                SetZeroAndNegativeFlags( Value );
            } break;
            case Ins::DEC_ZPX:
            {
                Word Address = AddrZeroPageX();
                Byte Value = ReadByte( Address );
                Value--;
                m_cycles--;
                WriteByte( Value, Address );
                SetZeroAndNegativeFlags( Value );
            } break;
            case Ins::DEC_ABS:
            {
                Word Address = AddrAbsolute();
                Byte Value = ReadByte( Address );
                Value--;
                m_cycles--;
                WriteByte( Value, Address );
                SetZeroAndNegativeFlags( Value );
            } break;
            case Ins::DEC_ABSX:
            {
                Word Address = AddrAbsoluteX_5();
                Byte Value = ReadByte( Address );
                Value--;
                m_cycles--;
                WriteByte( Value, Address );
                SetZeroAndNegativeFlags( Value );
            } break;
            case Ins::INC_ZP:
            {
                Word Address = AddrZeroPage();
                Byte Value = ReadByte( Address );
                Value++;
                m_cycles--;
                WriteByte( Value, Address );
                SetZeroAndNegativeFlags( Value );
            } break;
            case Ins::INC_ZPX:
            {
                Word Address = AddrZeroPageX();
                Byte Value = ReadByte( Address );
                Value++;
                m_cycles--;
                WriteByte( Value, Address );
                SetZeroAndNegativeFlags( Value );
            } break;
            case Ins::INC_ABS:
            {
                Word Address = AddrAbsolute();
                Byte Value = ReadByte( Address );
                Value++;
                m_cycles--;
                WriteByte( Value, Address );
                SetZeroAndNegativeFlags( Value );
            } break;
            case Ins::INC_ABSX:
            {
                Word Address = AddrAbsoluteX_5();
                Byte Value = ReadByte( Address );
                Value++;
                m_cycles--;
                WriteByte( Value, Address );
                SetZeroAndNegativeFlags( Value );
            } break;
            case Ins::BEQ:
            {
                BranchIf( Flags.Z, true );
            } break;
            default:
            {
                printf("Instruction %02X not handled\n", Instr);
                throw - 1;
            } break;
        }
    }
    const s64 NumCyclesUsed = CyclesRequested - m_cycles;
    return NumCyclesUsed;
}

Word CPU::AddrZeroPage()
{
    Byte ZeroPageAddr = FetchByte();
    return ZeroPageAddr;
}

Word CPU::AddrZeroPageX()
{
    Byte ZeroPageAddr = FetchByte();
    ZeroPageAddr += X;
    m_cycles--;
    return ZeroPageAddr;
}

Word CPU::AddrZeroPageY()
{
    Byte ZeroPageAddr = FetchByte();
    ZeroPageAddr += Y;
    m_cycles--;
    return ZeroPageAddr;
}

Word CPU::AddrAbsolute()
{
    Word AbsAddress = FetchWord();
    return AbsAddress;
}

Word CPU::AddrAbsoluteX()
{
    Word AbsAddress = FetchWord();
    Word AbsAddressX = AbsAddress + X;
    const bool CrossedPageBoundary = (AbsAddress ^ AbsAddressX) >> 8;
    if ( CrossedPageBoundary )
    {
        m_cycles--;
    }

    return AbsAddressX;
}

Word CPU::AddrAbsoluteX_5()
{
    Word AbsAddress = FetchWord();
    Word AbsAddressX = AbsAddress + X;
    m_cycles--;
    return AbsAddressX;
}

Word CPU::AddrAbsoluteY()
{
    Word AbsAddress = FetchWord();
    Word AbsAddressY = AbsAddress + Y;
    const bool CrossedPageBoundary = (AbsAddress ^ AbsAddressY) >> 8;
    if ( CrossedPageBoundary )
    {
        m_cycles--;
    }

    return AbsAddressY;
}

Word CPU::AddrIndirectX()
{
    Byte ZPAddress = FetchByte();
    ZPAddress += X;
    m_cycles--;
    Word EffectiveAddr = ReadWord(ZPAddress);
    return EffectiveAddr;
}

Word CPU::AddrIndirectY()
{
    Byte ZPAddress = FetchByte();
    Word EffectiveAddr = ReadWord( ZPAddress );
    Word EffectiveAddrY = EffectiveAddr + Y;
    const bool CrossedPageBoundary = (EffectiveAddr ^ EffectiveAddrY) >> 8;
    if ( CrossedPageBoundary )
    {
        m_cycles--;
    }
    return EffectiveAddrY;
}

Word CPU::AddrAbsoluteY_5()
{
    Word AbsAddress = FetchWord();
    Word AbsAddressY = AbsAddress + Y;	
    m_cycles--;
    return AbsAddressY;
}

Word CPU::AddrIndirectX_6()
{
    Byte ZPAddress = FetchByte();
    Word EffectiveAddr = ReadWord( ZPAddress );
    Word EffectiveAddrY = EffectiveAddr + X;
    m_cycles--;
    return EffectiveAddrY;
}

Word CPU::AddrIndirectY_6()
{
    Byte ZPAddress = FetchByte();
    Word EffectiveAddr = ReadWord( ZPAddress );
    Word EffectiveAddrY = EffectiveAddr + Y;
    m_cycles--;
    return EffectiveAddrY;
}

Word CPU::LoadPrg( const Byte* Program, u32 NumBytes ) const
{
    Word LoadAddress = 0;
    if ( Program && NumBytes > 2 )
    {
        u32 At = 0;
        const Word Lo = Program[At++];
        const Word Hi = Program[At++] << 8;
        LoadAddress = Lo | Hi;
        for ( Word i = LoadAddress; i < LoadAddress+NumBytes-2; i++ )
        {
            //TODO: mem copy?
        m_memory[i] = Program[At++];
        }
    }

    return LoadAddress;
}

}