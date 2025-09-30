#pragma once

#include "display.h"
#include "memory.h"

#include <memory>
#include <vector>

class Display;

class VirtualMachine {
public:
	VirtualMachine();

	void Execute(const uint8_t* program, size_t programSize);

private:
	void addValue();
	void drawSprite();
	void executeInstruction();
	void incrementProgramCounter();
	void jumpToMemoryLocation();
	void setRegister();

private:
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