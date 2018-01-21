using System;
using System.Collections.Generic;
using System.Text;

namespace UniqueEngine
{

    /// Primitive type.
    public enum PrimitiveType
    {
        TRIANGLE_LIST = 0,
        LINE_LIST,
        POINT_LIST,
        TRIANGLE_STRIP,
        LINE_STRIP,
        TRIANGLE_FAN
    }

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

    /// Blending mode.
    public enum BlendMode
    {
        REPLACE = 0,
        ADD,
        MULTIPLY,
        ALPHA,
        ADDALPHA,
        PREMULALPHA,
        INVDESTALPHA,
        SUBTRACT,
        SUBTRACTALPHA,
        MAX_BLENDMODES
    }

    /// Depth or stencil compare mode.
    public enum CompareMode
    {
        ALWAYS = 0,
        EQUAL,
        NOTEQUAL,
        LESS,
        LESSEQUAL,
        GREATER,
        GREATEREQUAL,
        MAX_COMPAREMODES
    }

    /// Culling mode.
    public enum CullMode
    {
        NONE = 0,
        CW,
        CCW,
        MAX_CULLMODES
    }

    /// Fill mode.
    public enum FillMode
    {
        SOLID = 0,
        WIREFRAME,
        POINT
    }

    /// Stencil operation.
    public enum StencilOp
    {
        KEEP = 0,
        ZERO,
        REF,
        INCR,
        DECR
    }

    public enum BufferUsage
    {
        Static,
        Dynamic,
        Transist,
    }

}
