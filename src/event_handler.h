/*
	LightBaker3000 Frontend project,
	(c) 2022 CrazyRussian
*/

#pragma once

#include "SDL2/SDL.h"

#define EVENT_HANDLED (1<<0) // Event was handled but should continue propagation
#define EVENT_CONSUMED (1<<1) // Event was handled and propagation must be stopped

#define EVENT_FINISHED (EVENT_CONSUMED | EVENT_HANDLED)

#define PRIORITY_VIEWPORT 0
#define PRIORITY_EDITING_TOOLBOX 1

class IEventHandler
{
public:
	virtual int HandleEvent(bool bWasHandled, const SDL_Event &e, const float flFrameDelta) = 0;

    virtual int Priority()
    {
        // Bigger value = more important
        return 0;
    }
};