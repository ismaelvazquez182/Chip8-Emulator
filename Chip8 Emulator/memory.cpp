// memory.cpp
#include <cstring>
#include <stdexcept>

#include "memory.h"

void Memory::LoadProgram(const uint8_t* program, size_t programSize)
{
	if (static_cast<uint16_t>(programSize) > (AVAILABLE_MEMORY - 0x200))
		throw std::runtime_error("[-] Program exceeds available memory!");

	std::memcpy(m_Memory.data() + 0x200, program, programSize);
}

void Memory::LoadFontIntoMemory(const uint8_t* fontData, size_t fontDataSize)
{
	std::memcpy(m_Memory.data(), fontData, fontDataSize);
}

uint16_t Memory::FetchInstruction(uint16_t offset) const
{
	return (m_Memory[offset] << 8) | m_Memory[offset + 1];
}

uint8_t Memory::Read(uint16_t offset) const
{
	return m_Memory[offset];
}