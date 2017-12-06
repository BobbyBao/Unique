
#pragma once

#include "../Core/Object.h"

namespace Unique
{

/// Core init.
uEvent(Startup)
{
};

/// Core shutdown.
uEvent(Shutdown)
{
};

/// Frame begin event.
uEvent(BeginFrame)
{
    unsigned frameNumber_;
    float timeStep_;
};

/// Application-wide logic update event.
uEvent(Update)
{
    float timeStep_;
};

/// Application-wide logic post-update event.
uEvent(PostUpdate)
{
    float timeStep_;
};

/// Render update event.
uEvent(RenderUpdate)
{
    float timeStep_;
};

/// Post-render update event.
uEvent(PostRenderUpdate)
{
    float timeStep_;
};

/// GUI render event.
uEvent(GUI)
{
};

/// Frame end event.
uEvent(EndFrame)
{
};

}
