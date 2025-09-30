// display.cpp
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdexcept>

#include "display.h"

Display::Display(int width, int height): m_IsDone(false), m_Scale (10), m_Width(width), m_Height(height)
{
    m_Width *= m_Scale;
    m_Height *= m_Scale;

    ::SDL_Init(SDL_INIT_VIDEO);
    m_Window = SDL_CreateWindow("Chip 8 Emulator", m_Width, m_Height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (m_Window == NULL) {
        ::SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        throw std::runtime_error("[-] Could not create window.");
    }

    m_Renderer = SDL_CreateRenderer(m_Window, NULL);
    if (m_Renderer == NULL) {
        ::SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create renderer: %s\n", SDL_GetError());
        throw std::runtime_error("[-] Could not create renderer.");
    }

    m_Pixels = new bool*[height];
    for (int i = 0; i < height; i++) {
        m_Pixels[i] = new bool[width];
    }

    ClearScreen();
}

Display::~Display()
{
    for (int i = 0; i < m_Height / m_Scale; i++) {
        delete[] m_Pixels[i];
    }
    delete[] m_Pixels;

    ::SDL_DestroyWindow(m_Window);
    ::SDL_Quit();
}

bool Display::IsDone() const { return m_IsDone; }

void Display::Render()
{
    SDL_Event event;

    while (::SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            m_IsDone = true;
        }
    }

    drawScreen();

    SDL_RenderPresent(m_Renderer);
}

void Display::drawScreen()
{
    for (int i = 0; i < m_Height / m_Scale; i++) {
        for (int j = 0; j < m_Width / m_Scale; j++) {
            SDL_FRect rect{};
            if (m_Pixels[i][j] == true) {
                SDL_SetRenderDrawColor(m_Renderer, 255, 255, 255, 255);
                rect.x = static_cast<float>(j * m_Width / (m_Width / m_Scale));
                rect.y = static_cast<float>(i * m_Height / (m_Height / m_Scale));
                rect.w = static_cast<float>(m_Height / (m_Height / m_Scale));
                rect.h = static_cast<float>(m_Height / (m_Height / m_Scale));

                SDL_RenderFillRect(m_Renderer, &rect);
            }
            else {
                SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 255);
                rect.x = static_cast<float>(j * m_Width / (m_Width / m_Scale));
                rect.y = static_cast<float>(i * m_Height / (m_Height / m_Scale));
                rect.w = static_cast<float>(m_Height / (m_Height / m_Scale));
                rect.h = static_cast<float>(m_Height / (m_Height / m_Scale));

                SDL_RenderFillRect(m_Renderer, &rect);
            }
        }
    }
}

void Display::ClearScreen()
{
    for (int i = 0; i < m_Height / m_Scale; i++) {
        for (int j = 0; j < m_Width / m_Scale; j++) {
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