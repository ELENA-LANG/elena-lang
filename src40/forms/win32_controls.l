import system'collections;
//import system'drawing.
import system'winforms;

// --- IControl ---

public interface IControl
{
//    control = $self.
//    
//    control_object = $nil.
    
    abstract IControl retrieve(WindowHandle handle) {}
    
//    stacksafe readDimensionTo vint:aWidth vint:aHeight []
//    
//    stacksafe readLocationTo vint:x vint:y []
//    
//    stacksafe setDimension int:aWidth int:aHeight []
//    
//    stacksafe setLocation int:x int:y []
    
    abstract open() {}

    abstract click() {}

//    change []
        
    abstract close() {}
}

//public IControlOf<T>
//{
//    control = IControl::
//    {
//        control_object = $self.
//        
//        open [ $self open ]
//    
//        change [ $self change ]
//    
//        close [ $self close ]
//    
//        retrieve hwnd:aHandle = $self retrieve hwnd:aHandle.
//    
//        setDimension int:aWidth int:aHeight [ $self setDimension int:aWidth int:aHeight ]
//    
//        setLocation int:aWidth int:aHeight [ $self setLocation int:aWidth int:aHeight. ]
//    
//        readDimensionTo vint:aWidth vint:aHeight [ $self readDimensionTo vint:aWidth vint:aHeight ]
//    
//        readLocationTo vint:aWidth vint:aHeight [ $self readLocationTo vint:aWidth vint:aHeight ]
//    }.
//}

// --- BaseWindowControl ---

public abstract class BaseWinControl : BaseControl
{
    WindowHandle theHandle;

    constructor new()
        <= new()
    {
    }
    
    IControl retrieve(WindowHandle handle)
    {
        if(nil == theHandle)
            { ^ nil };
            
        if(theHandle == handle)
            { ^ self };
            
        ^ nil 
    }

    internal abstract createHandle() {}            

    internal onCreate()
    {
        ifnot(theEnabled )
            { theHandle.setEnabled(0) };
    }

    click() {}
    
//    change []
    
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

//    set visible : aValue
//    [
//        $super set visible:aValue.
//        
//        ifnot($nil == theHandle)
//            [
//                if(theVisible)
//                    [ theHandle setVisible int:5 ];
//                    [ theHandle setVisible int:0 ]
//            ].
//    ]
//
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
                                                                                                
    set:Caption(string s)
    {
        theCaption := s;
        //super.Caption := s;
        
        ifnot(nil == theHandle)
        {
            theHandle.setCaption(cast wide(s))
        }
    }

    set:Caption(wide s)
    {
        theCaption := s;
        //super.Caption := s;
        
        ifnot(nil == theHandle)
        {
            theHandle.setCaption(s)
        }
    }

//    append caption : aValue
//    [    
//        $self set caption($self caption + aValue wide)
//    ]
//
//    caption
//    [
//        ifnot($nil == theHandle)
//            [
//                theCaption := theHandle getCaption
//            ].
//            
//        ^ theCaption
//    ]
//
//    setLocation int:x int:y
//    [
//        theRegion writeLocation int:x int:y.
//        
//        ifnot($nil == theHandle)
//            [
//                theHandle setPosition int:x int:y int:0 int:0 int:SWP_NOSIZE
//            ]
//    ]
//    
//    stacksafe setDimension int:aWidth int:aHeight
//    [
//        theRegion writeDimension int:aWidth int:aHeight.
//        
//        ifnot($nil == theHandle)
//            [
//                theHandle setPosition int:0 int:0 int:aWidth int:aHeight int:SWP_NOMOVE
//            ]
//    ]
//    
//    refresh
//    [
//        if($nil != theHandle)
//            [ theHandle refresh ]
//    ]
    
    internal resize(int width, int height)
    {
        theRegion.writeDimension(width, height)
    }  
}    

//// --- ControlCollection ---
//
//sealed class ControlCollection :: BaseControlCollection
//{
//    object theParent.
//    
//    constructor new : aParent
//        <= new;
//    [
//        theParent := aParent
//    ]
//    
//    append : aControl
//    [
//        $super append:(aControl control).
//        
//        aControl $set parent:theParent.
//        
//        if(theParent opened)
//        [
//            aControl open
//        ]
//    ]
//}

