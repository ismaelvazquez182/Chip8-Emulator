#pragma once

#include <array>
#include <cstdint>

struct SDL_Renderer;
struct SDL_Window;
enum SDL_Scancode;

struct KeyManager {
	uint8_t chip8Key;
	SDL_Scancode scancode;
	bool isPressed;
};

class Display {
public:
	Display(int width, int height);
	~Display();

	bool IsKeyCurrentlyPressed(uint8_t key);
	void ClearScreen();
	void FlipPixel(uint8_t row, uint8_t col);
	bool GetPixel(uint8_t row, uint8_t col);
	bool ShouldClose() const;
	void Render();

private:
	void allocatePixelMemory();
	void bindKeys();
	void createRenderer();
	void createWindow();
	void drawPixels();

private:
	int                               m_Height;
	bool                              m_IsDone;
	std::array<KeyManager, 16>        m_Keys{};
	bool**                            m_Pixels;
	SDL_Renderer*                     m_Renderer;
	int                               m_Scale;
	int                               m_Width;
	SDL_Window*                       m_Window;
};