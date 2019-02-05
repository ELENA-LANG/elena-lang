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
//        <= new &int:(system'external'KERNEL32 GetModuleHandleW &int:0).
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
    
//    embeddable constructor new    
//    [
//        theMessage := 0.
//    ]
//    
//    int rvprop message :: readMessageTo.
//    
//    stacksafe readMessageTo vint:aMessage
//    [
//        aMessage int := theMessage.
//    ]
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
    
//    stacksafe implicit int:n
//    [
//    ]
    
    public vprop int CreateParam
    {
        readTo(ref int retVal)
        {
            retVal := theCreateParam
        }        
    }
    
}

//// --- HMENU ---
//
//sealed const struct HMENU
//{
//    __dword<4> theHandle.
//    
//    constructor new
//    [
//        int h := system'external'USER32 CreateMenu.
//        
//        theHandle := h.
//    ]
//
//    constructor newPopup
//    [
//        int h := system'external'USER32 CreatePopupMenu.
//        
//        theHandle := h.
//    ]
//
//    stacksafe append int:Flags int:anID wide:aCaption
//    [
//        system'external'USER32 AppendMenuW int:theHandle int:Flags int:anID wide:aCaption.
//        
//        int err := system'external'KERNEL32 GetLastError.
//    ]
//            
//    stacksafe append int:Flags handle:aParent wide:aCaption
//    [
//        system'external'USER32 AppendMenuW int:theHandle int:(Flags || MF_POPUP) int:aParent wide:aCaption.
//    ]
//            
//    stacksafe free
//    [
//        system'external'USER32 DestroyMenu int:theHandle.
//    ]
//}
