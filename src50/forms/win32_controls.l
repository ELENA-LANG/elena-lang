import system'collections;
import system'routines;
import system'drawing;
import system'winforms;

// --- IControl ---

public interface IControl
{
    get abstract object Control();
    
    get abstract string ID();
    
    abstract IControl retrieve(WindowHandle handle);
    
    abstract setRegion(int x, int y, int width, int height);
    
    abstract open();

    abstract click();

    abstract change();
        
    abstract close();
}

// --- BaseWindowControl ---

public abstract class BaseWinControl : BaseControl
{
    WindowHandle theHandle;

    protected constructor new()
        <= new()
    {
    }
    
    get Control() = self;
    
    IControl retrieve(WindowHandle handle)
    {
        if(nil == theHandle)
            { ^ nil };
            
        if(theHandle == handle)
            { ^ self };
            
        ^ nil 
    }

    internal abstract createHandle();

    internal onCreate()
    {
        ifnot(theEnabled )
            { theHandle.setEnabled(0) };
    }

    click() {}
    
    change() {}
    
    close()
    {
    }

    open()
    {
        if(nil != theHandle)
            { InvalidOperationException.new("The form is already open").raise() };
            
        self.createHandle();
        
        self.onCreate()
    }

//    opened = ($nil != theHandle).
    
    WindowHandle cast() = theHandle;

    bool Visible
    {
        set(bool v)
        {
            super.Visible := v;
            
            ifnot(nil == theHandle)
            {
                theHandle.setVisible(theVisible ? 5 : 0);
            }
        }
    }

    bool Enabled
    {
        set(bool value)
        {
            super.Enabled := value;
            
            ifnot(nil == theHandle)
            {
                int val := theEnabled ? -1 : 0;
                
                theHandle.setEnabled(val)
            }
            
        } 
    }
    
    string Caption
    {
        set(string s)
        {
            theCaption := s;
            //super.Caption := s;
            
            ifnot(nil == theHandle)
            {
                theHandle.setCaption(cast wide(s))
            }
        }
    
        set(wide s)
        {
            theCaption := s;
            //super.Caption := s;
            
            ifnot(nil == theHandle)
            {
                theHandle.setCaption(s)
            }
        }
        
        set(v)
        {
            self.Caption := cast string(v)
        }
    }

    get string Caption()
    {
        ifnot(nil == theHandle)
        {
            theCaption := theHandle.getCaption()
        };
            
        ^ theCaption
    }

    setRegion(int x, int y, int width, int height)
    {
        super.setRegion(x, y, width, height);
        
        ifnot(nil == theHandle)
        {
            theHandle.setPosition(x, y, 0, 0, SWP_NOSIZE);
            theHandle.setPosition(0, 0, width, height, SWP_NOMOVE)
        }        
    }
    
    refresh()
    {
        if(nil != theHandle)
            { theHandle.refresh() }
    }
    
    internal resize(WindowHandle hwnd, int width, int height)
    {
        theRegion.writeDimension(width, height)
    }  
}    

// --- BaseWinContainer ---

public abstract class BaseWinContainer : BaseWinControl
{
    List<IControl> theControls;
    event Func1    onResize;
    
    protected constructor new()
        <= new()
    {
        theControls := new List<IControl>();
    }

    IControl retrieve(WindowHandle handle)
    {
        if(theHandle == handle)
            { ^ self };

        int len := theControls.Length;
        IControl current;
        for (int i := 0, i < len, i += 1) {
            current := theControls[i].retrieve(handle);
            if (current != nil) {
                ^ current;
            }
        };
                            
        ^ nil
    }
        
    open()
    {
        super.open();
        
        int len := theControls.Length;
        for (int i := 0, i < len, i += 1) {
            theControls[i].open()
        }
    }
        
    close()
    {
        int len := theControls.Length;
        for (int i := 0, i < len, i += 1) {
            theControls[i].close()
        }
    }
    
    internal resize(WindowHandle hwnd, int width, int height)
    {
        super.resize(hwnd, width, height);

        if(nil != onResize)
            { onResize(self) }
    }
    
