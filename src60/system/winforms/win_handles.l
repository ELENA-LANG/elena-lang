import system;

// --- HInstance ---

public sealed const struct HInstance
{
   embeddable __ptr _handle;
    
   constructor(pointer handle)
   {
      _handle := handle
   }
        
   get constructor Current()
   {
      pointer h := extern KERNEL32.GetModuleHandleW(0);
        
      _handle := h
   }
}

// --- HINSTANCE ---

public static HInstance CurrentInstance = HInstance.Current;

// --- MSG ---

public sealed struct MSG 
{
   handle _hwnd;
   int    _message;
   int    _wparam;
   int    _lparam;
   int    _time;
   int    _x;
   int    _y;    
}

// --- CREATESTRUCT ---

public sealed struct CREATESTRUCT
{
    pointer   createParam : rprop;
    HInstance instance; 
    int       menu;
    int       parent;
    int       height;
    int       width;
    int       y;
    int       x;
    int       style;
    pointer   namePtr;
    pointer   classPtr;
    int       exStyle;
}
