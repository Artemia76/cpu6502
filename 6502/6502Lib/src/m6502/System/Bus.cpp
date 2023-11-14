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

CBusChip::CBusChip (CBus& pBus, const Word& pMask, const Word& pBank) : Bus(pBus), Mask(pMask), Bank(pBank)
{
    Bus.Subscribe(this);
}

/*****************************************************************************/

CBusChip::CBusChip (const CBusChip& pCopy) : Bus(pCopy.Bus), Mask(pCopy.Mask), Bank(pCopy.Mask)
{
    Bus.Subscribe(this);
}

/*****************************************************************************/

CBusChip::~CBusChip()
{
    Bus.UnSubscribe(this);
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

void CBus::SetReady(const bool)
{

}

/*****************************************************************************/

void CBus::WriteBusData(const Word& pAddress, const Byte& pData)
{
    for (auto Chip : m_chips)
    {
        // If chip on bus is a master (e.g. CPU, etc) we pass
        if (Chip->Mask == 0xFFFF) continue;
        // If chip is on range of address
        if ((Chip->Mask & pAddress)==Chip->Bank)
        {
            Chip->OnWriteBusData(pAddress-Chip->Bank,pData);
        }
    }
}

/*****************************************************************************/

Byte CBus::ReadBusData(const Word& pAddress)
{
    for (auto Chip : m_chips)
    {
        // If chip on bus is a master (e.g. CPU, etc) we pass
        if (Chip->Mask == 0xFFFF) continue;
        // If chip is on range of address
        
        if ((Chip->Mask & pAddress)==Chip->Bank)
        {
            return Chip->OnReadBusData(pAddress-Chip->Bank);
        }
    }
    return 0;
}


/*****************************************************************************/

void CBus::Subscribe( CBusChip* pChip)
{
    if (std::find(m_chips.begin(), m_chips.end(),pChip) == m_chips.end())
    {
        m_chips.push_back(pChip);
    }
}

/*****************************************************************************/

void CBus::UnSubscribe( CBusChip* pChip)
{
    m_chips.erase(std::remove(m_chips.begin(), m_chips.end(), pChip), m_chips.end());
}

}