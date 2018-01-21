#include "UniquePCH.h"
#include "Application/Engine.h"

using namespace Unique;

uExport(Engine, void, SetMinFps, int, fps)
uExport(Engine, void, SetMaxFps, int, fps)

uExport(Engine, float, GetNextTimeStep)
uExport(Engine, int, GetMinFps)
uExport(Engine, int, GetMaxFps)