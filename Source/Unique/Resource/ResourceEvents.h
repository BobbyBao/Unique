//
// Copyright (c) 2008-2017 the Unique project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include "../Core/Object.h"

namespace Unique
{

/// Resource reloading started.
uEvent(ReloadStarted)
{
};

/// Resource reloading finished successfully.
uEvent(ReloadFinished)
{
};

/// Resource reloading failed.
uEvent(ReloadFailed)
{
};

/// Tracked file changed in the resource directories.
uEvent(FileChanged)
{
    String fileName_;                   // String
    String resourceName_;           // String
};

/// Resource loading failed.
uEvent(LoadFailed)
{
    String resourceName_;           // String
};

/// Resource not found.
uEvent(ResourceNotFound)
{
    String resourceName_;           // String
};

/// Unknown resource type.
uEvent(UnknownResourceType)
{
    StringID resourceType_;           // StringID
};

/// Resource background loading finished.
uEvent(ResourceBackgroundLoaded)
{
    String resourceName_;           // String
    bool success_;                     // bool
    class Resource* resource_;                   // Resource pointer
};

/// Language changed.
uEvent(ChangeLanguage)
{
};

}