    IControl findByID(string id)
        = theControls.seekEach:(x => x.ID == id);
        
    Controls = new
    {
        get generic()
        {
            string id := __received.MessageName.toPrintable();
            
            auto ctrl := self.findByID(id);
            if (ctrl != nil)
            {
                ^ ctrl.Control
            }
            else
            {
                ^ nil
            }
        }
    };        
}

// --- Frame ---

public sealed class Frame : BaseWinControl, interface<IControl>
{
    constructor new()
        <= new()
    {
    }
    
    internal createHandle()
    {
        WindowHandle parent := theParent;
        
        int styles := WS_CHILD || BS_GROUPBOX;
        if(theVisible)
            { styles := styles || WS_VISIBLE };
        
        theRegion.read(ref int x, ref int y, ref int width, ref int height);
        
        theHandle := new WindowHandle(
                        WS_EX_TRANSPARENT,
                        ButtonClass, 
                        theCaption, 
                        styles,
                        x, y, width, height,
                        parent, 
                        CurrentInstance,
                        nil)
    }
}

// --- Label ---
public closed class Label : BaseWinControl, interface<IControl>
{
    constructor new()
        <= new()
    {
    }

    internal createHandle()
    {
        WindowHandle parent := theParent;
        
        int styles := WS_CHILD;
        if(theVisible)
            { styles := styles || WS_VISIBLE };
        
        theRegion.read(ref int x, ref int y, ref int width, ref int height);
        
        theHandle := new WindowHandle(
                        0,
                        StaticClass, 
                        theCaption, 
                        styles,
                        x, y, width, height,
                        parent, 
                        CurrentInstance,
                        nil)
    }
}

// --- Label ---

public class StaticLabel : Label
{
    constructor new()
        <= new()
    {
    }
    
    internal createHandle()
    {
        WindowHandle parent := theParent;
        if(parent == 0)
            { InvalidOperationException.new("The parent window is closed").raise() };
        
        int styles := WS_CHILD || SS_SIMPLE;
        if(theVisible)
            { styles := styles || WS_VISIBLE };
        
        theRegion.read(ref int x, ref int y, ref int width, ref int height);
        
        theHandle := new WindowHandle(
                        0,
                        StaticClass, 
                        theCaption, 
                        styles,
                        x, y, width, height,
                        parent, 
                        CurrentInstance,
                        nil)
                        
    }
}

// --- BaseEdit ---

abstract class BaseEdit : BaseWinControl
{
    event Func1    onChange;
    
    protected constructor new()
        <= new()
    {
    }
          
    string Value = self.Caption;
    
    get multiret Value(ref wide retVal)
    {
        retVal := self.Caption
    }
    
    set Value(string value)
    {
        self.Caption := cast WideString(value);
    }
    
    internal updateStyles(ref int retVal)
    {
        int styles := retVal;
        
        if(theVisible)
            { styles := styles || WS_VISIBLE };
        if(theTabStop)
            { styles := styles || WS_TABSTOP };
            
        retVal := styles
    }
}

// --- Edit ---

public sealed class Edit : BaseEdit, interface<IControl>
{
    constructor new()
        <= new()
    {
        theTabStop := true
    }
    
    internal createHandle()
    {
        WindowHandle parent := theParent;
        
        int styles := WS_CHILD || WS_BORDER;
        self.updateStyles(ref styles);
        
        theRegion.read(ref int x, ref int y, ref int width, ref int height);
        
        theHandle := new WindowHandle(
                        WS_EX_CLIENTEDGE,
                        EditClass, 
                        theCaption, 
                        styles,
                        x, y, width, height,
                        parent, 
                        CurrentInstance,
                        nil)                        
    }
}

// --- Memo ---

public sealed class Memo : BaseEdit, interface<IControl>
{
    constructor new()
        <= new()
    {
        theTabStop := true
    }
    
