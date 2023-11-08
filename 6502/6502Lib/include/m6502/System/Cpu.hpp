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

#pragma once
#include <m6502/Config.hpp>
#include <m6502/System/Mem.hpp>
#include <m6502/System/Registers.hpp>
#include <m6502/System/OpCodes.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

namespace m6502
{

/**
 * @brief Registers for 6502 CPU
 * 
 */
class CPU : public Registers
{
    public:

        /**
         * @brief Construct a new CPU object
         * 
         * @param pMem 
         */
        explicit CPU(Mem& memory);

        /**
         * @brief Construct a new CPU object
         * 
         * @param memory 
         */
        explicit CPU(Mem&& memory) = delete;

        /**
         * @brief Construct a new CPU object
         * 
         * @param copy 
         */
        CPU(const CPU& copy);

        /**
         * @brief Destroy the CPU object
         * 
         */
        ~CPU() override;

        /**
         * @brief Reset the CPU object
         * 
         */
        void Reset( );

        /**
         * @brief Reset the CPU with the given start vector
         * 
         * @param ResetVector 
         */
        void Reset( Word ResetVector );

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
        Word LoadPrg( const Byte* Program, u32 NumBytes) const;

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
        s64 Execute( s64 Cycles);

    private:

        /**
         * @brief Memory space for CPU
         * 
         */
        Mem& m_memory;

        /**
         * @brief Cycles counter down for Execution
         *        process
         * 
         */
        s64 m_cycles;

        /**
         * @brief 
         * 
         * @return Byte 
         */
        Byte FetchByte();

        /**
         * @brief 
         * 
         * @return Word 
         */
        Word FetchWord();

        /**
         * @brief 
         * 
         * @return SByte 
         */
        SByte FetchSByte();

        /**
         * @brief 
         * 
         * @param Address 
         * @return Byte 
         */
        Byte ReadByte( Word Address );

        /**
         * @brief Read Word from memory at address
         * 
         * @param Address 
         * @return Word 
         */
        Word ReadWord( Word Address );

        /**
         * @brief Write 1 Byte to memory
         * 
         * @param Value 
         * @param Address 
         */
        void WriteByte( Byte Value, Word Address );

        /**
         * @brief Write 1 Word to memory
         * 
         * @param Value 
         * @param Address 
         */
        void WriteWord(	Word Value, Word Address );

        /**
         * @brief Write 1 Word on to Stack
         * 
         * @param Value 
         */
        void PushWordToStack( Word Value );

        /**
         * @brief Push the PC-1 onto the stack
         * 
         */
        void PushPCMinusOneToStack();

        /**
         * @brief Push the PC+1 onto the stack
         * 
         */
        void PushPCPlusOneToStack();

        /**
         * @brief Push the PC onto the stack
         * 
         */
        void PushPCToStack();

        /**
         * @brief Push a byte on to Stack
         * 
         * @param Value 
         */
        void PushByteOntoStack( Byte Value );

        /**
         * @brief Pop a byte value from the stack
         * 
         * @return Byte 
         */
        Byte PopByteFromStack();

        /**
         * @brief Pop a 16-bit value from the stack
         * 
         * @return Word 
         */
        Word PopWordFromStack();

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
        void SetZeroAndNegativeFlags( Byte Register );

        /**
         * @brief Addressing mode - Zero page
         * 
         * @return Word 
         */
        Word AddrZeroPage();

        /**
         * @brief Addressing mode - Zero page with X offset
         * 
         * @return Word 
         */
        Word AddrZeroPageX();

        /**
         * @brief Addressing mode - Zero page with Y offset
         * 
         * @return Word 
         */
        Word AddrZeroPageY();

        /**
         * @brief Addressing mode - Absolute
         * 
         * @return Word 
         */
        Word AddrAbsolute();

        /**
         * @brief Addressing mode - Absolute with X offset
         * 
         * @return Word
         * 
         * Addressing mode - Absolute with X offset
         * 
         */
        Word AddrAbsoluteX();

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
        Word AddrAbsoluteX_5();

        /**
         * @brief Addressing mode - Absolute with Y offset
         * 
         * @return Word 
         */
        Word AddrAbsoluteY();

        /**
         * @brief Addressing mode - Absolute with Y offset
         * 
         * @return Word 
         * 
         *  - Always takes a cycle for the Y page boundary)
         *	- See "STA Absolute,Y"
         *
         */
        Word AddrAbsoluteY_5();

        /**
         * @brief Addressing mode - Indirect X | Indexed Indirect
         * 
         * @return Word 
         */
        Word AddrIndirectX();

        /**
         * @brief Addressing mode - Indirect Y | Indirect Indexed
         * 
         * @return Word 
         */
        Word AddrIndirectY();

        /** Addressing mode - Indirect X | Indirect Indexed
        *	- Always takes a cycle for the Y page boundary)
        *	- See "STA (Indirect,Y) */

        Word AddrIndirectX_6();

        /**
         * @brief Addressing mode - Indirect Y | Indirect Indexed
         * 
         * @return Word 
         * 
         * - Always takes a cycle for the Y page boundary)
         * - See "STA (Indirect,Y)
         */
        Word AddrIndirectY_6();
};

}