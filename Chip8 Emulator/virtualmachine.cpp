// virtualmachine.cpp

#include <Windows.h>

#include "display.h"
#include "memory.h"
#include "virtualmachine.h"

VirtualMachine::VirtualMachine()
{
	m_Monitor = new Display(64, 32);
	m_Ram = new Memory();

	uint8_t font[]{ 0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
					0x20, 0x60, 0x20, 0x20, 0x70, // 1
					0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
					0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
					0x90, 0x90, 0xF0, 0x10, 0x10, // 4
					0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
					0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
					0xF0, 0x10, 0x20, 0x40, 0x40, // 7
					0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
					0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
					0xF0, 0x90, 0xF0, 0x90, 0x90, // A
					0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
					0xF0, 0x80, 0x80, 0x80, 0xF0, // C
					0xE0, 0x90, 0x90, 0x90, 0xE0, // D
					0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
					0xF0, 0x80, 0xF0, 0x80, 0x80 }; // F
	m_Ram->LoadFont(font, sizeof(font));
	m_ProgramCounterRegister = 0x200;
}

VirtualMachine::~VirtualMachine()
{
	delete m_Monitor;
	delete m_Ram;
}

void VirtualMachine::Execute(const uint8_t* program, size_t programSize)
{
	m_ProgramSize = programSize;
	m_Ram->LoadProgram(program, programSize);

	while(!m_Monitor->IsDone()) {
		m_CurrentInstruction = m_Ram->FetchInstruction(m_ProgramCounterRegister);
		incrementProgramCounter();
		executeInstruction();
		m_Monitor->Render();
		Sleep(100);
	}
}

void VirtualMachine::incrementProgramCounter() { m_ProgramCounterRegister += 2; }

void VirtualMachine::executeInstruction()
{
	uint8_t instructionFamily = m_CurrentInstruction >> 12;

	switch (instructionFamily) {
	case 0x0:
		if (m_CurrentInstruction == 0x00E0)
			m_Monitor->ClearScreen();
		break;
	case 0x1:
		m_ProgramCounterRegister = m_CurrentInstruction & 0xFFF;
		break;
	case 0x6:
		setRegister();
		break;
	case 7:
		addValue();
		break;
	case 10:
		m_IndexPointerRegister = m_CurrentInstruction & 0xFFF;
		break;
	case 13:
		drawSprite();
		break;
	}
}

void VirtualMachine::drawSprite()
{
	uint8_t x = (m_CurrentInstruction & 0x0F00) >> 8;
	uint8_t y = (m_CurrentInstruction & 0x00F0) >> 4;
	uint8_t n = m_CurrentInstruction & 0xF;

	uint8_t xPos = m_V[x] % 64; // wrap horizontally
	uint8_t yPos = m_V[y] % 32; // wrap vertically
	m_V[0x0F] = 0;

	for (int row = 0; row < n; row++) {
		uint8_t spriteByte = m_Ram->Read(m_IndexPointerRegister + row);

		for (int col = 0; col < 8; col++) {
			if (spriteByte & (0x80 >> col)) {
				uint8_t pixelX = (xPos + col) % 64;
				uint8_t pixelY = (yPos + row) % 32;

				if (m_Monitor->GetPixel(pixelY, pixelX))
					m_V[0x0F] = 1;

				m_Monitor->FlipPixel(pixelY, pixelX);
			}
		}
	}
}

void VirtualMachine::setRegister()
{
	uint8_t secondByte = m_CurrentInstruction & 0xFF;
	m_V[m_CurrentInstruction & 0xF00] = secondByte;
}

void VirtualMachine::addValue()
{
	uint8_t secondByte = m_CurrentInstruction & 0xFF;
	m_V[m_CurrentInstruction & 0xF00] += secondByte;
}