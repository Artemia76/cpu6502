/**
 * @file Bus.hpp
 * @author Gianni Peschiutta
 * @brief M6502Lib - Motorola 6502 CPU Emulator
 * @version 0.1
 * @date 2023-11-13
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

#ifndef BUS_HPP
#define BUS_HPP

#include <m6502/Config.hpp>
#include <vector>

namespace m6502
{

class CBus;

class CBusChip
{
    friend class CBus;

    protected: 
        CBusChip(CBus& pBus, const Word pMask, const Word pBank);
        CBusChip(const CBusChip& pCopy);

        ~CBusChip();

        void SetReady(const bool pFlag);
        void WriteBusData(const Word pAddress, const Byte pData);
        Byte ReadBusData(const Word pAddress);

        virtual void OnWriteBusData(const Word, const Byte){};
        virtual Byte OnReadBusData(const Word){return 0;};

        CBus& Bus;

        /**
         * @brief Mask used by bus to trigger R/W events
         *        0xFFFF value mean chip act as master (CPU)
         *        and not handle R/W operations
         */
        Word Mask;

        /**
         * @brief Bank of Mask 
         *        Ex : Mask = 0xC000 Bank = 0x4000
         *        Start Addr = 0x4000
         *        End Addr  = 07FFF
         */
        Word Bank;
};

typedef std::vector<CBusChip*> v_buschips;

/**
 * @brief Represent both address and data bus
 *        Role is to avoid collisions on R/W  flows 
 *        Between Chips on emulated hardware in
 *        Case of multithreaded use
 */
class CBus
{
    friend class CBusChip;
    public:
        CBus(){};
        ~CBus(){};

        void Reset();
    
    protected:
        void SetReady(const bool pFlag);
        void WriteBusData(const Word pAddress, const Byte pData);
        Byte ReadBusData(const Word pAddress);



    private:
        v_buschips m_chips;
        void Subscribe( CBusChip* pChip);
        void UnSubscribe( CBusChip* pChip);

};

}

#endif