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

CCPU::CCPU(CBus& pBus) : CBusChip(pBus, 0xFFFF, 0)
{
    reset();
    _cycles= 0;
}

/*****************************************************************************/

CCPU::CCPU(const CCPU& pCopy) : CRegisters(pCopy), CBusChip(pCopy)
{
    _cycles = pCopy._cycles;
}

/*****************************************************************************/

CCPU::~CCPU() {}

/*****************************************************************************/

void CCPU::reset()
{
    reset( 0xFFFC );
}

/*****************************************************************************/

void CCPU::reset( const Word& pResetVector )
{
    PC = pResetVector;
    SP = 0xFF;
    Flags.C = Flags.Z = Flags.I = Flags.D = Flags.B = Flags.V = Flags.N = 0;
    A = X = Y = 0;
}

/*****************************************************************************/

Byte CCPU::_fetchByte()
{
    _cycles--;
    //return ReadBusData(PC++);
    return CBusChip::bus.readBusData(PC++);
}

/*****************************************************************************/

SByte CCPU::_fetchSByte()
{
    return static_cast<SByte>(_fetchByte());
}

/*****************************************************************************/

Word CCPU::_fetchWord()
{
    // 6502 is little endian
    Word Data = bus.readBusData(PC++);
    Data |= (bus.readBusData(PC++) << 8 );
    _cycles-=2;
    return Data;
}

/*****************************************************************************/

Byte CCPU::_readByte( const Word& pAddress )
{
    _cycles--;
    return bus.readBusData(pAddress);
}

/*****************************************************************************/

Word CCPU::_readWord( const Word& pAddress )
{
    return _readByte( pAddress ) | (  _readByte( pAddress + 1 ) << 8 );
}

/*****************************************************************************/

void CCPU::_writeByte( const Byte& pValue, const Word& pAddress )
{
    bus.writeBusData( pAddress , pValue );
    _cycles--;
}

/*****************************************************************************/

void CCPU::_writeWord( const Word& pValue, const Word& pAddress )
{
    bus.writeBusData( pAddress , pValue & 0xFF);
    bus.writeBusData( pAddress + 1 , pValue >> 8);
    _cycles -= 2;
}

/*****************************************************************************/

Word CCPU::SPToAddress() const
{
    return 0x100 | SP;
}

/*****************************************************************************/

void CCPU::_pushWordToStack( const Word& pValue )
{
    _writeByte( pValue >> 8, SPToAddress());
    SP--;
    _writeByte( pValue & 0xFF, SPToAddress());
    SP--;
}

/*****************************************************************************/

void CCPU::_pushPCMinusOneToStack()
{
    _pushWordToStack( PC - 1 );
}

/*****************************************************************************/

void CCPU::_pushPCPlusOneToStack()
{
    _pushWordToStack( PC + 1 );
}

/*****************************************************************************/

void CCPU::_pushPCToStack()
{
    _pushWordToStack( PC );
}

/*****************************************************************************/

void CCPU::_pushByteOntoStack( const Byte& pValue )
{
    bus.writeBusData( SPToAddress() , pValue );
    _cycles-=2;
    SP--;
}

/*****************************************************************************/

Byte CCPU::_popByteFromStack()
{
    SP++;
    _cycles-=2;
    return bus.readBusData( SPToAddress());
}

/*****************************************************************************/

Word CCPU::_popWordFromStack()
{
    Word ValueFromStack = _readWord( SPToAddress()+1 );
    SP += 2;
    _cycles--;
    return ValueFromStack;
}

/*****************************************************************************/

void CCPU::_setZeroAndNegativeFlags( const Byte& pRegister )
{
    Flags.Z = (pRegister == 0);
    Flags.N = (pRegister & NegativeFlagBit) > 0;
}

/*****************************************************************************/

