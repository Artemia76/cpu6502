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

/**
 * @brief This abstract class enable a chip to connect on data bus
 * 
 */
class CBusChip
{
    /**
     * @brief CBus is the controller class
     * 
     */
    friend class CBus;

    protected:
        /**
         * @brief Construct a new CBusChip object
         * 
         * @param pBus 
         * @param pMask 
         * @param pBank 
         */
        CBusChip(CBus& pBus, const Word& pMask, const Word& pBank);

        /**
         * @brief Construct a new CBusChip object
         * 
         * @param pCopy 
         */
        CBusChip(const CBusChip& pCopy);

        /**
         * @brief Destroy the CBusChip object
         * 
         */
        ~CBusChip();

        /**
         * @brief Write Event from Bus
         * 
         */
        virtual void OnWriteBusData(const Word&, const Byte&){};

        /**
         * @brief Read Event from Bus
         * 
         * @return Byte 
         */
        virtual Byte OnReadBusData(const Word&){return 0;};

        /**
         * @brief Bus Parent
         * 
         */
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
 * @brief Link chips on a data bus
 *        Role is to avoid collisions on R/W  flows 
 *        Between Chips on emulated hardware in
 *        Case of multithreaded use
 */
class CBus
{
    friend class CBusChip;
    public:

        /**
         * @brief Construct a new CBus object
         * 
         */
        CBus(){};

        /**
         * @brief Destroy the CBus object
         * 
         */
        ~CBus(){};

        /**
         * @brief Send Reset Signal to all chips
         * 
         */
        void Reset();

        /**
         * @brief Set the Ready flag
         * 
         * @param pFlag 
         */
        void SetReady(const bool pFlag);

        /**
         * @brief Send data on bus
         * 
         * @param pAddress 
         * @param pData 
         */
        void WriteBusData(const Word& pAddress, const Byte& pData);

        /**
         * @brief Read data from bus
         * 
         * @param pAddress 
         * @return Byte 
         */
        Byte ReadBusData(const Word& pAddress);

    private:
        /**
         * @brief Vector contain list of chips connected on bus
         * 
         */
        v_buschips m_chips;

        /**
         * @brief Called by Chips to connect on bus events
         * 
         * @param pChip 
         */
        void Subscribe( CBusChip* pChip);

        /**
         * @brief Called by Chips to disconnect from bus events
         * 
         * @param pChip 
         */
        void UnSubscribe( CBusChip* pChip);

};

}

#endif