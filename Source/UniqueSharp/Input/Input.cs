using System;
using System.Collections.Generic;
using System.Text;

namespace Unique.Engine
{
    public partial class Input : Object
    {
        public bool GetKeyDown(int key)
        {
            return Input_GetKeyDown(native_, key);
        }

        public bool GetKeyPress(int key)
        {
            return Input_GetKeyPress(native_, key);
        }

        public bool GetMouseButtonDown(int button)
        {
            return Input_GetMouseButtonDown(native_, button);
        }

        public bool GetMouseButtonPress(int button)
        {
            return Input_GetMouseButtonPress(native_, button);
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
