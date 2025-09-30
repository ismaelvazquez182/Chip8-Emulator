#pragma once
#include <cstdint>

struct SDL_Renderer;
struct SDL_Window;

class Display {
public:
	Display(int width, int height);
	~Display();

	void ClearScreen();
	void FlipPixel(uint8_t row, uint8_t col);
	bool GetPixel(uint8_t row, uint8_t col);
	bool IsDone() const;
	void Render();

private:
	void allocateScreenMemory();
	void createRenderer();
	void createWindow();
	void drawScreen();

private:
	int               m_Height;
	bool              m_IsDone;
	bool**            m_Pixels;
	SDL_Renderer*     m_Renderer;
	int               m_Scale;
	int               m_Width;
	SDL_Window*       m_Window;
};