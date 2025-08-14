/*
    LightBaker3000 Frontend project,
    (c) 2023 CrazyRussian
*/

#include "application.h"
#include "common.h"
#include "gl_texture.h"


// The format of the files is as follows:
//
// dsprite_t file header structure
// <repeat dsprite_t.numframes times>
//   <if spritegroup, repeat dspritegroup_t.numframes times>
//     dspriteframe_t frame header structure
//     sprite bitmap
//   <else (single sprite frame)>
//     dspriteframe_t frame header structure
//     sprite bitmap
// <endrepeat>


rawimage_t* LoadNormal(dspriteframe_t *frameDesc, byte *pixels, color24_t *pallete)
{
    rawimage_t *pFrame = new rawimage_t(frameDesc->width, frameDesc->height, 3);

    size_t     numPixels    = frameDesc->width * frameDesc->height;
    color24_t *outRGBPixels = (color24_t *)pFrame->data;

    for (int j = frameDesc->height - 1; j >= 0; j--)
    {
        byte *row = &pixels[j * frameDesc->width];

        for (int i = 0; i < frameDesc->width; i++)
        {
            const color24_t &palleteColor = pallete[row[i]];
            *outRGBPixels                 = palleteColor;
            outRGBPixels++;
        }
    }

    return pFrame;
}

rawimage_t *LoadIndexAlpha(dspriteframe_t *frameDesc, byte *pixels, color24_t *pallete)
{
    rawimage_t *pFrame = new rawimage_t(frameDesc->width, frameDesc->height, 4);

    size_t     numPixels    = frameDesc->width * frameDesc->height;
    color32_t *outRGBPixels = (color32_t *)pFrame->data;

    for (int j = frameDesc->height - 1; j >= 0; j--)
    {
        byte *row = &pixels[j * frameDesc->width];

        for (int i = 0; i < frameDesc->width; i++)
        {
            const color24_t &palleteColor = pallete[row[i]];
            outRGBPixels->r                 = palleteColor.r;
            outRGBPixels->g                 = palleteColor.g;
            outRGBPixels->b                 = palleteColor.b;
         
            if (row[i] == 255)
                outRGBPixels->a = 0;
            else
                outRGBPixels->a = 255;

            outRGBPixels++;
        }
    }

    return pFrame;
}

RawTexture *DecodeGoldSourceSpite(byte *data, size_t length)
{
    dsprite_t *hdr = (dsprite_t *)data;

    if (hdr->ident != IDSPRITEHEADER)
        return nullptr;

    RawTexture *pResult = new RawTexture;

    size_t numFrames = hdr->numframes;
    byte * ptr       = data;

    ptr += sizeof(dsprite_t);

    // Pallete size and data
    short palleteSize = *(short *)ptr;
    palleteSize *= 3;

    ptr += sizeof(short);

    color24_t *pallete = (color24_t *)ptr;
    ptr += palleteSize;

    for (size_t i = 0; i < numFrames; i++)
    {
        spriteframetype_t type = *(spriteframetype_t *)ptr;
        ptr += sizeof(spriteframetype_t);

        if (type == SPR_SINGLE)
        {
            dspriteframe_t *frameDesc = (dspriteframe_t *)ptr;
            ptr += sizeof(dspriteframe_t);

            byte *pixels = ptr;
            ptr += frameDesc->width * frameDesc->height;

            rawimage_t *pFrame = nullptr;

            if (hdr->type == SPR_INDEXALPHA) 
                pFrame = LoadIndexAlpha(frameDesc, pixels, pallete);
            else
                pFrame = LoadNormal(frameDesc, pixels, pallete);


            pResult->AddRawFrame(pFrame);
        }
        else if (type == SPR_GROUP)
        {
        }
        else
        {
            // ((char *)"Bad sprite frame type %d !\n", type);
            delete pResult;
            return nullptr;
        }
    }

    return pResult;
}
