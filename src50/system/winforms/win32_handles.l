// --- HInstance ---

public sealed const struct HInstance
{
    embeddable __raw theHandle[4];
    
    constructor(int handle)
    {
        theHandle := handle
    }
        
    get constructor Current()
    {
        int h := extern KERNEL32.GetModuleHandleW(0);
        
        theHandle := h
    }
}

// --- HINSTANCE ---

public static HInstance CurrentInstance = HInstance.Current;

// --- MSG ---

public sealed struct MSG 
{
    handle theHWND;
    int    theMessage;
    int    theWParam;
    int    theLParam;
    int    theTime;
    int    theX;
    int    theY;    
}

// --- CREATESTRUCT ---

public sealed struct CREATESTRUCT
{
    int       theCreateParam;
    HInstance theInstance; 
    int       theMenu;
    int       theParent;
    int       theHeight;
    int       theWidth;
    int       theY;
    int       theX;
    int       theStyle;
    int       theNamePtr;
    int       theClassPtr;
    int       theExStyle;
    
    int CreateParam
    {
        get()
        {
            ^ theCreateParam
        }        
    }
    
}

// --- HMENU ---

sealed const struct HMENU
{
    embeddable __raw theHandle[4];
    
    constructor new()
    {
        int h := extern USER32.CreateMenu();
        
        theHandle := h
    }

    constructor newPopup()
    {
        int h := extern USER32.CreatePopupMenu();
        
        theHandle := h
    }

    append(int flags, id, wide caption)
    {
        extern USER32.AppendMenuW(theHandle, flags, id, caption);
        
        //int err := extern KERNEL32.GetLastError();
    }
            
    append(int flags, handle parent, wide caption)
    {
        int f := flags || MF_POPUP;
        
        extern USER32.AppendMenuW(theHandle, /*flags || MF_POPUP*/f, parent, caption);
    }
            
    free()
    {
        extern USER32.DestroyMenu(theHandle)
    }
}
