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

/*****************************************************************************/

CPU::CPU(Mem& mem) : m_memory(mem)
{
    Reset();
    m_cycles= 0;
}

/*****************************************************************************/

CPU::CPU(const CPU& copy) : Registers(copy), m_memory(copy.m_memory)
{
    m_cycles = copy.m_cycles;
}

/*****************************************************************************/

CPU::~CPU() {}

/*****************************************************************************/

void CPU::Reset()
{
    Reset( 0xFFFC );
}

/*****************************************************************************/

void CPU::Reset( Word ResetVector )
{
    PC = ResetVector;
    SP = 0xFF;
    Flags.C = Flags.Z = Flags.I = Flags.D = Flags.B = Flags.V = Flags.N = 0;
    A = X = Y = 0;
    m_memory.Initialise();
}

/*****************************************************************************/

Byte CPU::FetchByte()
{
    Byte Data = m_memory[PC];
    PC++;
    m_cycles--;
    return Data;
}

/*****************************************************************************/

SByte CPU::FetchSByte()
{
    return FetchByte();
}

/*****************************************************************************/

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

/*****************************************************************************/

Byte CPU::ReadByte( Word Address )
{
    Byte Data = m_memory[Address];
    m_cycles--;
    return Data;
}

/*****************************************************************************/

Word CPU::ReadWord( Word Address )
{
    Byte LoByte = ReadByte( Address );
    Byte HiByte = ReadByte( Address + 1 );
    return LoByte | (HiByte << 8);
}

/*****************************************************************************/

void CPU::WriteByte( Byte Value, Word Address )
{
    m_memory[Address] = Value;
    m_cycles--;
}

/*****************************************************************************/

void CPU::WriteWord( Word Value, Word Address )
{
    m_memory[Address] = Value & 0xFF;
    m_memory[Address + 1] = (Value >> 8);
    m_cycles -= 2;
}

/*****************************************************************************/

Word CPU::SPToAddress() const
{
    return 0x100 | SP;
}

/*****************************************************************************/

void CPU::PushWordToStack( Word Value )
{
    WriteByte( Value >> 8, SPToAddress());
    SP--;
    WriteByte( Value & 0xFF, SPToAddress());
    SP--;
}

/*****************************************************************************/

void CPU::PushPCMinusOneToStack()
{
    PushWordToStack( PC - 1 );
}

/*****************************************************************************/

void CPU::PushPCPlusOneToStack()
{
    PushWordToStack( PC + 1 );
}

/*****************************************************************************/

void CPU::PushPCToStack()
{
    PushWordToStack( PC );
}

/*****************************************************************************/

void CPU::PushByteOntoStack( Byte Value )
{
    const Word SPWord = SPToAddress();
    m_memory[SPWord] = Value;
    m_cycles--;
    SP--;
    m_cycles--;
}

/*****************************************************************************/

Byte CPU::PopByteFromStack()
{
    SP++;
    m_cycles--;
    const Word SPWord = SPToAddress();
    Byte Value = m_memory[SPWord];
    m_cycles--;
    return Value;
}

/*****************************************************************************/

Word CPU::PopWordFromStack()
{
    Word ValueFromStack = ReadWord( SPToAddress()+1 );
    SP += 2;
    m_cycles--;
    return ValueFromStack;
}

/*****************************************************************************/

void CPU::SetZeroAndNegativeFlags( Byte Register )
{
    Flags.Z = (Register == 0);
    Flags.N = (Register & NegativeFlagBit) > 0;
}

/*****************************************************************************/

