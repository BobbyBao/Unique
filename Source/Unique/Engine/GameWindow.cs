using System;
using System.Collections.Generic;
using System.Text;

namespace Unique.Engine
{
    public enum WindowState
    {
        Normal,
        FullScreen,
        Maximized,
        Minimized,
        BorderlessFullScreen
    }

    public abstract class GameWindow : Object
    {
        public virtual IntPtr Handle
        {
            get;
        }

        public virtual int Width
        {
            get; protected set;
        }

        public virtual int Height
        {
            get; protected set;
        }

        public virtual void Resize(int w, int h)
        {/*
            Graphics graphics = GetSubsystem<Graphics>();
            if(graphics)
            {
                graphics.Resize(w, h);
            }*/
        }

        public virtual void Close()
        {
        }

        public abstract int RunMessageLoop();

        public abstract void ProcessEvents();

        public event Action OnResized;
        public event Action OnClose;
        public event Action OnFocusLost;
        public event Action OnFocusGained;
        public event Action OnShown;
        public event Action OnHidden;
        public event Action OnMouseEnter;
        public event Action OnMouseLeave;
        public event Action OnExposed;
        public event Action<IntVector2> OnMoved;

        protected void RaiseResized(int w, int h)
        {
            Resize(w, h);
            OnResized?.Invoke();
        }

        protected void RaiseClose()
        {
            OnClose?.Invoke();
        }
        
        protected void RaiseFocusLost()
        {
            OnFocusLost?.Invoke();
        }

        protected void RaiseFocusGained()
        {
            OnFocusGained?.Invoke();
        }

        protected void RaiseShown()
        {
            OnShown?.Invoke();
        }

        protected void RaiseHidden()
        {
            OnHidden?.Invoke();
        }

        protected void RaiseMouseEnter()
        {
            OnMouseEnter?.Invoke();
        }

        protected void RaiseMouseLeave()
        {
            OnMouseLeave?.Invoke();
        }

        protected void RaiseExposed()
        {
            OnExposed?.Invoke();
        }

        protected void RaiseMoved(IntVector2 point)
        {
            OnMoved?.Invoke(point);
        }
    }
}
