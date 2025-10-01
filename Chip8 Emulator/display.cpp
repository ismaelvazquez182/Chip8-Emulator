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
    bindKeys();
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
        else if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
            for (auto& k : m_Keys) {
                if (event.key.scancode == k.scancode)
                    k.isPressed = (event.type == SDL_EVENT_KEY_DOWN);
            }
        }
    }

    drawPixels();

    ::SDL_RenderPresent(m_Renderer);
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

            ::SDL_SetRenderDrawColor(m_Renderer,
                m_Pixels[i][j] ? 255 : 0,
                m_Pixels[i][j] ? 255 : 0,
                m_Pixels[i][j] ? 255 : 0,
                255);

            ::SDL_RenderFillRect(m_Renderer, &rect);
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

void Display::bindKeys()
{
    m_Keys[0] =  { 0,  SDL_SCANCODE_X, false };
    m_Keys[1] =  { 1,  SDL_SCANCODE_1, false };
    m_Keys[2] =  { 2,  SDL_SCANCODE_2, false };
    m_Keys[3] =  { 3,  SDL_SCANCODE_3, false };
    m_Keys[4] =  { 4,  SDL_SCANCODE_Q, false };
    m_Keys[5] =  { 5,  SDL_SCANCODE_W, false };
    m_Keys[6] =  { 6,  SDL_SCANCODE_E, false };
    m_Keys[7] =  { 7,  SDL_SCANCODE_A, false };
    m_Keys[8] =  { 8,  SDL_SCANCODE_S, false };
    m_Keys[9] =  { 9,  SDL_SCANCODE_D, false };
    m_Keys[10] = { 10, SDL_SCANCODE_Z, false }; // A
    m_Keys[11] = { 11, SDL_SCANCODE_C, false }; // B
    m_Keys[12] = { 12, SDL_SCANCODE_4, false }; // C
    m_Keys[13] = { 13, SDL_SCANCODE_R, false }; // D
    m_Keys[14] = { 14, SDL_SCANCODE_F, false }; // E
    m_Keys[15] = { 15, SDL_SCANCODE_V, false }; // F

}

bool Display::GetPixel(uint8_t row, uint8_t col)
{
    return m_Pixels[row][col];
}

void Display::FlipPixel(uint8_t row, uint8_t col)
{
    m_Pixels[row][col] ^= true;
}

bool Display::IsKeyCurrentlyPressed(uint8_t key)
{
    return m_Keys[key].isPressed;
}