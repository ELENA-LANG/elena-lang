// --- HInstance ---

public sealed const struct HInstance
{
   handle _handle;
    
   constructor(handle handle)
   {
      _handle := handle
   }
        
   get constructor Current()
   {
      handle h := extern KERNEL32.GetModuleHandleW(0);
        
      _handle := h
   }

   handle Handle
      = _handle;
}

// --- HINSTANCE ---

public static HInstance CurrentInstance = HInstance.Current;

// --- MSG ---

public sealed packed MSG 
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

public sealed packed CREATESTRUCT
{
    pointer   CreateParam : rprop;
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
