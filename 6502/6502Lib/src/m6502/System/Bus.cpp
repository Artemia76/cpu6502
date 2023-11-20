/**
 * @file Bus.cpp
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

#include <m6502/System/Bus.hpp>

namespace m6502
{

CBusChip::CBusChip (CBus& pBus, const Word& pMask, const Word& pBank) : bus(pBus), mask(pMask), bank(pBank)
{
    bus._subscribe(this);
}

/*****************************************************************************/

CBusChip::CBusChip (const CBusChip& pCopy) : bus(pCopy.bus), mask(pCopy.mask), bank(pCopy.mask)
{
    bus._subscribe(this);
}

/*****************************************************************************/

CBusChip::~CBusChip()
{
    bus._unSubscribe(this);
}

/*****************************************************************************/

/*void CBusChip::SetReady(bool pFlag)
{
    Bus.SetReady(pFlag);
}*/

/*****************************************************************************/

/*void CBusChip::WriteBusData(const Word& pAddress, const Byte& pData)
{
    Bus.WriteBusData(pAddress,pData);
}*/

/*****************************************************************************/

/*Byte CBusChip::ReadBusData(const Word& pAddress)
{
    return Bus.ReadBusData(pAddress);
}*/



/*****************************************************************************/

void CBus::setReady(const bool)
{

}

/*****************************************************************************/

void CBus::writeBusData(const Word& pAddress, const Byte& pData)
{
    for (auto Chip : _chips)
    {
        // If chip on bus is a master (e.g. CPU, etc) we pass
        if (Chip->mask == 0xFFFF) continue;
        // If chip is on range of address
        if ((Chip->mask & pAddress)==Chip->bank)
        {
            Chip->onWriteBusData(pAddress-Chip->bank,pData);
        }
    }
}

/*****************************************************************************/

Byte CBus::readBusData(const Word& pAddress)
{
    for (auto Chip : _chips)
    {
        // If chip on bus is a master (e.g. CPU, etc) we pass
        if (Chip->mask == 0xFFFF) continue;
        // If chip is on range of address
        
        if ((Chip->mask & pAddress)==Chip->bank)
        {
            return Chip->onReadBusData(pAddress-Chip->bank);
        }
    }
    return 0;
}

/*****************************************************************************/

void CBus::_subscribe( CBusChip* pChip)
{
    if (std::find(_chips.begin(), _chips.end(),pChip) == _chips.end())
    {
        _chips.push_back(pChip);
    }
}

/*****************************************************************************/

void CBus::_unSubscribe( CBusChip* pChip)
{
    _chips.erase(std::remove(_chips.begin(), _chips.end(), pChip), _chips.end());
}

}