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

#pragma once
#include <m6502/Config.hpp>

namespace m6502
{

class Mem
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

}