/**
 * @file Cpu.hpp
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

#ifndef CPU_HPP
#define CPU_HPP

#include <m6502/Config.hpp>
#include <m6502/System/Bus.hpp>
#include <m6502/System/Registers.hpp>
#include <m6502/System/OpCodes.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <array>

namespace m6502
{

/**
 * @brief Registers for 6502 CPU
 * 
 */
class CCPU : public CRegisters, CBusChip
{
public:
    /**
     * @brief Construct a new CPU object
     * 
     * @param pBus 
     */
    explicit CCPU(CBus& pBus);

    /**
     * @brief Construct a new CPU object
     * 
     * @param pBus 
     */
    explicit CCPU(CBus&& pBus) = delete;

    /**
     * @brief Construct a new CPU object
     * 
     * @param pCopy 
     */
    CCPU(const CCPU& pCopy);

    /**
     * @brief Destroy the CPU object
     * 
     */
    ~CCPU() override;

    /**
     * @brief Reset the CPU object
     * 
     */
    void reset( );

    /**
     * @brief Reset the CPU with the given start vector
     * 
     * @param pResetVector 
     */
    void reset( const Word& pResetVector );

    /**
     * @brief Load program in memory
     * 
     * @param Program 
     * @param NumBytes 
     * @return the address that the rogram was loading into, or 0 if no program 
     * 
     * This method handle a Byte array containing programm.
     * The 2 first Bytes contain the memory address where
     * Load the program
     */
    Word loadPrg( const Byte* pProgram, u32 pNumBytes);

    /**
     * @brief Get the stack pointer
     * 
     * @return the stack pointer as a full 16-bit address (in the 1st page) 
     */
    Word SPToAddress() const;

    /**
     * @brief Execute specified number of cycles
     * 
     * @param Cycles 
     * @return The real numbers cycles excecuted
     */
    s64 execute( s64 pCycles);

private:

    /**
     * @brief Cycles counter down for Execution
     *        process
     * 
     */
    s64 _cycles;

    /**
     * @brief 
     * 
     * @return Byte 
     */
    Byte _fetchByte();

    /**
     * @brief 
     * 
     * @return Word 
     */
    Word _fetchWord();

    /**
     * @brief 
     * 
     * @return SByte 
     */
    SByte _fetchSByte();

    /**
     * @brief 
     * 
     * @param Address 
     * @return Byte 
     */
    Byte _readByte( const Word& Address );

    /**
     * @brief Read Word from memory at address
     * 
     * @param Address 
     * @return Word 
     */
    Word _readWord( const Word& Address );

    /**
     * @brief Write 1 Byte to memory
     * 
     * @param Value 
     * @param Address 
     */
    void _writeByte( const Byte& Value, const Word& Address );

    /**
     * @brief Write 1 Word to memory
     * 
     * @param Value 
     * @param Address 
     */
    void _writeWord(	const Word& Value, const Word& Address );

    /**
     * @brief Write 1 Word on to Stack
     * 
     * @param Value 
     */
    void _pushWordToStack( const Word& Value );

    /**
     * @brief Push the PC-1 onto the stack
     * 
     */
    void _pushPCMinusOneToStack();

    /**
     * @brief Push the PC+1 onto the stack
     * 
     */
    void _pushPCPlusOneToStack();

    /**
     * @brief Push the PC onto the stack
     * 
     */
    void _pushPCToStack();

    /**
     * @brief Push a byte on to Stack
     * 
     * @param Value 
     */
    void _pushByteOntoStack( const Byte& Value );

    /**
     * @brief Pop a byte value from the stack
     * 
     * @return Byte 
     */
    Byte _popByteFromStack();

    /**
     * @brief Pop a 16-bit value from the stack
     * 
     * @return Word 
     */
    Word _popWordFromStack();

    /**
     * @brief Sets the correct Process status after a load register instruction
     * 
     * @param Register 
     * 
     * Sets the correct Process status after a load register instruction
     * - LDA, LDX, LDY
     * @Register The A,X or Y Register
     * 
     */
    void _setZeroAndNegativeFlags( const Byte& Register );

    /**
     * @brief Addressing mode - Zero page
     * 
     * @return Word 
     */
    Word _addrZeroPage();

