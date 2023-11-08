/**
 * @file Config.hpp
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

#include <cstdint>

namespace m6502
{
using SByte = std::int8_t;
using Byte = std::uint8_t;
using SWord = std::int16_t;
using Word = std::uint16_t;

using u32 = std::uint32_t;
using s32 = std::int32_t;

using u64 = std::uint64_t;
using s64 = std::int64_t;

static constexpr u32 MAX_MEM = 1024*64;
}