s64 CCPU::execute( s64 pCycles )
{
    s64 CyclesRequested = pCycles;
    _cycles = pCycles;
    while ( _cycles > 0)
    {
        Byte Instr = _fetchByte();
        switch (ins(Instr))
        {
            case Ins::AND_IM:
            {
                A &= _fetchByte();
                _setZeroAndNegativeFlags(A);
            } break;
            case Ins::ORA_IM:
            {
                A |= _fetchByte();
                _setZeroAndNegativeFlags(A);
            } break;
            case Ins::EOR_IM:
            {
                A ^= _fetchByte();
                _setZeroAndNegativeFlags(A);
            } break;
            case Ins::AND_ZP:
            {
                _and( _addrZeroPage() );
            } break;
            case Ins::ORA_ZP:
            {
                _ora( _addrZeroPage() );
            } break;
            case Ins::EOR_ZP:
            {
                _eor( _addrZeroPage() );
            } break;
            case Ins::AND_ZPX:
            {
                _and( _addrZeroPageX() );
            } break;
            case Ins::ORA_ZPX:
            {
                _ora( _addrZeroPageX() );
            } break;
            case Ins::EOR_ZPX:
            {
                _eor( _addrZeroPageX() );
            } break;
            case Ins::AND_ABS:
            {
                _and( _addrAbsolute() );
            } break;
            case Ins::ORA_ABS:
            {
                _ora( _addrAbsolute() );
            } break;
            case Ins::EOR_ABS:
            {
                _eor( _addrAbsolute() );
            } break;
            case Ins::AND_ABSX:
            {
                _and( _addrAbsoluteX() );
            } break;
            case Ins::ORA_ABSX:
            {
                _ora( _addrAbsoluteX() );
            } break;
            case Ins::EOR_ABSX:
            {
                _eor( _addrAbsoluteX() );
            } break;
            case Ins::AND_ABSY:
            {
                _and( _addrAbsoluteY() );
            } break;
            case Ins::ORA_ABSY:
            {
                _ora( _addrAbsoluteY() );
            } break;
            case Ins::EOR_ABSY:
            {
                _eor( _addrAbsoluteY() );
            } break;
            case Ins::AND_INDX:
            {
                _and( _addrIndirectX() );
            } break;
            case Ins::ORA_INDX:
            {
                _ora( _addrIndirectX() );
            } break;
            case Ins::EOR_INDX:
            {
                _eor( _addrIndirectX() );
            } break;
            case Ins::AND_INDY:
            {
                _and( _addrIndirectY() );
            } break;
            case Ins::ORA_INDY:
            {
                _ora( _addrIndirectY() );
            } break;
            case Ins::EOR_INDY:
            {
                _eor( _addrIndirectY() );
            } break;
            case Ins::BIT_ZP:
            {
                Byte Value = _readByte( _addrZeroPage() );
                Flags.Z = ! (A & Value);
                Flags.N = (Value & NegativeFlagBit) != 0;
                Flags.V = (Value & OverflowFlagBit) != 0;
            } break;
            case Ins::BIT_ABS:
            {
                Byte Value = _readByte( _addrAbsolute() );
                Flags.Z = ! (A & Value);
                Flags.N = (Value & NegativeFlagBit) != 0;
                Flags.V = (Value & OverflowFlagBit) != 0;
            } break;
            case Ins::LDA_IM:
            {
                A = _fetchByte ();
                _setZeroAndNegativeFlags(A);
            } break;
            case Ins::LDX_IM:
            {
                X = _fetchByte ();
                _setZeroAndNegativeFlags(X);
            } break;
            case Ins::LDY_IM:
            {
                Y = _fetchByte ();
                _setZeroAndNegativeFlags(Y);
            } break;
            case Ins::LDA_ZP:
            {
                _loadRegister ( _addrZeroPage(), A );
            } break;
            case Ins::LDX_ZP:
            {
                _loadRegister ( _addrZeroPage(), X );
            } break;
            case Ins::LDY_ZP:
            {
                _loadRegister ( _addrZeroPage(), Y );
            } break;
            case Ins::LDA_ZPX:
            {
                _loadRegister ( _addrZeroPageX(), A );
            } break;
            case Ins::LDX_ZPY:
            {
                _loadRegister ( _addrZeroPageY(), X );
            } break;
            case Ins::LDY_ZPX:
            {
                _loadRegister ( _addrZeroPageX(), Y );
            } break;
            case Ins::LDA_ABS:
            {
                _loadRegister ( _addrAbsolute(), A );
            } break;
            case Ins::LDX_ABS:
            {
                _loadRegister ( _addrAbsolute(), X );
            } break;
            case Ins::LDY_ABS:
            {
                _loadRegister ( _addrAbsolute(), Y );
            } break;
            case Ins::LDA_ABSX:
            {
                _loadRegister ( _addrAbsoluteX(), A );
            } break;
            case Ins::LDA_ABSY:
            {
                _loadRegister ( _addrAbsoluteY(), A );
            } break;
            case Ins::LDX_ABSY:
            {
                _loadRegister ( _addrAbsoluteY(), X );
            } break;
            case Ins::LDY_ABSX:
            {
                _loadRegister ( _addrAbsoluteX(), Y );
            } break;
            case Ins::LDA_INDX:
            {
                _loadRegister ( _addrIndirectX(), A );
            } break;
            case Ins::LDA_INDY:
            {
                _loadRegister ( _addrIndirectY(), A );
            } break;
            case Ins::STA_ZP:
            {
                _writeByte ( A , _addrZeroPage() );
            } break;
            case Ins::STX_ZP:
            {
                _writeByte ( X , _addrZeroPage() );
            } break;
            case Ins::STY_ZP:
            {
                _writeByte ( Y , _addrZeroPage() );
            } break;
            case Ins::STA_ABS:
            {
                _writeByte ( A , _addrAbsolute() );
            } break;
            case Ins::STX_ABS:
            {
                _writeByte ( X , _addrAbsolute() );
            } break;
            case Ins::STY_ABS:
            {
                _writeByte ( Y , _addrAbsolute() );
            } break;
            case Ins::STA_ZPX:
            {
                _writeByte ( A , _addrZeroPageX() );
            } break;
            case Ins::STY_ZPX:
            {
                _writeByte ( Y , _addrZeroPageX() );
            } break;
            case Ins::STA_ABSX:
            {
                _writeByte ( A , _addrAbsoluteX_5() );
            } break;
            case Ins::STA_ABSY:
            {
                _writeByte ( A , _addrAbsoluteY_5() );
            } break;
            case Ins::STX_ZPY:
            {
                _writeByte ( X , _addrZeroPageY() );
            } break;
            case Ins::STA_INDX:
            {
                _writeByte ( A , _addrIndirectX_6() );
            } break;
            case Ins::STA_INDY:
            {
                _writeByte ( A , _addrIndirectY_6() );
            } break;
            case Ins::JSR:
            {
                Word SubAddr = _fetchWord();
                _pushPCMinusOneToStack();
                PC = SubAddr;
                _cycles--;
            } break;
            case Ins::RTS:
            {
                PC = _popWordFromStack() + 1;	
                _cycles -= 2;
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
                PC = _addrAbsolute();
            } break;
            case Ins::JMP_IND:
            {
                PC = _readWord( _addrAbsolute() );
            } break;
            case Ins::TSX:
            {
                X = SP;
                _cycles--;
                _setZeroAndNegativeFlags( X );
            } break;
            case Ins::TXS:
            {
                SP = X;
                _cycles--;
            } break;
            case Ins::PHA:
            {
                _pushByteOntoStack( A );
            } break;
            case Ins::PLA:
            {
                A = _popByteFromStack();
                _setZeroAndNegativeFlags( A );
                _cycles--;
            } break;
            case Ins::PHP:
            {
                _pushPSToStack();
            } break;
            case Ins::PLP:
            {
                _popPSFromStack();
                _cycles--;
            } break;
            case Ins::TAX:
            {
                X = A;
                _cycles--;
                _setZeroAndNegativeFlags( X );
            } break;
            case Ins::TAY:
            {
                Y = A;
                _cycles--;
                _setZeroAndNegativeFlags( Y );
            } break;
            case Ins::TXA:
            {
                A = X;
                _cycles--;
                _setZeroAndNegativeFlags( A );
            } break;
            case Ins::TYA:
            {
                A = Y;
                _cycles--;
                _setZeroAndNegativeFlags( A );
            } break;
            case Ins::INX:
            {
                X++;
                _cycles--;
                _setZeroAndNegativeFlags( X );
            } break;
            case Ins::INY:
            {
                Y++;
                _cycles--;
                _setZeroAndNegativeFlags( Y );
            } break;
            case Ins::DEX:
            {
                X--;
                _cycles--;
                _setZeroAndNegativeFlags( X );
            } break;
            case Ins::DEY:
            {
                Y--;
                _cycles--;
                _setZeroAndNegativeFlags( Y );
            } break;
            case Ins::DEC_ZP:
            {
                Word Address = _addrZeroPage();
                Byte Value = _readByte( Address );
                Value--;
                _cycles--;
                _writeByte( Value, Address );
                _setZeroAndNegativeFlags( Value );
            } break;
            case Ins::DEC_ZPX:
            {
                Word Address = _addrZeroPageX();
                Byte Value = _readByte( Address );
                Value--;
                _cycles--;
                _writeByte( Value, Address );
                _setZeroAndNegativeFlags( Value );
            } break;
            case Ins::DEC_ABS:
            {
                Word Address = _addrAbsolute();
                Byte Value = _readByte( Address );
                Value--;
                _cycles--;
                _writeByte( Value, Address );
                _setZeroAndNegativeFlags( Value );
            } break;
            case Ins::DEC_ABSX:
            {
                Word Address = _addrAbsoluteX_5();
                Byte Value = _readByte( Address );
                Value--;
                _cycles--;
                _writeByte( Value, Address );
                _setZeroAndNegativeFlags( Value );
            } break;
            case Ins::INC_ZP:
            {
                Word Address = _addrZeroPage();
                Byte Value = _readByte( Address );
                Value++;
                _cycles--;
                _writeByte( Value, Address );
                _setZeroAndNegativeFlags( Value );
            } break;
            case Ins::INC_ZPX:
            {
                Word Address = _addrZeroPageX();
                Byte Value = _readByte( Address );
                Value++;
                _cycles--;
                _writeByte( Value, Address );
                _setZeroAndNegativeFlags( Value );
            } break;
            case Ins::INC_ABS:
            {
                Word Address = _addrAbsolute();
                Byte Value = _readByte( Address );
                Value++;
                _cycles--;
                _writeByte( Value, Address );
                _setZeroAndNegativeFlags( Value );
            } break;
            case Ins::INC_ABSX:
            {
                Word Address = _addrAbsoluteX_5();
                Byte Value = _readByte( Address );
                Value++;
                _cycles--;
                _writeByte( Value, Address );
                _setZeroAndNegativeFlags( Value );
            } break;
            case Ins::BEQ:
            {
                _branchIf( Flags.Z, true );
            } break;
            case Ins::BNE:
            {
                _branchIf( Flags.Z, false );
            } break;
            case Ins::BSC:
            {
                _branchIf( Flags.C, true );
            } break;
            case Ins::BCC:
            {
                _branchIf( Flags.C, false );
            } break;
            case Ins::BMI:
            {
                _branchIf( Flags.N, true );
            } break;
            case Ins::BPL:
            {
                _branchIf( Flags.N, false );
            } break;
            case Ins::BVC:
            {
                _branchIf( Flags.V, false );
            } break;
            case Ins::BVS:
            {
                _branchIf( Flags.V, true );
            } break;
            case Ins::CLC:
            {
                Flags.C = false;
                _cycles--;
            } break;
            case Ins::SEC:
            {
                Flags.C = true;
                _cycles--;
            } break;
            case Ins::CLD:
            {
                Flags.D = false;
                _cycles--;
            } break;
            case Ins::SED:
            {
                Flags.D = true;
                _cycles--;
            } break;
            case Ins::CLI:
            {
                Flags.I = false;
                _cycles--;
            } break;
            case Ins::SEI:
            {
                Flags.I = true;
                _cycles--;
            } break;
            case Ins::CLV:
            {
                Flags.V = false;
                _cycles--;
            } break;
            case Ins::NOP:
            {
                _cycles--;
            } break;
            case Ins::ADC_ABS:
            {
                _ADC( _readByte( _addrAbsolute() ) );
            } break;
            case Ins::ADC_ABSX:
            {
                _ADC( _readByte( _addrAbsoluteX() ) );
            } break;
            case Ins::ADC_ABSY:
            {
                _ADC( _readByte( _addrAbsoluteY() ) );
            } break;
            case Ins::ADC_ZP:
            {
                _ADC( _readByte( _addrZeroPage() ) );
            } break;
            case Ins::ADC_ZPX:
            {
                _ADC( _readByte( _addrZeroPageX() ) );
            } break;
            case Ins::ADC_INDX:
            {
                _ADC( _readByte( _addrIndirectX() ) );
            } break;
            case Ins::ADC_INDY:
            {
                _ADC( _readByte( _addrIndirectY() ) );
            } break;
            case Ins::ADC:
            {
                _ADC( _fetchByte() );
            } break;
            case Ins::SBC:
            {
                _SBC( _fetchByte() );
            } break;
            case Ins::SBC_ABS:
            {
                _SBC( _readByte( _addrAbsolute() ) );
            } break;
            case Ins::SBC_ZP:
            {
                _SBC( _readByte( _addrZeroPage() ) );
            } break;
            case Ins::SBC_ZPX:
            {
                _SBC( _readByte( _addrZeroPageX() ) );
            } break;
            case Ins::SBC_ABSX:
            {
                _SBC( _readByte ( _addrAbsoluteX() ) );
            } break;
            case Ins::SBC_ABSY:
            {
                _SBC( _readByte( _addrAbsoluteY() ) );
            } break;
            case Ins::SBC_INDX:
            {
                _SBC( _readByte( _addrIndirectX() ) );
            } break;
            case Ins::SBC_INDY:
            {
                _SBC( _readByte( _addrIndirectY() ) );
            } break;
            case Ins::CPX:
            {
                _registerCompare( _fetchByte() , X );
            } break;
            case Ins::CPY:
            {
                _registerCompare( _fetchByte(), Y );
            } break;
            case Ins::CPX_ZP:
            {
                _registerCompare( _readByte( _addrZeroPage() ), X );
            } break;
            case Ins::CPY_ZP:
            {
                _registerCompare( _readByte( _addrZeroPage() ), Y );
            } break;
            case Ins::CPX_ABS:
            {
                _registerCompare( _readByte ( _addrAbsolute () ), X );
            } break;
            case Ins::CPY_ABS:
            {
                _registerCompare( _readByte ( _addrAbsolute () ), Y );
            } break;
            case Ins::CMP:
            {
                _registerCompare( _fetchByte(), A );
            } break;
            case Ins::CMP_ZP:
            {
                _registerCompare( _readByte( _addrZeroPage() ), A );
            } break;
            case Ins::CMP_ZPX:
            {
                _registerCompare( _readByte( _addrZeroPageX() ), A );
            } break;
            case Ins::CMP_ABS:
            {
                _registerCompare( _readByte( _addrAbsolute() ), A );
            } break;
            case Ins::CMP_ABSX:
            {
                _registerCompare( _readByte( _addrAbsoluteX() ), A );
            } break;
            case Ins::CMP_ABSY:
            {
                _registerCompare( _readByte( _addrAbsoluteY() ), A );
            } break;
            case Ins::CMP_INDX:
            {
                _registerCompare( _readByte( _addrIndirectX() ), A );
            } break;
            case Ins::CMP_INDY:
            {
                _registerCompare( _readByte( _addrIndirectY() ), A );
            } break;
            case Ins::ASL:
            {
                A = _ASL( A );
            } break;
            case Ins::ASL_ZP:
            {
                Word Address = _addrZeroPage();
                Byte Operand = _readByte( Address );
                Byte Result = _ASL( Operand );
                _writeByte( Result, Address );
            } break;
            case Ins::ASL_ZPX:
            {
                Word Address = _addrZeroPageX();
                Byte Operand = _readByte( Address );
                Byte Result = _ASL( Operand );
                _writeByte( Result, Address );
            } break;
            case Ins::ASL_ABS:
            {
                Word Address = _addrAbsolute();
                Byte Operand = _readByte( Address );
                Byte Result = _ASL( Operand );
                _writeByte( Result, Address );
            } break;
            case Ins::ASL_ABSX:
            {
                Word Address = _addrAbsoluteX_5();
                Byte Operand = _readByte( Address );
                Byte Result = _ASL( Operand );
                _writeByte( Result, Address );
            } break;
            case Ins::LSR:
            {
                A = _LSR( A );
            } break;
            case Ins::LSR_ZP:
            {
                Word Address = _addrZeroPage();
                Byte Operand = _readByte( Address );
                Byte Result = _LSR( Operand );
                _writeByte( Result, Address );
            } break;
            case Ins::LSR_ZPX:
            {
                Word Address = _addrZeroPageX();
                Byte Operand = _readByte( Address );
                Byte Result = _LSR( Operand );
                _writeByte( Result, Address );
            } break;
            case Ins::LSR_ABS:
            {
                Word Address = _addrAbsolute();
                Byte Operand = _readByte( Address );
                Byte Result = _LSR( Operand );
                _writeByte( Result, Address );
            } break;
            case Ins::LSR_ABSX:
            {
                Word Address = _addrAbsoluteX_5();
                Byte Operand = _readByte( Address );
                Byte Result = _LSR( Operand );
                _writeByte( Result, Address );
            } break;
            case Ins::ROL:
            {
                A = _ROL( A );
            } break;
            case Ins::ROL_ZP:
            {
                Word Address = _addrZeroPage( );
                Byte Operand = _readByte( Address );
                Byte Result = _ROL( Operand );
                _writeByte( Result, Address );
            } break;
            case Ins::ROL_ZPX:
            {
                Word Address = _addrZeroPageX();
                Byte Operand = _readByte( Address );
                Byte Result = _ROL( Operand );
                _writeByte( Result, Address );
            } break;
            case Ins::ROL_ABS:
            {
                Word Address = _addrAbsolute();
                Byte Operand = _readByte( Address );
                Byte Result = _ROL( Operand );
                _writeByte( Result, Address );
            } break;
            case Ins::ROL_ABSX:
            {
                Word Address = _addrAbsoluteX_5();
                Byte Operand = _readByte( Address );
                Byte Result = _ROL( Operand );
                _writeByte( Result, Address );
            } break;
            case Ins::ROR:
            {
                A = _ROR( A );
            } break;
            case Ins::ROR_ZP:
            {
                Word Address = _addrZeroPage();
                Byte Operand = _readByte( Address );			
                Byte Result = _ROR( Operand );
                _writeByte( Result, Address );
            } break;
            case Ins::ROR_ZPX:
            {
                Word Address = _addrZeroPageX( );
                Byte Operand = _readByte( Address );
                Byte Result = _ROR( Operand );
                _writeByte( Result, Address );
            } break;
            case Ins::ROR_ABS:
            {
                Word Address = _addrAbsolute();
                Byte Operand = _readByte( Address );
                Byte Result = _ROR( Operand );
                _writeByte( Result, Address );
            } break;
            case Ins::ROR_ABSX:
            {
                Word Address = _addrAbsoluteX_5();
                Byte Operand = _readByte(Address);
                Byte Result = _ROR( Operand );
                _writeByte( Result, Address );
            } break;
            case Ins::BRK:
            {
                _pushPCPlusOneToStack();
                _pushPSToStack();
                constexpr Word InterruptVector = 0xFFFE;
                PC = _readWord( InterruptVector );
                Flags.B = true;
                Flags.I = true;
            } break;
            case Ins::RTI:
            {
                _popPSFromStack();
                PC = _popWordFromStack();
            } break;
            default:
            {
                printf("Instruction %02X not handled\n", Instr);
                throw - 1;
            } break;
        }
    }
    const s64 NumCyclesUsed = CyclesRequested - _cycles;
    return NumCyclesUsed;
}

