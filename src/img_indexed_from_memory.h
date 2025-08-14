/*
    LightBaker3000 Frontend project,
    (c) 2023 CrazyRussian
*/


#pragma once

#include "common.h"
#include "color.h"

class RawTexture;

class IndexedFromMemoryImage
{
    byte *m_pPixels;
    color24_t *m_pPallete;

    uint32_t m_uiWidth;
    uint32_t m_uiHeight;
public:
    IndexedFromMemoryImage(uint32_t _w, uint32_t _h, byte *pixelData, byte *palleteData);
    ~IndexedFromMemoryImage();

    RawTexture *MakeRawTexture();

};