    internal createHandle()
    {
        WindowHandle parent := theParent;
        
        int styles := WS_CHILD || WS_BORDER || ES_MULTILINE || ES_WANTRETURN || ES_AUTOHSCROLL || ES_AUTOVSCROLL || WS_VSCROLL || WS_HSCROLL;
        self.updateStyles(ref styles);
        
        theRegion.read(ref int x, ref int y, ref int width, ref int height);
        
        theHandle := new WindowHandle(
                        WS_EX_CLIENTEDGE,
                        EditClass, 
                        theCaption, 
                        styles,
                        x, y, width, height,
                        parent, 
                        CurrentInstance,
                        nil)                        
    }
}

// --- BaseButton ---

public abstract class BaseButton : BaseWinControl
{
    event Func1 onClick;
    
    protected constructor new()
        <= new()
    {
        theTabStop := true
    }
    
    internal updateStyles(ref int retVal)
    {
        int styles := retVal;
        
        if(theVisible)
            { styles := styles || WS_VISIBLE };
        if(theTabStop)
            { styles := styles || WS_TABSTOP };
            
        retVal := styles
    }
    
    click()
    {
        if(nil != onClick)
            { onClick(self) }
    }
}

// --- Button ---

public sealed class Button : BaseButton, interface<IControl>
{
    constructor new()
        <= new()
    {
    }

    internal createHandle()
    {
        WindowHandle parent := theParent;
        
        int styles := WS_CHILD || BS_PUSHBUTTON;
        self.updateStyles(ref styles);
        
        theRegion.read(ref int x, ref int y, ref int width, ref int height);
        
        theHandle := new WindowHandle(
                        0,
                        ButtonClass, 
                        theCaption, 
                        styles,
                        x, y, width, height,
                        parent, 
                        CurrentInstance,
                        nil)                        
    }
}

// --- RadioButton ---

public closed class RadioButton : BaseButton, interface<IControl>
{
    bool theChecked;
    
    constructor new()
        <= new()
    {
        theChecked := false
    }
    
    constructor new(string caption)
        <= new()
    {
        theChecked := false;
        
        self.Caption := caption
    }
    
    bool Checked
    {
        get()
        {
            ifnot(nil == theHandle)
                { theChecked := theHandle.isChecked() };
                
            ^ theChecked
        }
        
        set(bool v)
        {
            theChecked := v;
            
            ifnot(nil == theHandle)
            {
                theHandle.setChecked(v ? BST_CHECKED : BST_UNCHECKED)
            }
        }
    }

    internal createHandle()
    {
        WindowHandle parent := theParent;
        
        int styles := WS_CHILD || BS_AUTORADIOBUTTON;
        self.updateStyles(ref styles);
        
        theRegion.read(ref int x, ref int y, ref int width, ref int height);
        
        theHandle := new WindowHandle(
                        0,
                        ButtonClass, 
                        theCaption, 
                        styles,
                        x, y, width, height,
                        parent, 
                        CurrentInstance,
                        nil);
                        
        if(theChecked)
            { self.Checked := true }
    }
}

// --- BaseList ---

public abstract class BaseList : BaseWinControl
{
    ArrayList theList;
    
    event Func<object,int,object> onIndexChanged;
    
    protected constructor new()
        <= new()
    {
        theList := new ArrayList()
    }
    
    abstract appendItem(wide caption);
    
    Items = new
    {
        append(object, caption)
        {
            theList.append(KeyValue.new(caption, object));
            
            self.appendItem(cast wide(caption))
        }
    };
    
    internal updateStyles(ref int retVal)
    {
        int styles := retVal;
        
        if(theVisible)
            { styles := styles || WS_VISIBLE };
        if(theTabStop)
            { styles := styles || WS_TABSTOP };
            
        retVal := styles
    }
}

// --- Combobox ---

public sealed class Combobox : BaseList, interface<IControl>
{
    constructor new()
        <= new()
    {
    }

    int SelectedIndex
    {
        get()
        {
            int index := extern USER32.SendMessageW(theHandle, CB_GETCURSEL, 0, 0);
            
            ^ index
        }
        
        set(int value)
        {
            extern USER32.SendMessageW(theHandle, CB_SETCURSEL, value, 0);
            
            self.change()
        }
    }
    
