using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace Unique.Engine
{
    public enum MouseButton
    {
        None,
        Left,
        Middle,
        Right,
        X1,
        X2,
        Count

    }

    public enum GamepadAxis
    {
        LeftX,
        LeftY,
        LeftZ,
        RightX,
        RightY,
        RightZ,

        Count
    }

    public enum Modifier : byte
    {
        None = 0,
        LeftAlt = 0x01,
        RightAlt = 0x02,
        LeftCtrl = 0x04,
        RightCtrl = 0x08,
        LeftShift = 0x10,
        RightShift = 0x20,
        LeftGUI = 0x40,
        RightGUI = 0x80,     
    }

    public enum Key : int
    {
        None = 0,
        Esc,
        Return,
        Tab,
        Space,
        Backspace,
        Up,
        Down,
        Left,
        Right,
        Insert,
        Delete,
        Home,
        End,
        PageUp,
        PageDown,
        Print,
        Plus,
        Minus,
        LeftBracket,
        RightBracket,
        Semicolon,
        Quote,
        Comma,
        Period,
        Slash,
        Backslash,
        Tilde,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        NumPad0,
        NumPad1,
        NumPad2,
        NumPad3,
        NumPad4,
        NumPad5,
        NumPad6,
        NumPad7,
        NumPad8,
        NumPad9,
        Key0,
        Key1,
        Key2,
        Key3,
        Key4,
        Key5,
        Key6,
        Key7,
        Key8,
        Key9,
        KeyA,
        KeyB,
        KeyC,
        KeyD,
        KeyE,
        KeyF,
        KeyG,
        KeyH,
        KeyI,
        KeyJ,
        KeyK,
        KeyL,
        KeyM,
        KeyN,
        KeyO,
        KeyP,
        KeyQ,
        KeyR,
        KeyS,
        KeyT,
        KeyU,
        KeyV,
        KeyW,
        KeyX,
        KeyY,
        KeyZ,

        GamepadA,
        GamepadB,
        GamepadX,
        GamepadY,
        GamepadThumbL,
        GamepadThumbR,
        GamepadShoulderL,
        GamepadShoulderR,
        GamepadUp,
        GamepadDown,
        GamepadLeft,
        GamepadRight,
        GamepadBack,
        GamepadStart,
        GamepadGuide,

        Count
     
    }

    public class Input
    {
        static byte[] chars_ = new byte[4];
        static byte[] modifiers_ = new byte[4];

        public unsafe static bool GetKeyState(Key key)
        {
            fixed (byte* m = &modifiers_[0])
                return GetKeyState(key,(IntPtr) m);
        }

        public static bool GetKeyState(Key key, IntPtr modifiers)
        {
            return Input_GetKeyState(key, modifiers) != 0;
        }
        
        public static byte GetModifiersState()
        {
            return Input_GetModifiersState();
        }
        
        public static byte[] GetChar()
        {
            IntPtr c = Input_GetChar();
            //Utilities.Write()
            return chars_;
        }
        
        public unsafe static void GetMouse(ref IntVector3 mouse)
        {
            Input_GetMouse(ref mouse);
        }

        public static bool GetMouseState(MouseButton button)
        {
            return Input_GetMouseState(button) != 0;
        }
        
        public static bool GetGamepadAxis(ushort handle, GamepadAxis axis)
        {
            return Input_GetGamepadAxis(handle, axis) != 0;
        }

        public static void InjectKeyState(Key key, Modifier modifier, byte down)
        {
            Input_InjectKeyState((int)key, (byte)modifier, down);
        }
        
        public static void InjectMousePos(int mx, int my)
        {
            Input_InjectMousePos(mx, my);
        }

        public static void InjectMouseWheel(int delta)
        {
            Input_InjectMouseWheel(delta);
        }

        public static void InjectMouseButtonState(MouseButton button, byte down)
        {
            Input_InjectMouseButtonState((byte)button, down);
        }

        public static void InjectChar(byte[] c)
        {
            unsafe
            {
                fixed (byte* mc = c)
                    Input_InjectChar((byte)c.Length, mc);
            }
        }

        public unsafe static void InjectChar(int len, byte* c)
        {
            Input_InjectChar((byte)len, c);
        }

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern byte Input_GetKeyState(Key key, IntPtr modifiers);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern byte Input_GetModifiersState();

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Input_GetChar();

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Input_GetMouse(ref IntVector3 _mouse);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern byte Input_GetMouseState(MouseButton _button);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern byte Input_GetGamepadAxis(ushort _handle, GamepadAxis _axis);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Input_InjectKeyState(int _key, byte _modifiers, byte _down);
        
        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Input_InjectMousePos(int _mx, int _my);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Input_InjectMouseWheel(int delta);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        static extern void Input_InjectMouseButtonState(byte _button, byte _state);

        [DllImport(Native.DllName, CallingConvention = CallingConvention.Cdecl)]
        unsafe static extern void Input_InjectChar(byte _len, byte* _char);
    }
}
