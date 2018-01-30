using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace UniqueEngine
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Color
    {
        public float r, g, b, a;

        public Color(byte r, byte g, byte b, byte a = 255)
        {
            this.r = MathHelper.Clamp(r / 255.0f, 0, 1.0f);
            this.g = MathHelper.Clamp(g / 255.0f, 0, 1.0f);
            this.b = MathHelper.Clamp(b / 255.0f, 0, 1.0f);
            this.a = MathHelper.Clamp(a / 255.0f, 0, 1.0f);
        }

        public Color(float r, float g, float b, float a = 1.0f)
        {
            this.r = r;
            this.g = g;
            this.b = b;
            this.a = a;
        }
    }
}