/*****************************************************************************/

Word CCPU::_addrZeroPage()
{
    return static_cast<Word>(_fetchByte());
}

/*****************************************************************************/

Word CCPU::_addrZeroPageX()
{
    Byte ZeroPageAddr = _fetchByte();
    ZeroPageAddr += X;
    _cycles--;
    return ZeroPageAddr;
}

/*****************************************************************************/

Word CCPU::_addrZeroPageY()
{
    Byte ZeroPageAddr = _fetchByte();
    ZeroPageAddr += Y;
    _cycles--;
    return ZeroPageAddr;
}

/*****************************************************************************/

Word CCPU::_addrAbsolute()
{
    return _fetchWord();
}

/*****************************************************************************/

Word CCPU::_addrAbsoluteX()
{
    Word AbsAddress = _fetchWord();
    Word AbsAddressX = AbsAddress + X;
    const bool CrossedPageBoundary = (AbsAddress ^ AbsAddressX) >> 8;
    if ( CrossedPageBoundary )
    {
        _cycles--;
    }

    return AbsAddressX;
}

/*****************************************************************************/

Word CCPU::_addrAbsoluteX_5()
{
    Word AbsAddress = _fetchWord();
    _cycles--;
    return AbsAddress + X;
}

/*****************************************************************************/