    get Selected()
    {
        int index := self.SelectedIndex;
            
        ^ theList[index]
    }
    
    change() 
    {
        if(onIndexChanged != nil)
        {
            int index := self.SelectedIndex;
            
            onIndexChanged(self, index)
        }
    }
    
    appendItem(wide caption)
    {
        if(nil == theHandle)
            { ^ self };
        
        theHandle.sendMessage(CB_ADDSTRING, 0, caption)
    }
    
    internal loadStrings()
    {
        int len := theList.Length;
        
        int i := 0;
        while(i < len)
        {
            var kv := theList[i];
            
            theHandle.sendMessage(CB_ADDSTRING, 0, kv.Key);
            
            i := i + 1
        }
    }
    
    internal createHandle()
    {
        WindowHandle parent := theParent;
        
        int styles := WS_CHILD || CBS_DROPDOWNLIST;
        self.updateStyles(ref styles);
        
        theRegion.read(ref int x, ref int y, ref int width, ref int height);
        
        theHandle := new WindowHandle(
                        0,
                        ComboboxClass, 
                        emptyWideString, 
                        styles,
                        x, y, width, height,
                        parent, 
                        CurrentInstance,
                        nil);
                        
        self.loadStrings()
    }
}

// --- Paintbox ---

class PaintboxListener : WindowCallback
{
    Paintbox theOwner;
    
    constructor(Paintbox owner)
    {
        theOwner := owner
    }
    
    onpaint(WindowHandle hwnd, ref int retVal)
    {
        theOwner.paint();
        
        retVal := 0
    }
    
    onsize(WindowHandle hwnd, int width, int height, ref int result)
    {
        theOwner.resize(hwnd, width, height);
        
        result := 0
    }
    
    ondestroy(WindowHandle hwnd, ref int result)
    {
        theOwner.destroy();
        
        super.ondestroy(hwnd, ref result)
    }
}

public closed class Paintbox : BaseWinControl, interface<IControl>
{
    Canvas  theCanvas;
    
    event Func<object,Canvas,object> onPaint;
    
    constructor new()
        <= new()
    {
        theCanvas := new Canvas()
    }

    internal destroy()
    {
        theCanvas.free()
    }
 
    internal paint()
    {
        PAINTSTRUCT struct := new PAINTSTRUCT();
        
        struct.begin(theHandle);
        HDC dc := struct.HDC;
        
        theCanvas.open(dc);
        
        onPaint.?(self, theCanvas);

        theCanvas.close();

        struct.end(theHandle)
    }
 
    internal createHandle()
    {
        WindowHandle parent := theParent;

        int styles := WS_CHILD;
        if(theVisible)
            { styles := styles || WS_VISIBLE };
                        
        theRegion.read(ref int x, ref int y, ref int width, ref int height);
        
        theHandle := new WindowHandle(
                        WS_EX_TRANSPARENT,
                        PaintboxClass,
                        theCaption,
                        styles,
                        x, y, width, height,
                        parent,
                        CurrentInstance,
                        new PaintboxListener(self))
    }
}

public sealed class Imagebox : Paintbox
{
    HBITMAP  theImage;
    HDC      theImageDC;
    bool     theNeedToRefresh;
    
    constructor new()
        <= new()
    {
        theNeedToRefresh := false
    }

    internal destroy()
    {
        super.destroy();
        
        theImage.free();
        theImageDC.free()
    }
    
    internal paint()
    {
        if(theNeedToRefresh)
            { self.refresh() };
        
        PAINTSTRUCT struct := new PAINTSTRUCT();
        
        struct.begin(theHandle);
        
        theRegion.readDimensionTo(ref int width, ref int height);
        
        HDC dc := struct.HDC;
        
        dc.copy(0, 0, width, height, theImageDC, 0, 0, SRCCOPY);
//        int err := system'external'KERNEL32 GetLastError.
                
        struct.end(theHandle)
    }
 
    refresh()
    {
        if(nil == theHandle)
            { ^ self };
        
        onPaint.?(self, theCanvas);

        super.refresh();
        theNeedToRefresh := false
    }
 
