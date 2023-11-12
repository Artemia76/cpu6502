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

#include <m6502/System/Registers.hpp>

namespace m6502
{

/*****************************************************************************/

Registers::Registers ()
{
    PC = 0xFFFC;
    SP = 0xFF;
    Flags.C = Flags.Z = Flags.I = Flags.D = Flags.B = Flags.V = Flags.N = 0;
    A = X = Y = 0;
}

/*****************************************************************************/

Registers::Registers (const Registers& copy)
{
    PC = copy.PC;
    SP = copy.SP;
    PS = copy.PS;
    A = copy.A;
    X = copy.X;
    Y = copy.Y;
}

/*****************************************************************************/

Registers::~Registers () {}

}