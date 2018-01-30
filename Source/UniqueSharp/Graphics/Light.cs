using System;
using System.Collections.Generic;
using System.Text;

namespace UniqueEngine
{
    public enum LightType : int
    {
        DIRECTIONAL = 0,
        SPOT,
        POINT
    }

    public partial class Light : Drawable
    {
        public Light()
        {
        }

        public Light SetLightType(LightType type)
        {
            Light_SetLightType(native_, type);
            return this;
        }
                
        public Light Light_SetColor(ref Color color)
        {
            Light_SetColor(native_, ref color);
            return this;
        }

        public Light Light_SetTemperature(float temperature)
        {
            Light_SetTemperature(native_, temperature);
            return this;
        }

        public Light Light_SetRadius(float radius)
        {
            Light_SetRadius(native_, radius);
            return this;
        }

        public Light Light_SetLength(float length)
        {
            Light_SetLength(native_, length);
            return this;
        }

        public Light Light_SetUsePhysicalValues(float enable)
        {
            Light_SetUsePhysicalValues(native_, enable);
            return this;
        }

        public Light Light_SetSpecularIntensity(float intensity)
        {
            Light_SetSpecularIntensity(native_, intensity);
            return this;
        }

        public Light Light_SetBrightness(float brightness)
        {
            Light_SetBrightness(native_, brightness);
            return this;
        }

        public Light Light_SetRange(float range)
        {
            Light_SetRange(native_, range);
            return this;
        }

        public Light Light_SetFov(float fov)
        {
            Light_SetFov(native_, fov);
            return this;
        }
        
        public Light Light_SetAspectRatio(float aspectRatio)
        {
            Light_SetAspectRatio(native_, aspectRatio);
            return this;
        }

        
    }
}
