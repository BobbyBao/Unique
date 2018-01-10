#include "UniquePCH.h"
#include "Input/Input.h"

using namespace Unique;

uExport(Input, bool, GetKeyDown, int, key)
uExport(Input, bool, GetKeyPress, int, key)

uExport(Input, bool, GetMouseButtonDown, int, button)
uExport(Input, bool, GetMouseButtonPress, int, button)

uExport(Input, IntVector2, GetMousePosition)
uExport(Input, IntVector2, GetMouseMove)
uExport(Input, int, GetMouseMoveX)
uExport(Input, int, GetMouseMoveY)
uExport(Input, int, GetMouseMoveWheel)