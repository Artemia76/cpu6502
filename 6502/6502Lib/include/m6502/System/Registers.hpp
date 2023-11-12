/**
 * @file Registers.hpp
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

#ifndef REGISTERS_HPP
#define REGISTERS_HPP

#include <m6502/Config.hpp>

namespace m6502
{

/**
 * @brief Status Flags of 6502 CPU
 * 
 */
struct StatusFlags
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

/**
 * @brief 6502 CPU Registers
 * 
 */
class Registers
{
    public:
        /**
         * @brief Construct a new Registers object
         * 
         * @param copy 
         */
        Registers(const Registers& copy);

        /**
         * @brief Destroy the Registers object
         * 
         */
        virtual ~Registers();

        /**
         * @brief program counter
         * 
         */
        Word PC;
        /**
         * @brief stack pointer
         * 
         */
        Byte SP;

        /**
         * @brief Accumulator Register
         * 
         */
        Byte A;

        /**
         * @brief X Register
         * 
         */
        Byte X;

        /**
         * @brief Y Register
         * 
         */
        Byte Y;
        union
        {
            /**
             * @brief Process Status as Word
             * 
             */
            Byte 		PS;

            /**
             * @brief Process Status as Flag
             *        Struct
             * 
             */
            StatusFlags	Flags;
        };
        
        /**
         * @brief Significant bit
         *        For status register
         * 
         */
    static constexpr Byte
        NegativeFlagBit = 0b10000000,
        OverflowFlagBit = 0b01000000,
        BreakFlagBit = 0b000010000,
        UnusedFlagBit = 0b000100000,
        InterruptDisableFlagBit = 0b000000100,
        ZeroBit = 0b00000001;
    protected:
        /**
         * @brief Construct a new Registers object
         *        from derivated class
         * 
         */
        Registers();
};

}

#endif