Word CCPU::_addrAbsoluteY()
{
    Word AbsAddress = _fetchWord();
    Word AbsAddressY = AbsAddress + Y;
    const bool CrossedPageBoundary = (AbsAddress ^ AbsAddressY) >> 8;
    if ( CrossedPageBoundary )
    {
        _cycles--;
    }

    return AbsAddressY;
}

/*****************************************************************************/

Word CCPU::_addrIndirectX()
{
    _cycles--;
    return _readWord(_fetchByte() + X);
}

/*****************************************************************************/

Word CCPU::_addrIndirectY()
{
    Byte ZPAddress = _fetchByte();
    Word EffectiveAddr = _readWord( ZPAddress );
    Word EffectiveAddrY = EffectiveAddr + Y;
    const bool CrossedPageBoundary = (EffectiveAddr ^ EffectiveAddrY) >> 8;
    if ( CrossedPageBoundary )
    {
        _cycles--;
    }
    return EffectiveAddrY;
}

/*****************************************************************************/

Word CCPU::_addrAbsoluteY_5()
{
    _cycles--;
    return _fetchWord() + Y;
}

/*****************************************************************************/

Word CCPU::_addrIndirectX_6()
{
    _cycles--;
    return _readWord( _fetchByte() ) + X;
}

/*****************************************************************************/

