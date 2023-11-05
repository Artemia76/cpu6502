/**
 * @file Mem.hpp
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

#include <m6502/System/Mem.hpp>

namespace m6502
{

Mem::Mem ()
{
    Initialise();
}

Mem::~Mem()
{
}

/**
 * @brief Read 1 Byte from memory
 * 
 * @param Address 
 * @return Byte 
 */
Byte Mem::operator[]( u32 Address) const
{
    // assert here Address is < MAX_MEM
    return Data[Address];
}

/**
 * @brief Write 1 Byte to memory
 * 
 * @param Address 
 * @return Byte& 
 */
Byte& Mem::operator[]( u32 Address)
{
    // assert here Address is < MAX_MEM
    return Data[Address];
}

}