s64 CPU::Execute( s64 cycles )
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

    /** Do add with carry given the the operand */
	auto ADC = [ this ] ( Byte Operand )
	{
		ASSERT( Flags.D == false, "haven't handled decimal mode!" );
		const bool AreSignBitsTheSame =
			!((A ^ Operand) & NegativeFlagBit);
		Word Sum = static_cast<Word>(A);
		Sum += Operand;
		Sum += Flags.C;
		A = (Sum & 0xFF);
		SetZeroAndNegativeFlags( A );
		Flags.C = Sum > 0xFF;
		Flags.V = AreSignBitsTheSame &&
			((A ^ Operand) & NegativeFlagBit);
	};
    
    /** Do subtract with carry given the the operand */
	auto SBC = [&ADC] ( Byte Operand )
	{
		ADC( ~Operand );
	};

    /** Do compare accumulator to operand*/
    auto CMP = [ this ] ( Byte Operand )
    {
        Byte Temp = A - Operand;
        Flags.N = (Temp & NegativeFlagBit) > 0;
        Flags.Z = A == Operand;
        Flags.C = A >= Operand;
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
    s64 CyclesRequested = cycles;
    m_cycles = cycles;
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
                And( AddrZeroPage() );
            } break;
            case Ins::ORA_ZP:
            {
                Ora( AddrZeroPage() );
            } break;
            case Ins::EOR_ZP:
            {
                Eor( AddrZeroPage() );
            } break;
            case Ins::AND_ZPX:
            {
                And( AddrZeroPageX() );
            } break;
            case Ins::ORA_ZPX:
            {
                Ora( AddrZeroPageX() );
            } break;
            case Ins::EOR_ZPX:
            {
                Eor( AddrZeroPageX() );
            } break;
            case Ins::AND_ABS:
            {
                And( AddrAbsolute() );
            } break;
            case Ins::ORA_ABS:
            {
                Ora( AddrAbsolute() );
            } break;
            case Ins::EOR_ABS:
            {
                Eor( AddrAbsolute() );
            } break;
            case Ins::AND_ABSX:
            {
                And( AddrAbsoluteX() );
            } break;
            case Ins::ORA_ABSX:
            {
                Ora( AddrAbsoluteX() );
            } break;
            case Ins::EOR_ABSX:
            {
                Eor( AddrAbsoluteX() );
            } break;
            case Ins::AND_ABSY:
            {
                And( AddrAbsoluteY() );
            } break;
            case Ins::ORA_ABSY:
            {
                Ora( AddrAbsoluteY() );
            } break;
            case Ins::EOR_ABSY:
            {
                Eor( AddrAbsoluteY() );
            } break;
            case Ins::AND_INDX:
            {
                And( AddrIndirectX() );
            } break;
            case Ins::ORA_INDX:
            {
                Ora( AddrIndirectX() );
            } break;
            case Ins::EOR_INDX:
            {
                Eor( AddrIndirectX() );
            } break;
            case Ins::AND_INDY:
            {
                And( AddrIndirectY() );
            } break;
            case Ins::ORA_INDY:
            {
                Ora( AddrIndirectY() );
            } break;
            case Ins::EOR_INDY:
            {
                Eor( AddrIndirectY() );
            } break;
            case Ins::BIT_ZP:
            {
                Byte Value = ReadByte( AddrZeroPage() );
                Flags.Z = ! (A & Value);
                Flags.N = (Value & NegativeFlagBit) != 0;
                Flags.V = (Value & OverflowFlagBit) != 0;
            } break;
            case Ins::BIT_ABS:
            {
                Byte Value = ReadByte( AddrAbsolute() );
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
                LoadRegister ( AddrZeroPage(), A );
            } break;
            case Ins::LDX_ZP:
            {
                LoadRegister ( AddrZeroPage(), X );
            } break;
            case Ins::LDY_ZP:
            {
                LoadRegister ( AddrZeroPage(), Y );
            } break;
            case Ins::LDA_ZPX:
            {
                LoadRegister ( AddrZeroPageX(), A );
            } break;
            case Ins::LDX_ZPY:
            {
                LoadRegister ( AddrZeroPageY(), X );
            } break;
            case Ins::LDY_ZPX:
            {
                LoadRegister ( AddrZeroPageX(), Y );
            } break;
            case Ins::LDA_ABS:
            {
                LoadRegister ( AddrAbsolute(), A );
            } break;
            case Ins::LDX_ABS:
            {
                LoadRegister ( AddrAbsolute(), X );
            } break;
            case Ins::LDY_ABS:
            {
                LoadRegister ( AddrAbsolute(), Y );
            } break;
            case Ins::LDA_ABSX:
            {
                LoadRegister ( AddrAbsoluteX(), A );
            } break;
            case Ins::LDA_ABSY:
            {
                LoadRegister ( AddrAbsoluteY(), A );
            } break;
            case Ins::LDX_ABSY:
            {
                LoadRegister ( AddrAbsoluteY(), X );
            } break;
            case Ins::LDY_ABSX:
            {
                LoadRegister ( AddrAbsoluteX(), Y );
            } break;
            case Ins::LDA_INDX:
            {
                LoadRegister ( AddrIndirectX(), A );
            } break;
            case Ins::LDA_INDY:
            {
                LoadRegister ( AddrIndirectY(), A );
            } break;
            case Ins::STA_ZP:
            {
                WriteByte ( A , AddrZeroPage() );
            } break;
            case Ins::STX_ZP:
            {
                WriteByte ( X , AddrZeroPage() );
            } break;
            case Ins::STY_ZP:
            {
                WriteByte ( Y , AddrZeroPage() );
            } break;
            case Ins::STA_ABS:
            {
                WriteByte ( A , AddrAbsolute() );
            } break;
            case Ins::STX_ABS:
            {
                WriteByte ( X , AddrAbsolute() );
            } break;
            case Ins::STY_ABS:
            {
                WriteByte ( Y , AddrAbsolute() );
            } break;
            case Ins::STA_ZPX:
            {
                WriteByte ( A , AddrZeroPageX() );
            } break;
            case Ins::STY_ZPX:
            {
                WriteByte ( Y , AddrZeroPageX() );
            } break;
            case Ins::STA_ABSX:
            {
                WriteByte ( A , AddrAbsoluteX_5() );
            } break;
            case Ins::STA_ABSY:
            {
                WriteByte ( A , AddrAbsoluteY_5() );
            } break;
            case Ins::STX_ZPY:
            {
                WriteByte ( X , AddrZeroPageY() );
            } break;
            case Ins::STA_INDX:
            {
                WriteByte ( A , AddrIndirectX_6() );
            } break;
            case Ins::STA_INDY:
            {
                WriteByte ( A , AddrIndirectY_6() );
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
                PC = PopWordFromStack() + 1;	
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
                PC = AddrAbsolute();
            } break;
            case Ins::JMP_IND:
            {
                PC = ReadWord( AddrAbsolute() );
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
            case Ins::BNE:
            {
                BranchIf( Flags.Z, false );
            } break;
            case Ins::BSC:
            {
                BranchIf( Flags.C, true );
            } break;
            case Ins::BCC:
            {
                BranchIf( Flags.C, false );
            } break;
            case Ins::BMI:
            {
                BranchIf( Flags.N, true );
            } break;
            case Ins::BPL:
            {
                BranchIf( Flags.N, false );
            } break;
            case Ins::BVC:
            {
                BranchIf( Flags.V, false );
            } break;
            case Ins::BVS:
            {
                BranchIf( Flags.V, true );
            } break;
            case Ins::CLC:
            {
                Flags.C = false;
                m_cycles--;
            } break;
            case Ins::SEC:
            {
                Flags.C = true;
                m_cycles--;
            } break;
            case Ins::CLD:
            {
                Flags.D = false;
                m_cycles--;
            } break;
            case Ins::SED:
            {
                Flags.D = true;
                m_cycles--;
            } break;
            case Ins::CLI:
            {
                Flags.I = false;
                m_cycles--;
            } break;
            case Ins::SEI:
            {
                Flags.I = true;
                m_cycles--;
            } break;
            case Ins::CLV:
            {
                Flags.V = false;
                m_cycles--;
            } break;
            case Ins::NOP:
            {
                m_cycles--;
            } break;
            case Ins::ADC_ABS:
            {
                ADC( ReadByte( AddrAbsolute() ) );
            } break;
            case Ins::ADC_ABSX:
            {
                ADC( ReadByte( AddrAbsoluteX() ) );
            } break;
            case Ins::ADC_ABSY:
            {
                ADC( ReadByte( AddrAbsoluteY() ) );
            } break;
            case Ins::ADC_ZP:
            {
                ADC( ReadByte( AddrZeroPage() ) );
            } break;
            case Ins::ADC_ZPX:
            {
                ADC( ReadByte( AddrZeroPageX() ) );
            } break;
            case Ins::ADC_INDX:
            {
                ADC( ReadByte( AddrIndirectX() ) );
            } break;
            case Ins::ADC_INDY:
            {
                ADC( ReadByte( AddrIndirectY() ) );
            } break;
            case Ins::ADC:
            {
                ADC( FetchByte() );
            } break;
            case Ins::SBC:
            {
                SBC( FetchByte() );
            } break;
            case Ins::SBC_ABS:
            {
                SBC( ReadByte( AddrAbsolute() ) );
            } break;
            case Ins::SBC_ZP:
            {
                SBC( ReadByte( AddrZeroPage() ) );
            } break;
            case Ins::SBC_ZPX:
            {
                SBC( ReadByte( AddrZeroPageX() ) );
            } break;
            case Ins::SBC_ABSX:
            {
                SBC( ReadByte ( AddrAbsoluteX() ) );
            } break;
            case Ins::SBC_ABSY:
            {
                SBC( ReadByte( AddrAbsoluteY() ) );
            } break;
            case Ins::SBC_INDX:
            {
                SBC( ReadByte( AddrIndirectX() ) );
            } break;
            case Ins::SBC_INDY:
            {
                SBC( ReadByte( AddrIndirectY() ) );
            } break;
            case Ins::CMP:
            {
                CMP ( FetchByte() );
            } break;
            case Ins::CMP_ZP:
            {
                CMP ( ReadByte( AddrZeroPage() ) );
            } break;
            case Ins::CMP_ZPX:
            {
                CMP ( ReadByte( AddrZeroPageX() ) );
            } break;
            case Ins::CMP_ABS:
            {
                CMP ( ReadByte( AddrAbsolute() ) );
            } break;
            case Ins::CMP_ABSX:
            {
                CMP ( ReadByte( AddrAbsoluteX() ) );
            } break;
            case Ins::CMP_ABSY:
            {
                CMP ( ReadByte( AddrAbsoluteY() ) );
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

/*****************************************************************************/

Word CPU::AddrZeroPage()
{
    Byte ZeroPageAddr = FetchByte();
    return static_cast<Word>(ZeroPageAddr);
}

/*****************************************************************************/

Word CPU::AddrZeroPageX()
{
    Byte ZeroPageAddr = FetchByte();
    ZeroPageAddr += X;
    m_cycles--;
    return ZeroPageAddr;
}

/*****************************************************************************/

Word CPU::AddrZeroPageY()
{
    Byte ZeroPageAddr = FetchByte();
    ZeroPageAddr += Y;
    m_cycles--;
    return ZeroPageAddr;
}

/*****************************************************************************/

Word CPU::AddrAbsolute()
{
    Word AbsAddress = FetchWord();
    return AbsAddress;
}

/*****************************************************************************/

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

/*****************************************************************************/

Word CPU::AddrAbsoluteX_5()
{
    Word AbsAddress = FetchWord();
    Word AbsAddressX = AbsAddress + X;
    m_cycles--;
    return AbsAddressX;
}

/*****************************************************************************/

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

/*****************************************************************************/

Word CPU::AddrIndirectX()
{
    Byte ZPAddress = FetchByte();
    ZPAddress += X;
    m_cycles--;
    Word EffectiveAddr = ReadWord(ZPAddress);
    return EffectiveAddr;
}

/*****************************************************************************/

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

/*****************************************************************************/

Word CPU::AddrAbsoluteY_5()
{
    Word AbsAddress = FetchWord();
    Word AbsAddressY = AbsAddress + Y;	
    m_cycles--;
    return AbsAddressY;
}

/*****************************************************************************/

Word CPU::AddrIndirectX_6()
{
    Byte ZPAddress = FetchByte();
    Word EffectiveAddr = ReadWord( ZPAddress );
    Word EffectiveAddrY = EffectiveAddr + X;
    m_cycles--;
    return EffectiveAddrY;
}

/*****************************************************************************/

Word CPU::AddrIndirectY_6()
{
    Byte ZPAddress = FetchByte();
    Word EffectiveAddr = ReadWord( ZPAddress );
    Word EffectiveAddrY = EffectiveAddr + Y;
    m_cycles--;
    return EffectiveAddrY;
}

/*****************************************************************************/

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