Word CCPU::_addrIndirectY_6()
{
    _cycles--;
    return _readWord( _fetchByte() ) + Y;
}

/*****************************************************************************/

Word CCPU::loadPrg( const Byte* pProgram, u32 NumBytes )
{
    Word LoadAddress = 0;
    if ( pProgram && NumBytes > 2 )
    {
        Word At = 0;
        const Word Lo = pProgram[At++];
        const Word Hi = pProgram[At++] << 8;
        LoadAddress = Lo | Hi;
        for ( Word Addr = LoadAddress; Addr < LoadAddress+NumBytes-2; Addr++ )
        {
            //TODO: mem copy?
            bus.writeBusData(Addr, pProgram[At++]);
        }
        PC = LoadAddress;
    }
    return LoadAddress;
}

/*****************************************************************************/

void CCPU::_loadRegister(Word pAddress, Byte& pRegister)
{
    pRegister = _readByte ( pAddress );
    _setZeroAndNegativeFlags( pRegister );
}

/*****************************************************************************/

void CCPU::_and( Word pAddress )
{
    A &= _readByte( pAddress );
    _setZeroAndNegativeFlags( A );
}

/*****************************************************************************/

void CCPU::_ora( Word pAddress )
{
    A |= _readByte( pAddress );
    _setZeroAndNegativeFlags( A );
};