    internal resize(WindowHandle handle, int width, int height)
    {
        ifnot(nil == theImage)
        {
            theImage.free();
            theImageDC.free()
        };
            
        HDC dc := handle.HDC;
            
        theImage := HBITMAP.create(dc, width, height);
        theImageDC := HDC.newCompatible(dc);
        theImage.select(theImageDC);

        theCanvas.open(theImageDC);

        theNeedToRefresh := true
    }
}

// --- BasePanel ---

public abstract class BasePanel : BaseWinContainer
{
    Frame theFrame;
    
    protected constructor new()
        <= new()
    {
        theFrame := Frame.new();
        
        // NOTE : a frame should be the first element
        self.appendControlInternal:theFrame
    }
    
    internal appendControlInternal(BaseControl control)
    {
        control.Parent := self;
        
        theControls.append(control)        
    }
    
    internal appendControlInternal(ImageList control)
    {
        control.Parent := self;
        
        theControls.append(control)        
    }
    
    int Width
    {
        set(int v)
        {
            super.Width := v;
            
            theFrame.Width := v
        }        
    }
    
    int Height
    {
        set(int h)
        {
            super.Height := h;
            
            theFrame.Height := h
        }
    }
            
    setRegion(int x, int y, int width, int height)
    {
        super.setRegion(x, y, width, height);
        
        theFrame.setRegion(0, 0, width, height);
    }
    
    string Caption
    {
        set(string caption)
        {
            theFrame.Caption := caption
        }
        
        set(wide caption)
        {
            theFrame.Caption := caption
        }
    }    
    
    internal resize(WindowHandle handle, int width, int height)
    {
        theFrame.Width := width;
        theFrame.Height := height
    }    
}

// --- Panel ---

class PanelListener : WindowCallback
{
    Panel theOwner;
    
    constructor(Panel owner)
    {
        theOwner := owner
    }
    
    onsize(WindowHandle hwnd, int width, int height, ref int result)
    {
        theOwner.resize(hwnd, width, height);
        
        result := 0
    }
    
    onclick(WindowHandle hwnd, ref int result)
    {
        var control := theOwner.retrieve(hwnd);
        if(nil != control)
            { control.click() };
        
        super.onclick(hwnd, ref result);
    }
    
    onchange(WindowHandle hwnd, ref int result)
    {
        var control := theOwner.retrieve(hwnd);
        if(nil != control)
            { control.change() };
        
        super.onchange(hwnd, ref result);
    }
}

public sealed class Panel : BasePanel, interface<IControl>
{
    constructor new()
        <= new()
    {
    }
    
    appendControl(BaseControl control)
    {
        self.appendControlInternal(control);
    }
    appendControl(ImageList control)
    {
        self.appendControlInternal(control);
    }
    
    internal createHandle()
    {
        WindowHandle parent := theParent;

        int styles := WS_CHILD || WS_CLIPSIBLINGS;
        if(theVisible)
            { styles := styles || WS_VISIBLE };
                        
        theRegion.read(ref int x, ref int y, ref int width, ref int height);
        
        theHandle := new WindowHandle(
                        WS_EX_CONTROLPARENT,
                        PanelClass,
                        theCaption,
                        styles,
                        x, y, width, height,
                        parent,
                        CurrentInstance,
                        new PanelListener(self))
                        
    }
}

// --- RadioButtonGroup ---

class RadioButtonGroupListener : WindowCallback
{
    RadioButtonGroup theOwner;
    
    onclick(WindowHandle hwnd, ref int retVal)
    {
        theOwner.onClick(hwnd);
        
        retVal := 0
    }

    onsize(WindowHandle hwnd, int width, int height, ref int result)
    {
        theOwner.resize(hwnd, width, height);
        
        result := 0
    }
    
    constructor(RadioButtonGroup owner)
    {
        theOwner := owner
    }
}

public sealed class RadioButtonGroup : BasePanel, interface<IControl>
{
    int   theSelected;
    event Func1 onIndexChanged;
    
