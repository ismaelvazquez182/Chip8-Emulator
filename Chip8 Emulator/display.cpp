// display.cpp
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdexcept>

#include "display.h"

Display::Display(int width, int height): m_IsDone(false), m_Scale (10), m_Width(width), m_Height(height)
{
    ::SDL_Init(SDL_INIT_VIDEO);
    createWindow();
    createRenderer();
    allocatePixelMemory();
    ClearScreen();
}

void Display::createWindow()
{
    m_Window = SDL_CreateWindow("Chip 8 Emulator", m_Width * m_Scale, m_Height * m_Scale, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (m_Window == NULL) {
        ::SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        throw std::runtime_error("[-] Could not create window.");
    }
}

void Display::createRenderer()
{
    m_Renderer = SDL_CreateRenderer(m_Window, NULL);
    if (m_Renderer == NULL) {
        ::SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create renderer: %s\n", SDL_GetError());
        throw std::runtime_error("[-] Could not create renderer.");
    }
}

void Display::allocatePixelMemory()
{
    m_Pixels = new bool* [m_Height];
    for (int i = 0; i < m_Height; i++) {
        m_Pixels[i] = new bool[m_Width];
    }
}

Display::~Display()
{
    for (int i = 0; i < m_Height; i++) {
        delete[] m_Pixels[i];
    }
    delete[] m_Pixels;

    ::SDL_DestroyWindow(m_Window);
    ::SDL_Quit();
}

bool Display::ShouldClose() const { return m_IsDone; }

void Display::Render()
{
    SDL_Event event;

    while (::SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            m_IsDone = true;
        }
    }

    drawPixels();

    SDL_RenderPresent(m_Renderer);
}

void Display::drawPixels()
{
    for (int i = 0; i < m_Height; i++) {
        for (int j = 0; j < m_Width; j++) {
            SDL_FRect rect{};
            rect.x = static_cast<float>(j * m_Scale);
            rect.y = static_cast<float>(i * m_Scale);
            rect.w = static_cast<float>(m_Scale);
            rect.h = static_cast<float>(m_Scale);

            SDL_SetRenderDrawColor(m_Renderer,
                m_Pixels[i][j] ? 255 : 0,
                m_Pixels[i][j] ? 255 : 0,
                m_Pixels[i][j] ? 255 : 0,
                255);

            SDL_RenderFillRect(m_Renderer, &rect);
        }
    }
}

void Display::ClearScreen()
{
    for (int i = 0; i < m_Height; i++) {
        for (int j = 0; j < m_Width; j++) {
            m_Pixels[i][j] = false;
        }
    }
}

bool Display::GetPixel(uint8_t row, uint8_t col)
{
    return m_Pixels[row][col];
}

void Display::FlipPixel(uint8_t row, uint8_t col)
{
    m_Pixels[row][col] ^= true;
}