/*****************************************************************************/

void CCPU::_eor( Word pAddress )
{
    A ^= _readByte( pAddress );
    _setZeroAndNegativeFlags( A );
};

/*****************************************************************************/

void CCPU::_branchIf( bool pTest, bool pExpected )
{
    SByte Offset = _fetchSByte();
    if ( pTest == pExpected )
    {
        const Word PCOld = PC;
        PC += Offset;
        _cycles--;

        const bool PageChanged = (PC >> 8) != (PCOld >> 8);
        if ( PageChanged )
        {
            _cycles--;
        }
    }
};

/*****************************************************************************/

void CCPU::_ADC( Byte pOperand )
{
    ASSERT( Flags.D == false, "haven't handled decimal mode!" );
    const bool AreSignBitsTheSame =
        !((A ^ pOperand) & NegativeFlagBit);
    Word Sum = static_cast<Word>(A);
    Sum += pOperand;
    Sum += Flags.C;
    A = (Sum & 0xFF);
    _setZeroAndNegativeFlags( A );
    Flags.C = Sum > 0xFF;
    Flags.V = AreSignBitsTheSame &&
        ((A ^ pOperand) & NegativeFlagBit);
};
    
/*****************************************************************************/

void CCPU::_SBC( Byte pOperand )
{
    _ADC( ~pOperand );
};

