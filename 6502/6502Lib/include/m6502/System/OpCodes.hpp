/**
 * @file OpCodes.hpp
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

#include <map>
#include <m6502/Config.hpp>
#include <map>

/**
 * @brief Return ByteCode from OpCode Instruction
 * 
 */
#define opcode(i) static_cast<Byte>(i)

/**
 * @brief Return OpCode from ByteCode Instruction
 * 
 */
#define ins(o) static_cast<Ins>(o)

namespace m6502
{

 // opcodes
enum class Ins : Byte
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
    //Stack
	TSX = 0xBA,
	TXS = 0x9A,
	PHA = 0x48,
	PLA = 0x68,
	PHP = 0x08,
	PLP = 0x28,
    //Jumps and calls
	JMP_ABS = 0x4C,
	JMP_IND = 0x6C,
	JSR = 0x20,
	RTS = 0x60,
	//Logical Ops
	
	//AND
	AND_IM = 0x29,
	AND_ZP = 0x25,
	AND_ZPX = 0x35,
	AND_ABS = 0x2D,
	AND_ABSX = 0x3D,
	AND_ABSY = 0x39,
	AND_INDX = 0x21,
	AND_INDY = 0x31,

	//OR
	ORA_IM = 0x09,
	ORA_ZP = 0x05,
	ORA_ZPX = 0x15,
	ORA_ABS = 0x0D,
	ORA_ABSX = 0x1D,
	ORA_ABSY = 0x19,
	ORA_INDX = 0x01,
	ORA_INDY = 0x11,

	//EOR
	EOR_IM = 0x49,
	EOR_ZP  = 0x45,
	EOR_ZPX = 0x55,
	EOR_ABS = 0x4D,
	EOR_ABSX = 0x5D,
	EOR_ABSY = 0x59,
	EOR_INDX = 0x41,
	EOR_INDY = 0x51,

	//BIT
	BIT_ZP = 0x24,
	BIT_ABS = 0x2C,

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

}