    constructor new()
        <= new()
    {
        theSelected := -1
    }

    int SelectedIndex
    {
        get() = theSelected;
        
        set(int n)
        {
            ifnot(-1 == theSelected)
                { self.setChecked(false) };
            
            theSelected := n;
        
            self.setChecked(true);
        
            onIndexChanged.?(self);
        }
    }

    private setChecked(value)
    {
        int len := theControls.Length;
        IControl current;
        for (int i := 1, i < len, i += 1) {
            current := theControls[i];
            int index := i - 1;
            if (index == theSelected)
            {
                current.Control.Checked := value;
                
                ^ self
            }
        };
    }

    appendItem(string s)
    {
        self.appendControlInternal(RadioButton.new(s));
    }

    setRegion(int x, int y, int width, int height)
    {
        super.setRegion(x, y, width, height);
        
        self.resize()
    }

    internal onClick(WindowHandle handle)
    {
        var retVal := nil;
        int len := theControls.Length;
        IControl current;
        for (int i := 1, i < len, i += 1) {
            current := theControls[i];            
            
            retVal := current.retrieve(handle);
            if(nil != retVal)
            {
                int index := i - 1;
                theSelected := index;
                
                onIndexChanged.?(self);
                
                ^ self
            }
        }
    }

    internal resize()
    {
        int width := theRegion.Width;
        int height := theRegion.Height;
        
        self.resize(theHandle, width, height)
    }
            
    internal resize(WindowHandle handle, int width, int height)
    {
        int len := theControls.Length;
        IControl current;
        for (int i := 1, i < len, i += 1) {
            current := theControls[i];
            int index := i - 1;
            
            current.setRegion(4, 18 + index * 20, width - 6, 20);
        };
        
        super.resize(handle, width, height)
    }
    
    internal createHandle()
    {
        WindowHandle parent := theParent;

        int styles := WS_CHILD || WS_CLIPSIBLINGS;
        if(theVisible)
            { styles := styles || WS_VISIBLE };
                        
        theRegion.read(ref int x, ref int y, ref int width, ref int height);
        
        theHandle := new WindowHandle(
                        WS_EX_CONTROLPARENT,
                        PanelClass,
                        theCaption,
                        styles,
                        x, y, width, height,
                        parent,
                        CurrentInstance,
                        new RadioButtonGroupListener(self));
                                                
        if(theSelected != -1 && nil != onIndexChanged)
            { onIndexChanged(self) }
    }
}

// --- ImageList ---

public sealed class ImageList : Object, interface<IControl>
{
    Dimension    theSize;
    object       theParent;
    List<Image>  theImages; 
    int          theIndex;
 
    constructor new()
    {
        theIndex := 0;
        theImages := new List<Image>();
        
        theSize :=  new Dimension(50, 50)
    }

    setRegion(int x, int y, int width, int height)
    {
        theSize.Width := width;
        theSize.Height := height
    }

    get Control() = self;

    prop string ID;
         
    assign(Paintbox paintbox)
    {
        paintbox.onPaint := (object sender, Canvas canvas)
        {
            int width := theSize.Width;
            int height := theSize.Height;
            
            auto image := theImages[theIndex];
            
            canvas.write(image, 0, 0, width, height)
        }
    }

    int SelectedIndex
    {
        get() = theIndex;
        
        set(int n)
        {
            theIndex := n
        }
    }

    IControl retrieve(WindowHandle handle) 
    {
        ^ nil
    }
    
    click() {}

    Parent = theParent;    

    set internal Parent(object o)
    {
        theParent := o
    }

    appendImage(string path)
    {        
        theImages.append(Image.load(path, theSize))
    }
    
    open()
    {
        WindowHandle parent := theParent;
        HDC parentDC := parent.HDC;        

        int len := theImages.Length;
        for (int i := 0, i < len, i += 1) {
            theImages[i].open(parentDC)
        }
    }
    
    change() {}
    
    close()
    {
        int len := theImages.Length;
        for (int i := 0, i < len, i += 1) {
            theImages[i].free()
        }
    }    
}
