// memory.cpp
#include <cstring>
#include <stdexcept>

#include "memory.h"

Memory::Memory() : m_AvailableMemory(4086) {
	m_Memory = new uint8_t[m_AvailableMemory];
}

Memory::~Memory()
{
	delete[] m_Memory;
}

void Memory::LoadProgram(const uint8_t* program, size_t programSize)
{
	if (static_cast<uint16_t>(programSize) > (m_AvailableMemory - 0x200))
		throw std::runtime_error("[-] Program exceeds available memory!");

	std::memcpy(m_Memory + 0x200, program, programSize);
}

void Memory::LoadFont(const uint8_t* fontData, size_t fontDataSize)
{
	std::memcpy(m_Memory, fontData, fontDataSize);
}

uint16_t Memory::FetchInstruction(uint16_t offset) const
{
	return (m_Memory[offset] << 8) | m_Memory[offset + 1];
}

uint8_t Memory::Read(uint16_t offset) const
{
	return m_Memory[offset];
}