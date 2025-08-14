/*
    LightBaker3000 Frontend project,
    (c) 2023 CrazyRussian
*/

#include "common.h"
#include "mathlib.h"
#include "img_indexed_from_memory.h"
#include "gl_texture.h"

IndexedFromMemoryImage::IndexedFromMemoryImage(uint32_t _w, uint32_t _h, byte *pixelData, byte *palleteData)
{
    m_pPixels  = new byte[_w * _h];
    m_pPallete = new color24_t[256];

    memcpy(m_pPixels, pixelData, _w * _h);
    memcpy(m_pPallete, palleteData, sizeof(color24_t) * 256);

    m_uiWidth  = _w;
    m_uiHeight = _h;
}

IndexedFromMemoryImage::~IndexedFromMemoryImage()
{
    delete m_pPixels;
    delete m_pPallete;
}

RawTexture *IndexedFromMemoryImage::MakeRawTexture()
{
    RawTexture *pResult = new RawTexture();

    rawimage_t *frame = new rawimage_t(m_uiWidth, m_uiHeight, 3);

    size_t nPixels = m_uiWidth * m_uiHeight;

    color24_t *outpixels = (color24_t *)frame->data;

    for (size_t i = 0; i < nPixels; i++)
    {
        byte idx     = m_pPixels[i];
        outpixels[i] = m_pPallete[idx];
    }

    pResult->AddRawFrame(frame);

    return pResult;
}