// --- BaseWinContainer ---

public abstract class BaseWinContainer : BaseWinControl
{
    List<IControl> theControls;
//    func1    theResize.
    
    constructor new()
        <= new()
    {
        theControls := new List<IControl>();
    }

//    set onResize:anAction
//    [
//        theResize := anAction func1.
//    ]

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
    
    internal resize(int width, int height)
    {
        super.resize(width, height);

//        ifnot($nil == theResize)
//            [ theResize eval:$self ]
    }  
}

//// --- Frame ---
//
//sealed class Frame :: BaseWinControl, icontrolof<frame>
//{
//    constructor new
//        <= new;
//    [
//    ]
//    
//    $createHandle
//    [
//        hwnd aParent := theParent hwnd.
//        
//        int Styles := WS_CHILD || BS_GROUPBOX.
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
//        theHandle := WindowHandle new int:WS_EX_TRANSPARENT
//                        wide:ButtonClass wide:(theCaption wide) int:Styles
//                        int:anX int:anY int:aWidth int:aHeight 
//                        hwnd:aParent hinstance:CurrentInstance object:$nil.
//    ]
//}

// --- Label ---
public closed class Label : BaseWinControl, interface<IControl>
{
    constructor()
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

//// --- Label ---
//
//class StaticLabel :: Label
//{
//    constructor new
//        <= new;
//    [
//    ]
//    
//    $createHandle
//    [
//        hwnd aParent := theParent hwnd.
//        if(0 == aParent)
//            [ InvalidOperationException new:"The parent window is closed"; raise ].
//        
//        int Styles := WS_CHILD || SS_SIMPLE.
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
//        theHandle := WindowHandle new int:0
//                        wide:StaticClass wide:(theCaption wide) int:Styles
//                        int:anX int:anY int:aWidth int:aHeight 
//                        hwnd:aParent hinstance:CurrentInstance object:$nil
//    ]
//}

// --- BaseEdit ---

abstract class BaseEdit : BaseWinControl
{
//    func1 theChanged.
//    
    constructor new()
        <= new()
    {
    }
    
//    set onChange:aFunction
//    [
//        theChanged := aFunction func1.
//    ]
//    
//    wide = $self caption.
//          
//    literal = $self caption.
          
    string Value = self.Caption;
    
//    append : aValue
//    [
//        $self set caption:($self caption + aValue wide).
//    ]
//    

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
    
//    change 
//    [
//        if($nil != theChanged)
//            [ theChanged eval:$self ]
//    ]
}

// --- Edit ---

public sealed class Edit : BaseEdit, interface<IControl>
{
    constructor()
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

//// --- Memo ---
//
//sealed class Memo :: BaseEdit, icontrolof<memo>
//{
//    constructor new
//        <= new;
//    [
//        theTabStop := true
//    ]
// 
//    wide = $self caption.
//          
//    literal = $self caption.
//    
//    $createHandle
//    [
//        hwnd aParent := theParent hwnd.
//        
//        int Styles := WS_CHILD || WS_BORDER || ES_MULTILINE || ES_WANTRETURN || ES_AUTOHSCROLL || ES_AUTOVSCROLL || WS_VSCROLL || WS_HSCROLL.
//        $self $updateStyles vint:Styles.
//        
//        int anX := 0.
//        int anY := 0.
//        int aWidth := 0.
//        int aHeight := 0.
//        
//        theRegion read vint:anX vint:anY vint:aWidth vint:aHeight.
//        
//        theHandle := WindowHandle new int:WS_EX_CLIENTEDGE
//                        wide:EditClass wide:(theCaption wide) int:Styles
//                        int:anX int:anY int:aWidth int:aHeight 
//                        hwnd:aParent hinstance:CurrentInstance object:$nil.
//    ]
//}

// --- BaseButton ---

public abstract class BaseButton : BaseWinControl
{
    event Func1 onClick;
    
    constructor new()
        <= new()
    {
//        theTabStop := true.
    }
    
    internal updateStyles(ref int retVal)
    {
        int styles := retVal;
        
        if(theVisible)
            { styles := styles || WS_VISIBLE };
//        if(theTabStop)
//            [ Styles := Styles || WS_TABSTOP ].
            
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
    constructor()
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

//// --- RadioButton ---
//
//limited class RadioButton :: BaseButton, icontrolof<radiobutton>
//{
//    bool theChecked.
//    
//    constructor new
//        <= new;
//    [
//        theChecked := false.
//    ]
//    
//    checked
//    [
//        ifnot($nil == theHandle)
//            [ theChecked := theHandle isChecked ].
//            
//        ^ theChecked
//    ]
// 
//    set checked:aValue
//    [
//        theChecked := aValue bool.
//        
//        ifnot($nil == theHandle)
//        [
//            if(theChecked)
//                [ theHandle setChecked int:BST_CHECKED ];
//                [ theHandle setChecked int:BST_UNCHECKED ]
//        ].
//    ]
//
//    $createHandle
//    [
//        hwnd aParent := theParent hwnd.
//        
//        int Styles := 0.
//        Styles := (WS_CHILD || BS_AUTORADIOBUTTON).
//        $self $updateStyles vint:Styles.
//                
//        int anX := 0.
//        int anY := 0.
//        int aWidth := 0.
//        int aHeight := 0.
//        
//        theRegion read vint:anX vint:anY vint:aWidth vint:aHeight.
//        
//        theHandle := WindowHandle new int:0
//                        wide:ButtonClass wide:(theCaption wide) int:Styles
//                        int:anX int:anY int:aWidth int:aHeight 
//                        hwnd:aParent hinstance:CurrentInstance object:$nil.
//                        
//        if(theChecked)
//            [ $self set checked:true ]
//    ]
//}
//
//// --- BaseList ---
//
//class BaseList :: BaseWinControl
//{
//    array_list theList.
//    
//    index_changed event onIndexChanged :: theChangeAction.
//    
//    constructor new
//        <= new;
//    [
//        theList := ArrayList new
//    ]
//    
//    items =
//    {
//        append : anObject as:aCaption
//        [
//            theList append:(KeyValue new key:aCaption value:anObject).
//            
//            self $appendItem wide:(aCaption wide)
//        ]
//    }.
//    
//    stacksafe $updateStyles vint:aRetVal
//    [
//        int Styles := aRetVal.
//        
//        if(theVisible)
//            [ Styles := Styles || WS_VISIBLE ].
//        if(theTabStop)
//            [ Styles := Styles || WS_TABSTOP ].
//            
//        aRetVal int := Styles.
//    ]
//}
//
//// --- Combobox ---
//
//sealed class Combobox :: BaseList, icontrolof<combobox>
//{
//    constructor new
//        <= new;
//    [
//    ]
//    
//    int vprop selected_index :: readSelectedIndexTo, setSelectedIndex.
//    
//    stacksafe readSelectedIndexTo vint:retVal
//    [
//        int index := system'external'USER32 SendMessageW int:theHandle int:CB_GETCURSEL int:0 int:0.
//        
//        retVal int := index
//    ]
//    
//    stacksafe setSelectedIndex int:index
//    [
//        system'external'USER32 SendMessageW int:theHandle int:CB_SETCURSEL int:index int:0.
//        
//        $self change
//    ]
//    
//    selected 
//    [
//        int index := $self selected_index.
//            
//        ^ theList getAt int:index.
//    ]
//    
//    change 
//    [
//        if($nil != theChangeAction)
//        [
//            int index := $self selected_index.
//            
//            theChangeAction onIndexChange:$self int:index.
//        ].
//    ]
//    
//    $appendItem wide:aCaption
//    [
//        if($nil == theHandle)
//            [ ^ $self ].
//        
//        theHandle sendMessage int:CB_ADDSTRING int:0 wide:aCaption.
//    ]
//    
//    $loadStrings
//    [
//        int aLength := theList length.
//        
//        int i := 0.
//        while(i < aLength)
//        [
//            var aKeyValue := theList getAt int:i.
//            
//            theHandle sendMessage int:CB_ADDSTRING int:0 wide:(aKeyValue key).
//            
//            i := i + 1
//        ]
//    ]
//    
//    $createHandle
//    [
//        hwnd aParent := theParent hwnd.
//        
//        int Styles := 0.
//        Styles := (WS_CHILD || CBS_DROPDOWNLIST).
//        $self $updateStyles vint:Styles.
//                
//        int anX := 0.
//        int anY := 0.
//        int aWidth := 0.
//        int aHeight := 0.
//        
//        theRegion read vint:anX vint:anY vint:aWidth vint:aHeight.
//        
//        theHandle := WindowHandle new int:0
//                        wide:ComboboxClass wide:emptyWideLiteralValue int:Styles
//                        int:anX int:anY int:aWidth int:aHeight 
//                        hwnd:aParent hinstance:CurrentInstance object:$nil.
//                        
//        $self $loadStrings
//    ]
//}
//
//// --- Paintbox ---
//
//class PaintboxListener :: WindowCallback
//{
//    paintbox theOwner.
//    
//    constructor new paintbox:anOwner
//    [
//        theOwner := anOwner.
//    ]
//    
//    onpaint hwnd:aControl vint:aResult
//    [
//        theOwner $paint.
//        
//        aResult int := 0.
//    ]
//    
//    onsize hwnd:aControl int:width int:height vint:aResult
//    [
//        theOwner $resize hwnd:aControl int:width int:height.
//        
//        aResult int := 0.
//    ]
//    
//    ondestroy hwnd:aControl vint:aResult
//    [
//        theOwner $destroy.
//        
//        $super ondestroy hwnd:aControl vint:aResult
//    ]
//}
//
//limited closure class PaintAction
//{
//    closure(object sender, canvas canvas) []
//}
//
//limited class Paintbox :: BaseWinControl, icontrolof<paintbox>
//{
//    canvas    theCanvas.
//    paintfunc thePaint.
//    
//    constructor new
//        <= new;
//    [
//        theCanvas := Canvas new
//    ]
//
//    set onPaint:aFunction
//    [
//        thePaint := aFunction paintfunc.
//    ]
//   
//    $destroy
//    [
//        theCanvas free
//    ]
// 
//    $paint
//    [
//        win_paintstruct struct := PAINTSTRUCT new.
//        hdc dc := 0.
//        
//        struct begin hwnd:theHandle.
//        struct read vhdc:dc.
//        
//        theCanvas open hdc:dc.
//        
//        ifnot($nil == thePaint)
//            [ thePaint($self, theCanvas) ].
//        
//        theCanvas close.
//
//        struct end hwnd:theHandle
//    ]
// 
//    stacksafe $resize hwnd:aControl int:aWidth int:aHeight
//    [
//    ]
// 
//    $resize int:aWidth int:aHeight
//    [
//        $self $resize hwnd:theHandle int:aWidth int:aHeight.
//    ]
// 
//    $createHandle
//    [
//        hwnd aParent := theParent hwnd.
//
//        int Styles := WS_CHILD.
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
//        theHandle := WindowHandle new int:WS_EX_TRANSPARENT
//                        wide:PaintboxClass wide:(theCaption wide) int:Styles
//                        int:anX int:anY int:aWidth int:aHeight 
//                        hwnd:aParent hinstance:CurrentInstance object:(PaintboxListener new paintbox:$self)
//    ]
//}
//
//class Imagebox :: Paintbox
//{
//    hbitmap  theImage.
//    hdc      theImageDC.
//    bool     theNeedToRefresh.
//    
//    constructor new
//        <= new;
//    [
//        theNeedToRefresh := false
//    ]
//
//    $destroy
//    [
//        $super $destroy.
//        
//        theImage free.
//        theImageDC free
//    ]
//    
//    $paint
//    [
//        if(theNeedToRefresh)
//            [ $self refresh ].
//        
//        win_paintstruct struct := PAINTSTRUCT new.
//        
//        struct begin hwnd:theHandle.
//        
//        int aWidth := 0.
//        int aHeight := 0.
//        theRegion readDimensionTo vint:aWidth vint:aHeight.
//        
//        hdc dc := 0.
//        struct read vhdc:dc.
//        
//        dc copy int:0 int:0 int:aWidth int:aHeight hdc:theImageDC int:0 int:0 int:SRCCOPY.
////        int err := system'external'KERNEL32 GetLastError.
//                
//        struct end hwnd:theHandle.
//    ]
// 
//    refresh
//    [
//        if($nil == theHandle)
//            [ ^ $self ].
//        
//        ifnot($nil == thePaint)
//            [ thePaint($self,theCanvas) ].
//
//        $super refresh.
//        theNeedToRefresh := false
//    ]
// 
//    $resize hwnd:aHandle int:aWidth int:aHeight
//    [
//        ifnot($nil == theImage)
//            [
//                theImage free.
//                theImageDC free
//            ].
//            
//        hdc dc := 0.
//        aHandle read vhdc:dc.
//            
//        theImage := HBITMAP new hdc:dc int:aWidth int:aHeight.
//        theImageDC := HDC newCompatible hdc:dc.
//        theImage select hdc:theImageDC.
//
//        theCanvas open hdc:theImageDC.
//
//        theNeedToRefresh := true
//    ]
//}
//
//// --- BasePanel ---
//
//class BasePanel :: BaseWinContainer
//{
//    frame theFrame.
//    
//    constructor new
//        <= new;
//    [
//        theFrame := Frame new.
//        
//        // NOTE : a frame should be the first element
//        theControls append:theFrame.
//    ]
//    
//    setDimension int:aWidth int:aHeight
//    [
//        $super setDimension int:aWidth int:aHeight.
//        
//        theFrame setDimension int:aWidth int:aHeight.
//    ]
//    
//    set caption:aCaption
//    [
//        theFrame set caption:aCaption.
//    ]
//    
//    $resize int:aWidth int:aHeight
//    [
//        dimension aSize := Dimension new.
//        aSize write int:aWidth int:aHeight.
//        
//        theFrame setDimension int:aWidth int:aHeight
//    ]    
//}
//
//// --- Panel ---
//
//class PanelListener :: WindowCallback
//{
//    panel theOwner.
//    
//    constructor new panel:anOwner
//    [
//        theOwner := anOwner
//    ]
//    
//    onsize hwnd:aControl int:width int:height vint:aResult
//    [
//        theOwner $resize int:width int:height.
//        
//        aResult int := 0.
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
//        $super onchange hwnd:aHandle vint:aResult.
//    ]
//}
//
//sealed class Panel :: BasePanel, icontrolof<panel>
//{
//    constructor new
//        <= new;
//    [
//    ]
//    
//    controls = theControls.
//            
//    $createHandle
//    [
//        hwnd aParent := theParent hwnd.
//
//        int Styles := WS_CHILD || WS_CLIPSIBLINGS.
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
//        theHandle := WindowHandle new int:WS_EX_CONTROLPARENT
//                        wide:PanelClass wide:(theCaption wide) int:Styles
//                        int:anX int:anY int:aWidth int:aHeight 
//                        hwnd:aParent hinstance:CurrentInstance object:(PanelListener new panel:$self)
//    ]
//}
//
//// --- RadioButtonGroup ---
//
//class RadioButtonGroupListener :: WindowCallback
//{
//    radiogroup theOwner.
//    
//    onclick hwnd:aHandle vint:aResult
//    [
//        theOwner $onClick hwnd:aHandle.
//        
//        aResult int := 0.
//    ]
//    
//    onsize hwnd:aControl int:width int:height vint:aResult
//    [
//        theOwner $resize int:width int:height.
//        
//        aResult int := 0.
//    ]
//    
//    constructor new radiogroup:anOwner
//    [
//        theOwner := anOwner
//    ]
//}
//
//sealed class RadioButtonGroup :: BasePanel, icontrolof<radiogroup>
//{
//    int   theSelected.
//    func1 theIndexChanged.
//    
//    constructor new
//        <= new;
//    [
//        theSelected := -1
//    ]
//
//    selected_index = theSelected.
//
//    $setChecked : aValue
//    [
//        int anIndex := -1. // !! to skip a frame
//        
//        var aCurrent := $nil.
//        enumerator it := theControls enumerator.
//        while (it next)
//        [ 
//            aCurrent := it get.
//        
//            if(theSelected == anIndex)
//                [
//                    aCurrent control_object; set checked:aValue.
//                    
//                    ^ $self
//                ].
//                
//            anIndex := anIndex + 1  
//        ]
//    ]
//
//    set selected_index : anIndex
//    [
//        ifnot(-1 == theSelected)
//            [ $self $setChecked:false ].
//        
//        theSelected := anIndex int.
//        
//        $self $setChecked:true.
//        
//        if($nil != theIndexChanged)
//            [ theIndexChanged eval:$self ]
//    ]
//    
//    set onIndexChanged:aFunction
//    [
//        theIndexChanged := aFunction func1.
//    ]
//
//    items =
//    {
//        append : anItem
//        [
//            theControls append:(RadioButton new; set caption:anItem).
//            
//            $self $resize
//        ]
//    }.
//
//    setDimension int:aWidth int:aHeight
//    [
//        $super setDimension int:aWidth int:aHeight.
//        
//        $self $resize
//    ]
//
//    stacksafe $onClick hwnd:aHandle
//    [
//        int anIndex := -1. // !! to skip a frame
//        
//        var aRetVal := $nil.
//        control aCurrent := $nil.
//        enumerator it := theControls enumerator.
//        while(it next)
//        [ 
//            aCurrent := it get; control.
//        
//            aRetVal := aCurrent retrieve hwnd:aHandle.
//            if($nil != aRetVal) 
//            [
//                theSelected := IntNumber new int:anIndex.
//                
//                if($nil != theIndexChanged)
//                    [ theIndexChanged eval:$self ].
//                    
//                ^ $self
//            ].
//                
//            anIndex := anIndex + 1
//        ]
//    ]
//
//    $resize
//    [
//        int aWidth := 0.
//        int aHeight := 0.
//        theRegion readDimensionTo vint:aWidth vint:aHeight.
//        
//        $self $resize int:aWidth int:aHeight
//    ]
//            
//    $resize int:aWidth int:aHeight
//    [
//        control aCurrent := $nil.
//        enumerator it := theControls enumerator.
//        int i := 0.
//        
//        it next. // NOTE : skip the frame
//        
//        while (it next)
//        [ 
//            aCurrent := it get; control.
//        
//            aCurrent setLocation int:4 int:(18 + i * 20).
//            aCurrent setDimension int:(aWidth - 6) int:20.
//        
//            i :=  i + 1
//        ].
//        
//        $super $resize int:aWidth int:aHeight
//    ]
//    
//    $createHandle
//    [
//        hwnd aParent := theParent hwnd.
//
//        int Styles := WS_CHILD || WS_CLIPSIBLINGS.
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
//        theHandle := WindowHandle new int:WS_EX_CONTROLPARENT
//                        wide:PanelClass wide:(theCaption wide) int:Styles
//                        int:anX int:anY int:aWidth int:aHeight 
//                        hwnd:aParent hinstance:CurrentInstance object:(RadioButtonGroupListener new radiogroup:$self).
//                        
//        if((theSelected != -1)and:($nil != theIndexChanged))
//            [ theIndexChanged eval:$self ]
//    ]
//}
//
//// --- ImageList ---
//
//sealed class ImageList
//{
//    dimension theSize.
//    object    theParent.
//    object    theImages.
//    int       theIndex.
// 
//    constructor new
//    [
//        theIndex := 0.
//        theImages := List new.
//        
//        theSize :=  Dimension new width:50 height:50.
//    ]
// 
//    assign : aControl
//       <= assign paintbox:(aControl paintbox).
// 
//    assign paintbox:aPaintbox
//    [
//        aPaintbox set onPaint:(:sender:aCanvas)<object,canvas>
//        [
//            int aWidth := 0.
//            int aHeight := 0.
//            theSize dimension; read vint:aWidth vint:aHeight.
//            
//            var anImage := theImages getAt int:(theIndex int).
//            
//            aCanvas write image:(anImage image) int:0 int:0 int:aWidth int:aHeight
//        ].
//    ]
//
//    selected_index = theIndex.
//
//    set selected_index : anIndex
//    [
//        theIndex := anIndex int.
//    ] 
//   
//    control = IControl::
//    {
//        control_object = $self.
// 
//        open [ $self open ]
//          
//        close [ $self close ]
//          
//        retrieve hwnd:aHandle = $nil.
//    
//        setDimension int:aWidth int:aHeight [ $self setDimension int:aWidth int:aHeight ]
//    }.
// 
//    $set parent:aParent
//    [
//        theParent := aParent.
//    ]
//          
//    dimension = theSize.
//    
//    stacksafe setDimension int:aWidth int:aHeight
//    [
//        theSize write int:aWidth int:aHeight.
//    ]
//    
//    set width:aWidth height:aHeight
//    [
//        theSize set width:aWidth.
//        theSize set height:aHeight.
//    ]
//    
//    append path:aPath
//    [        
//        theImages append:(Image new literal:aPath dimension:theSize).
//    ]
//    
//    open
//    [
//        hdc aParentDC := theParent hwnd; hdc.        
//        
//        enumerator it := theImages enumerator.
//        while(it next) [
//            it get; image; open hdc:aParentDC
//        ].
//    ]
//    
//    close
//    [
//        enumerator it := theImages enumerator.
//        while(it next) [
//            it get; free
//        ].
//    ]    
//}
//
//// --- DockingBox ---
//
//sealed class DockingBox
//{
//    object  theParent.
//    control theLeft.
//    control theRight.
//    control theTop.
//    control theBottom.
//    control theClient.
//
//    constructor new
//    [
//    ]
// 
//    assign client:aControl
//    [
//        theClient := aControl control.
//    ]
//
//    assign left:aControl
//    [
//        theLeft := aControl control.
//    ]
//
//    assign right:aControl
//    [
//        theRight := aControl control.
//    ]
//
//    assign top:aControl
//    [
//        theTop := aControl control.
//    ]
//
//    assign bottom:aControl
//    [
//        theBottom := aControl control.
//    ]
//   
//    control = IControl::
//    {
//        control_object = $self.
// 
//        open [ $self open ]
//          
//        close [ $self close ]
//          
//        retrieve hwnd:aHandle = $nil.
//    }.
// 
//    $set parent:aParent
//    [
//        theParent := aParent.
//        
//        theParent set onResize: (:control)
//        [
//            self refresh
//        ].
//    ]
//
//    open
//    [
//        $self refresh.
//    ]
//     
//    close
//    [
//    ]        
//              
//    refresh
//    [
//        int tmp := 0.
//        
//        dimension theSize := theParent dimension.
//
//        int height := 0.
//        int width := 0.
//        int x := 0.
//        int y := 0.
//        int aTotalWidth := 0.
//        int aTotalHeight := 0.
//        theSize read vint:aTotalWidth vint:aTotalHeight.
//
//        // Bottom
//        if($nil != theBottom)
//        [
//            theBottom readDimensionTo vint:tmp vint:height.
//            theBottom setDimension int:aTotalWidth int:height.
//            
//            aTotalHeight := aTotalHeight - height
//        ].
//        
//        // Top
//        if($nil != theTop)
//        [
//            
//            theTop readDimensionTo vint:tmp vint:height.
//            theTop setDimension int:aTotalWidth int:height.
//            
//            aTotalHeight := aTotalHeight - height.
//            
//            y := height
//        ].
//        
//        // Left
//        if($nil != theLeft)
//        [
//            theLeft readDimensionTo vint:width vint:tmp.
//            theLeft setDimension int:width int:aTotalHeight.
//            
//            aTotalWidth := aTotalWidth - width.
//            
//            x := width
//        ].
//        
//        // Right
//        if($nil != theRight)
//        [
//            theRight readDimensionTo vint:width vint:tmp.
//            theRight setDimension int:width int:aTotalHeight.
//            
//            aTotalWidth := aTotalWidth - width
//        ].
//
//        // Client
//        if($nil != theClient)
//        [
//            theClient setDimension int:aTotalWidth int:aTotalHeight.
//            theClient setLocation int:x int:y.            
//        ].
//        
//        x := x + aTotalWidth.
//        y := y + aTotalHeight.
//
//        if($nil != theRight)        
//            [ theRight setLocation int:x int:height ].
//            
//        if($nil != theBottom)
//            [ theBottom setLocation int:0 int:y ].
//    ]    
//}
