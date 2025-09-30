// virtualmachine.cpp

#include <Windows.h>

#include "virtualmachine.h"

// NOTE: These names are based on the control-flow effect in this implementation,
// not the literal CHIP-8 opcode description. For example, 3xkk ("skip if Vx == kk")
// is named NOT_EQUAL_JUMP_3 here, because if Vx == kk we increment the PC (skip),
// and if not, execution "falls through" to the next instruction (a jump in this design).
// This naming convention matches how instructions are handled in executeInstruction().
enum class InstructionFamily {
	CLEAR_SCREEN_OR_RETURN_0,
	JUMP_1,
	CALL_2,
	NOT_EQUAL_JUMP_3,
	EQUAL_JUMP_4,
	NOT_EQUAL_REGISTER_JUMP_5,
	SET_6,
	ADD_7,
	SET_REGISTER_OR_BIT_MANIPULATION_8,
	EQUAL_REGISTER_JUMP_9,
	SET_INDEX_REGISTER_A,
	JUMP_WITH_OFFSET_B,
	RANDOM_C,
	DISPLAY_D,
	SKIP_IF_KEY_E,
	MISC_F
};

VirtualMachine::VirtualMachine()
{
	m_Screen = std::make_unique<Display>(64, 32);
	m_Ram    = std::make_unique<Memory>();

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
	m_Ram->LoadFontIntoMemory(font, sizeof(font));
	m_ProgramCounterRegister = 0x200;
}

void VirtualMachine::Execute(const uint8_t* program, size_t programSize)
{
	m_Ram->LoadProgram(program, programSize);

	while(!m_Screen->ShouldClose()) {
		m_CurrentInstruction = m_Ram->FetchInstruction(m_ProgramCounterRegister);
		incrementProgramCounter();
		executeInstruction();
		m_Screen->Render();
		Sleep(100);
	}
}

void VirtualMachine::incrementProgramCounter() { m_ProgramCounterRegister += 2; }

void VirtualMachine::executeInstruction()
{
	InstructionFamily instructionFamily = static_cast<InstructionFamily>(m_CurrentInstruction >> 12);
	uint8_t x = (m_CurrentInstruction & 0x0F00) >> 8;
	uint8_t y = (m_CurrentInstruction & 0x00F0) >> 4;

	switch (instructionFamily) {
	case InstructionFamily::CLEAR_SCREEN_OR_RETURN_0:
		if (m_CurrentInstruction == 0x00E0)
			m_Screen->ClearScreen();
		if (m_CurrentInstruction == 0x00EE) {
			m_ProgramCounterRegister = m_Stack.back();
			m_Stack.pop_back();
		}
		break;
	case InstructionFamily::JUMP_1:
		jumpToMemoryLocation();
		break;
	case InstructionFamily::CALL_2:
		m_Stack.emplace_back(m_ProgramCounterRegister);
		jumpToMemoryLocation();
		break;
	case InstructionFamily::NOT_EQUAL_JUMP_3:
		if (m_V[x] == (m_CurrentInstruction & 0xFF))
			incrementProgramCounter();
		break;
	case InstructionFamily::EQUAL_JUMP_4:
		if (m_V[x] != (m_CurrentInstruction & 0xFF))
			incrementProgramCounter();
		break;
	case InstructionFamily::NOT_EQUAL_REGISTER_JUMP_5:
		if (m_V[x] == m_V[y])
			incrementProgramCounter();
		break;
	case InstructionFamily::SET_6:
		setRegister();
		break;
	case InstructionFamily::ADD_7:
		addValue();
		break;
	case InstructionFamily::SET_REGISTER_OR_BIT_MANIPULATION_8:
		break;
	case InstructionFamily::EQUAL_REGISTER_JUMP_9:
		if (m_V[x] != m_V[y])
			incrementProgramCounter();
		break;
	case InstructionFamily::SET_INDEX_REGISTER_A:
		m_IndexPointerRegister = m_CurrentInstruction & 0xFFF;
		break;
	case InstructionFamily::JUMP_WITH_OFFSET_B:
		break;
	case InstructionFamily::RANDOM_C:
		break;
	case InstructionFamily::DISPLAY_D:
		drawSprite();
		break;
	case InstructionFamily::SKIP_IF_KEY_E:
		break;
	case InstructionFamily::MISC_F:
		break;
	}
}

void VirtualMachine::jumpToMemoryLocation()
{
	m_ProgramCounterRegister = m_CurrentInstruction & 0xFFF;
}

void VirtualMachine::setRegister()
{
	uint8_t secondByte = m_CurrentInstruction & 0xFF;
	m_V[(m_CurrentInstruction & 0xF00) >> 8] = secondByte;
}

void VirtualMachine::addValue()
{
	uint8_t secondByte = m_CurrentInstruction & 0xFF;
	m_V[(m_CurrentInstruction & 0xF00) >> 8] += secondByte;
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

				if (m_Screen->GetPixel(pixelY, pixelX))
					m_V[0x0F] = 1;

				m_Screen->FlipPixel(pixelY, pixelX);
			}
		}
	}
}