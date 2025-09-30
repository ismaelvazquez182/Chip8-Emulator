#pragma once

#include <array>
#include <cstdint>

class Memory {
public:
	Memory() = default;

	uint16_t FetchInstruction(uint16_t offset) const;
	void     LoadFontIntoMemory(const uint8_t* fontData, size_t fontDataSize);
	void     LoadProgram(const uint8_t* program, size_t programSize);
	uint8_t  Read(uint16_t offset) const;

private:
	static constexpr uint16_t             AVAILABLE_MEMORY{ 4096 };
	std::array<uint8_t, AVAILABLE_MEMORY> m_Memory{};
};
