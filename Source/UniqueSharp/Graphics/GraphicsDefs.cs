using System;
using System.Collections.Generic;
using System.Text;

namespace UniqueEngine
{
    
    /// %Geometry type for vertex shader geometry variations.
    public enum GeometryType
    {
        STATIC = 0,
        SKINNED = 1,
        INSTANCED = 2,
        BILLBOARD = 3,
        DIRBILLBOARD = 4,
        TRAIL_FACE_CAMERA = 5,
        TRAIL_BONE = 6,
        MAX_GEOMETRYTYPES = 7,
        // This is not a real geometry type for VS, but used to mark objects that do not desire to be instanced
        STATIC_NOINSTANCING = 7,
    }
    

}
