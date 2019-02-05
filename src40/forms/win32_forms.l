//import system'drawing.
import system'winforms;

// --- BaseForm ---

public abstract class BaseWinForm : BaseWinContainer
{
//    func1 theOpenEvent.
    
    constructor new()
        <= new()
    {
    }
    
//    controls = theControls.
//
//    set onOpen:aFunction
//    [
//        theOpenEvent := aFunction func1.
//    ]        
    
    open()
    {
        super.open();
        
//        if($nil != theOpenEvent)
//            [ theOpenEvent eval:self ]
    }
    
//    set visible : aValue
//    [
//        $super set visible:aValue.
//        
//        if($nil == theHandle)
//        [
//            $self open.
//        ].
//    ]
//    
//    close
//    [
//        $super close.
//        
//        theHandle free
//    ]
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
        theOwner.resize(int width, int height);
        
        result := 0;
    }
    
//    onclick hwnd:aHandle vint:aResult
//    [
//        var aControl := theOwner retrieve hwnd:aHandle.
//        if($nil != aControl)
//            [ aControl click ].
//        
//        $super onclick hwnd:aHandle vint:aResult
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
//    
//    onmenu int:aCode vint:aResult    
//    [
//        var anItem := theOwner retrieveMenu int:aCode.
//        if($nil != anItem)
//            [ anItem click ].
//        
//        $super onmenu int:aCode vint:aResult
//    ]    
}

public closed class SDIForm : BaseWinForm
{
//    object theMenu.
    
    constructor new()
        <= new()
    {
    }
    
//    sdiform = $self.
//    
//    set menu:aMenu
//    [
//        theMenu := aMenu.
//    ]
//    
//    stacksafe retrieveMenu int:anID
//        => theMenu.
    
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