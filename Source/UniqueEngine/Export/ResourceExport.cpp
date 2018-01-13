#include "UniquePCH.h"

using namespace Unique;

uExportNew(ResourceRef, const StringID&, type, const String&, name)
uExportDelete(ResourceRef)

uExportNew(ResourceRefList, const StringID&, type, const char**, name, int, count)
uExportDelete(ResourceRefList)
