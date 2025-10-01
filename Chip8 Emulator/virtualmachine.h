#pragma once

#include "display.h"
#include "memory.h"

#include <memory>
#include <vector>

class VirtualMachine {
public:
	VirtualMachine();

	void Execute(const uint8_t* program, size_t programSize);

private:
	void addValue(uint8_t x, uint8_t secondByte);
	void drawSprite(uint8_t x, uint8_t y, uint8_t n);
	void executeInstruction();
	void handle8xyOperations(uint8_t x, uint8_t y, uint8_t n);
	void handleFxyOperations(uint8_t x, uint8_t secondByte);
	void incrementProgramCounter();
	void setRegister(uint8_t secondByte);
	void setRegisterToMemoryLocation(uint16_t &_register, uint8_t offset = 0);
	
private:
	bool                     m_IsBlock{ false };
	uint16_t                 m_CurrentInstruction{ 0 };
	uint8_t                  m_DelayTimer{ 0 };
	uint16_t                 m_IndexPointerRegister{ 0 };
	uint16_t                 m_ProgramCounterRegister{ 0 }; // instruction pointer
	std::unique_ptr<Display> m_Screen{ nullptr };
	std::unique_ptr<Memory>  m_Ram{ nullptr };
	uint8_t                  m_SoundTimer{ 0 };
	std::vector<uint16_t>    m_Stack{ 0 };
	uint8_t                  m_V[16]{ 0 }; // general purpose registers
};