    /**
     * @brief Addressing mode - Zero page with X offset
     * 
     * @return Word 
     */
    Word _addrZeroPageX();

    /**
     * @brief Addressing mode - Zero page with Y offset
     * 
     * @return Word 
     */
    Word _addrZeroPageY();

    /**
     * @brief Addressing mode - Absolute
     * 
     * @return Word 
     */
    Word _addrAbsolute();

    /**
     * @brief Addressing mode - Absolute with X offset
     * 
     * @return Word
     * 
     * Addressing mode - Absolute with X offset
     * 
     */
    Word _addrAbsoluteX();

    /**
     * @brief Addressing mode - Absolute with X offset
     * 
     * @return Word
     * 
     *  Addressing mode - Absolute with X offset
     *  - Always takes a cycle for the X page boundary)
     *  - See "STA Absolute,X"
     * 
     */
    Word _addrAbsoluteX_5();

    /**
     * @brief Addressing mode - Absolute with Y offset
     * 
     * @return Word 
     */
    Word _addrAbsoluteY();

    /**
     * @brief Addressing mode - Absolute with Y offset
     * 
     * @return Word 
     * 
     *  - Always takes a cycle for the Y page boundary)
     *	- See "STA Absolute,Y"
        *
        */
    Word _addrAbsoluteY_5();

    /**
     * @brief Addressing mode - Indirect X | Indexed Indirect
     * 
     * @return Word 
     */
    Word _addrIndirectX();

    /**
     * @brief Addressing mode - Indirect Y | Indirect Indexed
     * 
     * @return Word 
     */
    Word _addrIndirectY();

    /** Addressing mode - Indirect X | Indirect Indexed
    *	- Always takes a cycle for the Y page boundary)
    *	- See "STA (Indirect,Y) */

    Word _addrIndirectX_6();

    /**
     * @brief Addressing mode - Indirect Y | Indirect Indexed
     * 
     * @return Word 
     * 
     * - Always takes a cycle for the Y page boundary)
     * - See "STA (Indirect,Y)
     */
    Word _addrIndirectY_6();

    /**
     * @brief Load the specied Register with data in memory
     * 
     * @param pAddress 
     * @param pRegister 
     */
    void _loadRegister(Word pAddress, Byte& pRegister);

    /**
     * @brief And the A Register with the value from the memory address
     * 
     * @param pAddress 
     */
    void _and( Word pAddress );

    /**
     * @brief Or the A Register with the value from the memory address
     * 
     * @param pAddress 
     */
    void _ora( Word pAddress );

    /**
     * @brief Eor the A Register with the value from the memory address
     * 
     * @param pAddress 
     */
    void _eor( Word pAddress );

    /**
     * @brief Conditional branch
     * 
     * @param pTest 
     * @param pExpected 
     */
    void _branchIf( bool pTest, bool pExpected );

    /**
     * @brief Do add with carry given the the operand
     * 
     * @param pOperand 
     */
    void _ADC( Byte pOperand );
    
    /**
     * @brief Do subtract with carry given the the operand
     * 
     * @param pOperand 
     */
    void _SBC( Byte pOperand );

    /**
     * @brief Sets the processor status for a CMP/CPX/CPY instruction
     * 
     * @param pOperand 
     * @param pRegisterValue 
     */
    void _registerCompare( Byte pOperand, Byte pRegisterValue );

    /**
     * @brief Arithmetic shift left
     * 
     * @param Operand 
     * @return Byte 
     */
    Byte _ASL( Byte Operand );

    /**
     * @brief Logical shift right
     * 
     * @param Operand 
     * @return Byte 
     */
    Byte _LSR( Byte Operand );

    /**
     * @brief Rotate left
     * 
     * @param Operand 
     * @return Byte 
     */
    Byte _ROL( Byte Operand );

    /**
     * @brief Rotate right
     * 
     * @param Operand 
     * @return Byte 
     */
    Byte _ROR( Byte Operand );

    /**
     * @brief Push Processor status onto the stack
     *        Setting bits 4 & 5 on the stack
     * 
     */
    void _pushPSToStack();

    /**
     * @brief Pop Processor status from the stack
     *        Clearing bits 4 & 5 (Break & Unused)
     * 
     */
    void _popPSFromStack();
};

}

#endif