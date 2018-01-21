using System;
using System.Collections.Generic;
using System.Text;

namespace UniqueEngine
{
    public enum MouseButton : byte
    {
        Left = 1,
        Middle = 2,
        Right = 3,
        X1 = 4,
        X2 = 5,
    }

    public partial class Input : Object
    {
        public bool GetKeyDown(Keycode key)
        {
            return Input_GetKeyDown(native_, (int)key);
        }

        public bool GetKeyPress(Keycode key)
        {
            return Input_GetKeyPress(native_, (int)key);
        }

        public bool GetMouseButtonDown(MouseButton button)
        {
            return Input_GetMouseButtonDown(native_, (1 << (((int)button) - 1)));
        }

        public bool GetMouseButtonPress(MouseButton button)
        {
            return Input_GetMouseButtonPress(native_, (1 << ((int)(button) - 1)));
        }

        public IntVector2 GetMousePosition()
        {
            return Input_GetMousePosition(native_);
        }

        public IntVector2 GetMouseMove()
        {
            return Input_GetMouseMove(native_);
        }

        public int GetMouseMoveX()
        {
            return Input_GetMouseMoveX(native_);
        }

        public int GetMouseMoveY()
        {
            return Input_GetMouseMoveY(native_);
        }

        public int GetMouseMoveWheel()
        {
            return Input_GetMouseMoveWheel(native_);
        }

    }
}
