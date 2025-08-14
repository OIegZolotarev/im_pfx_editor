/*
    LightBaker3000 Frontend project,
    (c) 2023 CrazyRussian
*/

#pragma once

typedef struct  
{
    byte r, g, b;
}color24_t;


typedef struct
{
    byte r, g, b, a;
} color32_t;


typedef glm::vec4 ColorRGBA;

int ToColorU32(const ColorRGBA &col);

ColorRGBA PointerHash(void *ptr);
