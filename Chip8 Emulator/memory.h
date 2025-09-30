#pragma once
#include <cstdint>

class Memory {
public:
	Memory();
	~Memory();

	void LoadProgram(const uint8_t* program, size_t programSize);
	void LoadFont(const uint8_t* fontData, size_t fontDataSize);
	uint16_t FetchInstruction(uint16_t offset) const;
	uint8_t Read(uint16_t offset) const;
private:
	uint16_t m_AvailableMemory;
	uint8_t* m_Memory;
};
