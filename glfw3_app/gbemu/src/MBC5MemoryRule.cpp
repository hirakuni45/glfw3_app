/*
 * Gearboy - Nintendo Game Boy Emulator
 * Copyright (C) 2012  Ignacio Sanchez

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/
 *
 */

#include "MBC5MemoryRule.h"
#include "Video.h"
#include "Memory.h"
#include "Processor.h"
#include "Input.h"
#include "Cartridge.h"

MBC5MemoryRule::MBC5MemoryRule(Processor* pProcessor,
        Memory* pMemory, Video* pVideo, Input* pInput,
        Cartridge* pCartridge, Audio* pAudio) : MemoryRule(pProcessor,
pMemory, pVideo, pInput, pCartridge, pAudio)
{
    m_pRAMBanks = new u8[0x20000];
    Reset(false);
}

MBC5MemoryRule::~MBC5MemoryRule()
{
    SafeDeleteArray(m_pRAMBanks);
}

void MBC5MemoryRule::Reset(bool bCGB)
{
    m_bCGB = bCGB;
    m_iCurrentRAMBank = 0;
    m_iCurrentROMBank = 1;
    m_iCurrentROMBankHi = 0;
    m_bRamEnabled = false;
    for (int i = 0; i < 0x20000; i++)
        m_pRAMBanks[i] = 0xFF;
    m_CurrentROMAddress = 0x4000;
    m_CurrentRAMAddress = 0;
}

u8 MBC5MemoryRule::PerformRead(u16 address)
{
    switch (address & 0xE000)
    {
        case 0x4000:
        case 0x6000:
        {
            u8* pROM = m_pCartridge->GetTheROM();
            return pROM[(address - 0x4000) + m_CurrentROMAddress];
        }
        case 0xA000:
        {
            if (m_bRamEnabled)
            {
                return m_pRAMBanks[(address - 0xA000) + m_CurrentRAMAddress];
            }
            else
            {
                Log("--> ** Attempting to read from disabled ram %X", address);
                return 0xFF;
            }
        }
        default:
        {
            return m_pMemory->Retrieve(address);
        }
    }
}

void MBC5MemoryRule::PerformWrite(u16 address, u8 value)
{
    switch (address & 0xE000)
    {
        case 0x0000:
        {
            if (m_pCartridge->GetRAMSize() > 0)
            {
                bool previous = m_bRamEnabled;
                m_bRamEnabled = ((value & 0x0F) == 0x0A);

                if (IsValidPointer(m_pRamChangedCallback) && previous && !m_bRamEnabled)
                {
                    (*m_pRamChangedCallback)();
                }
            }
            break;
        }
        case 0x2000:
        {
            if (address < 0x3000)
            {
                m_iCurrentROMBank = value | (m_iCurrentROMBankHi << 8);
            }
            else
            {
                m_iCurrentROMBankHi = value & 0x01;
                m_iCurrentROMBank = (m_iCurrentROMBank & 0xFF) | (m_iCurrentROMBankHi << 8);
            }
            m_iCurrentROMBank &= (m_pCartridge->GetROMBankCount() - 1);
            m_CurrentROMAddress = m_iCurrentROMBank * 0x4000;
            break;
        }
        case 0x4000:
        {
            m_iCurrentRAMBank = value & 0x0F;
            m_iCurrentRAMBank &= (m_pCartridge->GetRAMBankCount() - 1);
            m_CurrentRAMAddress = m_iCurrentRAMBank * 0x2000;
            break;
        }
        case 0x6000:
        {
            Log("--> ** Attempting to write on invalid address %X %X", address, value);
            break;
        }
        case 0xA000:
        {
            if (m_bRamEnabled)
            {
                m_pRAMBanks[(address - 0xA000) + m_CurrentRAMAddress] = value;
            }
            else
            {
                Log("--> ** Attempting to write on RAM when ram is disabled %X %X", address, value);
            }
            break;
        }
        default:
        {
            m_pMemory->Load(address, value);
            break;
        }
    }
}

