//import system'drawing.
import system'winforms;

// --- BaseForm ---

public abstract class BaseWinForm : BaseWinContainer
{
    event Func1    onOpen;
    
    protected constructor new()
        <= new()
    {
    }
    
    appendControl(BaseControl control)
    {
        control.Parent := self;
        
        theControls.append(control)
    }
    
    open()
    {
        super.open();
        
        if(nil != onOpen)
            { onOpen(self) }
    }
    
    set bool Visible(bool value)
    {
        super.Visible := value;
        
        if(nil == theHandle)
        {
            self.open()
        };
    }
    
    close()
    {
        super.close();
        
        theHandle.free()
    }
}

// --- SDI ---

class SDIWindowListener : SDIWindowCallback
{
    SDIForm theOwner;
    
    constructor(SDIForm owner)
    {
        theOwner := owner
    }
    
    onsize(WindowHandle hwnd, int width, int height, ref int result)
    {
        theOwner.resize(hwnd, width, height);
        
        result := 0;
    }
    
    onclick(WindowHandle handle, ref int result)
    {
        auto control := theOwner.retrieve(handle);
        if(nil != control)
            { control.click() };
        
        super.onclick(handle, ref result)
    }

    onchange(WindowHandle handle, ref int result)
    {
        auto control := theOwner.retrieve(handle);
        if(nil != control)
            { control.change() };
        
        super.onchange(handle, ref result)
    }
    
    onmenu(int code, ref int retVal)
    {
        var item := theOwner.retrieveMenu(code);
        if(nil != item)
            { item.click() };
        
        super.onmenu(code, ref retVal)
    }
}

public closed class SDIForm : BaseWinForm
{
    object theMenu;
    
    constructor new()
        <= new()
    {
    }
    
    set Menu(menu)
    {
        theMenu := menu
    }
    
    retrieveMenu(int id)
        => theMenu;
    
    internal createHandle()
    {
        int styles := WS_OVERLAPPEDWINDOW;
        if(theVisible)
            { styles := styles || WS_VISIBLE };
        
        theRegion.read(ref int x, ref int y, ref int width, ref int height);
        
        theHandle := new WindowHandle(
                            EX_SDI_OVERLAPPEDWINDOW,
                            SDIWindowClass, 
                            cast wide(theCaption), 
                            styles,
                            x, y, width, height,
                            WindowHandle.Default, 
                            CurrentInstance,
                            new SDIWindowListener(self))
    }
}

// --- SDIDialog ---

public class SDIDialog : SDIForm
{
    constructor new()
        <= new()
    {
    }
    
    internal createHandle()
    {
        int styles := WS_DIALOGWINDOW;
        if(theVisible)
            { styles := styles || WS_VISIBLE };
        
        theRegion.read(ref int x, ref int y, ref int width, ref int height);
        
        theHandle := new WindowHandle(
                            EX_SDI_OVERLAPPEDWINDOW,
                            SDIWindowClass,
                            theCaption,
                            styles,
                            x, y, width, height,
                            WindowHandle.Default,
                            CurrentInstance,
                            new SDIWindowListener(self));
    }
}

//// --- Child ---
//
//class ChildWindowListener :: ChildWindowCallback
//{
//    form theOwner.
//    
//    constructor new form:anOwner
//    [
//        theOwner := anOwner.
//    ]
//    
//    onsize hwnd:aControl int:width int:height vint:aResult
//    [
//        theOwner $resize int:width int:height.
//        
//        aResult int := 0
//    ]
//    
//    onclick hwnd:aHandle vint:aResult
//    [
//        var aControl := theOwner retrieve hwnd:aHandle.
//        if($nil != aControl)
//            [ aControl click ].
//        
//        $super onclick hwnd:aHandle vint:aResult.
//    ]
//    
//    onchange hwnd:aHandle vint:aResult
//    [
//        var aControl := theOwner retrieve hwnd:aHandle.
//        if($nil != aControl)
//            [ aControl change ].
//        
//        $super onchange hwnd:aHandle vint:aResult
//    ]
//}
//
//limited class ChildForm :: BaseWinForm
//{
//    constructor new parent:aParent
//        <= new;
//    [
//        theParent := aParent
//    ]
//    
//    form = $self.
//    
//    run
//    [
//        $self open.
//        theParent set enabled:false.
//        
//        'program run hwnd:theHandle.
//        
//        theParent set enabled:true
//    ]    
//
//    close 
//    [
//        $self set visible:false.
//        
//        $super close
//    ]
//            
//    $createHandle
//    [
//        int Styles := WS_POPUPWINDOW || WS_DLGFRAME.
//        if(theVisible)
//            [ Styles := Styles || WS_VISIBLE ].
//        
//        int anX := 0.
//        int anY := 0.
//        int aWidth := 0.
//        int aHeight := 0.
//        
//        theRegion read vint:anX vint:anY vint:aWidth vint:aHeight.
//        
//        theHandle := WindowHandle new int:(WS_EX_DLGMODALFRAME || WS_EX_CONTROLPARENT)
//                        wide:SDIWindowClass wide:(theCaption wide) int:Styles
//                        int:anX int:anY int:aWidth int:aHeight 
//                        hwnd:(theParent hwnd)
//                        hinstance:CurrentInstance 
//                        object:(ChildWindowListener new form:$self).
//    ]
//}
