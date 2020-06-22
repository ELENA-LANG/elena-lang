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

// --- Child ---

class ChildWindowListener : ChildWindowCallback
{
    ChildForm theOwner;
    
    constructor(ChildForm owner)
    {
        theOwner := owner
    }
    
    onsize(WindowHandle hwnd, int width, int height, ref int result)
    {
        theOwner.resize(hwnd, width, height);
        
        result := 0
    }
    
    onclick(WindowHandle handle, ref int result)
    {
        auto control := theOwner.retrieve(handle);
        if(nil != control)
            { control.click() };
        
        super.onclick(handle, ref result)
    }
    
    onchange(WindowHandle hwnd, ref int retVal)
    {
        auto control := theOwner.retrieve(handle);
        if(nil != control)
            { control.click() };
        
        super.onchange(handle, ref retVal)
    }
}

public closed class ChildForm : BaseWinForm
{
    constructor new(parent)
        <= new()
    {
        theParent := parent
    }
    
    run()
    {
        self.open();
        theParent.Enabled := false;
        
        forward program.run(theHandle);
        
        theParent.Enabled := true
    }    

    close()
    {
        self.Visible := false;
        
        super.close()
    }

    internal createHandle()
    {
        int styles := WS_POPUPWINDOW || WS_DLGFRAME;
        if(theVisible)
            { styles := styles || WS_VISIBLE };
        
        theRegion.read(ref int x, ref int y, ref int width, ref int height);
        
        theHandle := new WindowHandle(
                            WS_EX_DLGMODALFRAME || WS_EX_CONTROLPARENT,
                            SDIWindowClass,
                            theCaption,
                            styles,
                            x, y, width, height,
                            cast WindowHandle(theParent),
                            CurrentInstance,
                            new ChildWindowListener(self));
    }

}