void MBC5MemoryRule::SaveRam(OSTREAM& file)
{
    Log("MBC5MemoryRule save RAM...");
    Log("MBC5MemoryRule saving %d banks...", m_pCartridge->GetRAMBankCount());

    s32 ramSize = m_pCartridge->GetRAMBankCount() * 0x2000;

    for (s32 i = 0; i < ramSize; i++)
    {
        u8 ram_byte = m_pRAMBanks[i];
        file.write(reinterpret_cast<const char*> (&ram_byte), 1);
    }

    Log("MBC5MemoryRule save RAM done");
}

bool MBC5MemoryRule::LoadRam(ISTREAM& file, s32 fileSize)
{
    Log("MBC5MemoryRule load RAM...");
    Log("MBC5MemoryRule loading %d banks...", m_pCartridge->GetRAMBankCount());

    s32 ramSize = m_pCartridge->GetRAMBankCount() * 0x2000;

    if ((fileSize > 0) && (fileSize != ramSize))
    {
        Log("MBC5MemoryRule incorrect size. Expected: %d Found: %d", ramSize, fileSize);
        return false;
    }

    for (s32 i = 0; i < ramSize; i++)
    {
        u8 ram_byte = 0;
        file.read(reinterpret_cast<char*> (&ram_byte), 1);
        m_pRAMBanks[i] = ram_byte;
    }

    Log("MBC5MemoryRule load RAM done");

    return true;
}

size_t MBC5MemoryRule::GetRamSize()
{
    return m_pCartridge->GetRAMBankCount() * 0x2000;
}

u8* MBC5MemoryRule::GetRamBanks()
{
    return m_pRAMBanks;
}

u8* MBC5MemoryRule::GetCurrentRamBank()
{
    return &m_pRAMBanks[m_CurrentRAMAddress];
}

u8* MBC5MemoryRule::GetRomBank0()
{
    return m_pMemory->GetMemoryMap() + 0x0000;
}

u8* MBC5MemoryRule::GetCurrentRomBank1()
{
    u8* pROM = m_pCartridge->GetTheROM();
    return &pROM[m_CurrentROMAddress];
}

void MBC5MemoryRule::SaveState(OSTREAM& stream)
{
///    using namespace std;

    stream.write(reinterpret_cast<const char*> (&m_iCurrentRAMBank), sizeof(m_iCurrentRAMBank));
    stream.write(reinterpret_cast<const char*> (&m_iCurrentROMBank), sizeof(m_iCurrentROMBank));
    stream.write(reinterpret_cast<const char*> (&m_iCurrentROMBankHi), sizeof(m_iCurrentROMBankHi));
    stream.write(reinterpret_cast<const char*> (&m_bRamEnabled), sizeof(m_bRamEnabled));
    stream.write(reinterpret_cast<const char*> (m_pRAMBanks), 0x20000);
    stream.write(reinterpret_cast<const char*> (&m_CurrentROMAddress), sizeof(m_CurrentROMAddress));
    stream.write(reinterpret_cast<const char*> (&m_CurrentRAMAddress), sizeof(m_CurrentRAMAddress));
}

void MBC5MemoryRule::LoadState(ISTREAM& stream)
{
///    using namespace std;

    stream.read(reinterpret_cast<char*> (&m_iCurrentRAMBank), sizeof(m_iCurrentRAMBank));
    stream.read(reinterpret_cast<char*> (&m_iCurrentROMBank), sizeof(m_iCurrentROMBank));
    stream.read(reinterpret_cast<char*> (&m_iCurrentROMBankHi), sizeof(m_iCurrentROMBankHi));
    stream.read(reinterpret_cast<char*> (&m_bRamEnabled), sizeof(m_bRamEnabled));
    stream.read(reinterpret_cast<char*> (m_pRAMBanks), 0x20000);
    stream.read(reinterpret_cast<char*> (&m_CurrentROMAddress), sizeof(m_CurrentROMAddress));
    stream.read(reinterpret_cast<char*> (&m_CurrentRAMAddress), sizeof(m_CurrentRAMAddress));
}
