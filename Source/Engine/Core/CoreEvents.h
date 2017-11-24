
#pragma once

#include "../Core/Object.h"

namespace Unique
{

/// Core init.
UNIQUE_EVENT(E_INIT, Init)
{
};

/// Core shutdown.
UNIQUE_EVENT(E_SHUTDOWN, Shutdown)
{
};

/// Frame begin event.
UNIQUE_EVENT(E_BEGINFRAME, BeginFrame)
{
    unsigned frameNumber_;
    float timeStep_;
};

/// Application-wide logic update event.
UNIQUE_EVENT(E_UPDATE, Update)
{
    float timeStep_;
};

/// Application-wide logic post-update event.
UNIQUE_EVENT(E_POSTUPDATE, PostUpdate)
{
    float timeStep_;
};

/// Render update event.
UNIQUE_EVENT(E_RENDERUPDATE, RenderUpdate)
{
    float timeStep_;
};

/// Post-render update event.
UNIQUE_EVENT(E_POSTRENDERUPDATE, PostRenderUpdate)
{
    float timeStep_;
};

/// GUI render event.
UNIQUE_EVENT(E_GUI, GUI)
{
};

/// Frame end event.
UNIQUE_EVENT(E_ENDFRAME, EndFrame)
{
};

}
