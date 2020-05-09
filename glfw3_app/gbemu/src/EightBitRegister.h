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

#ifndef EIGHTBITREGISTER_H
#define	EIGHTBITREGISTER_H

#include "definitions.h"

class EightBitRegister
{
public:
    EightBitRegister() : m_Value(0) { }
    void SetValue(u8 value);
    u8 GetValue() const;
    void Increment();
    void Decrement();

private:
    u8 m_Value;
};


inline void EightBitRegister::SetValue(u8 value)
{
    this->m_Value = value;
}

inline u8 EightBitRegister::GetValue() const
{
    return m_Value;
}

inline void EightBitRegister::Increment()
{
    m_Value++;
}

inline void EightBitRegister::Decrement()
{
    m_Value--;
}

#endif	/* EIGHTBITREGISTER_H */