/*****************************************************************************/

void CCPU::_registerCompare( Byte pOperand, Byte pRegisterValue )
{
    Byte Temp = pRegisterValue - pOperand;
    Flags.N = (Temp & NegativeFlagBit) > 0;
    Flags.Z = pRegisterValue == pOperand;
    Flags.C = pRegisterValue >= pOperand;
};

/*****************************************************************************/

Byte CCPU::_ASL( Byte pOperand )
{
    Flags.C = (pOperand & NegativeFlagBit) > 0;
    Byte Result = pOperand << 1;
    _setZeroAndNegativeFlags( Result );
    _cycles--;
    return Result;
};

/*****************************************************************************/

Byte CCPU::_LSR( Byte pOperand )
{
    Flags.C = (pOperand & ZeroBit) > 0;
    Byte Result = pOperand >> 1;
    _setZeroAndNegativeFlags( Result );
    _cycles--;
    return Result;
};

/*****************************************************************************/

Byte CCPU::_ROL( Byte pOperand )
{
    Byte NewBit0 = Flags.C ? ZeroBit : 0;
    Flags.C = (pOperand & NegativeFlagBit) > 0;
    pOperand = pOperand << 1;
    pOperand |= NewBit0;
    _setZeroAndNegativeFlags( pOperand );
    _cycles--;
    return pOperand;
};

/*****************************************************************************/

Byte CCPU::_ROR( Byte pOperand )
{
    bool OldBit0 = (pOperand & ZeroBit) > 0;
    pOperand = pOperand >> 1;
    if ( Flags.C )
    {
        pOperand |= NegativeFlagBit;
    }
    _cycles--;
    Flags.C = OldBit0;
    _setZeroAndNegativeFlags( pOperand );
    return pOperand;
};

/*****************************************************************************/

void CCPU::_pushPSToStack()
{
    Byte PSStack = PS | BreakFlagBit | UnusedFlagBit;		
    _pushByteOntoStack( PSStack );
};

/*****************************************************************************/

void CCPU::_popPSFromStack()
{
    PS = _popByteFromStack();
    Flags.B = false;
    Flags.Unused = false;
};

}