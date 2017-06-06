#import system.
#import system'collections.
#import system'drawing.
#import system'winforms.

// --- IControl ---

#class(limited) IControl
{
    #method control = $self.
    
    #method object = $nil.
    
    #method(stacksafe) retrieve &hwnd:aHandle = $nil.
    
    #method(stacksafe) readDimension &vint:aWidth &vint:aHeight []
    
    #method(stacksafe) readLocation &vint:x &vint:y []
    
    #method(stacksafe) setDimension &int:aWidth &int:aHeight []
    
    #method(stacksafe) setLocation &int:x &int:y []
    
    #method open []

    #method change []
        
    #method close []
}

#template icontrolof : type
{
    #method control = IControl
    {
        object = $owner.
        
        open [ $owner open. ]
    
        change [ $owner change. ]
    
        close [ $owner close. ]
    
        retrieve &hwnd:aHandle = $owner retrieve &hwnd:aHandle.
    
        setDimension &int:aWidth &int:aHeight [ $owner setDimension &int:aWidth &int:aHeight. ]
    
        setLocation &int:aWidth &int:aHeight [ $owner setLocation &int:aWidth &int:aHeight. ]
    
        readDimension &vint:aWidth &vint:aHeight [ $owner readDimension &vint:aWidth &vint:aHeight. ]
    
        readLocation &vint:aWidth &vint:aHeight [ $owner readLocation &vint:aWidth &vint:aHeight. ]
    }.
}

// --- BaseWindowControl ---

#class BaseWinControl :: BaseControl
{
    #field(hwnd)theHandle.

    #constructor new
        <= (new)
    [
    ]
    
    #method(stacksafe) retrieve &hwnd:aHandle
    [
        ($nil == theHandle)
            ? [ ^ $nil. ].
            
        (theHandle == aHandle)
            ? [ ^ $self. ].
            
        ^ $nil.            
    ]

    #method $createHandle []            

    #method $onCreate
    [
        theEnabled 
            ! [ theHandle setEnabled &int:0. ].
    ]

    #method click []
    
    #method change []
    
    #method close []    

    #method open
    [
        ($nil != theHandle)
            ? [ #throw InvalidOperationException new:"The form is already open". ].
            
        $self $createHandle.
        
        $self $onCreate.
    ]

    #method is &opened = ($nil != theHandle).
    
    #method hwnd = theHandle.

    #method set &visible : aValue
    [
        $super set &visible:aValue.
        
        ($nil == theHandle)
            ! [
                theVisible
                    ? [ theHandle setVisible &int:5. ]
                    ! [ theHandle setVisible &int:0. ].
            ].
    ]
                                                
    #method set &enabled : aValue
    [
        $super set &enabled:aValue.
        
        ($nil == theHandle)
            ! [
                theEnabled
                    ? [ theHandle setEnabled &int:-1. ]
                    ! [ theHandle setEnabled &int:0. ].
            ].
    ]
    
    #method set &caption : aCaption
    [
        $super set &caption:aCaption.
        
        ($nil == theHandle)
            ! [
                theHandle setCaption &wide:(aCaption wide).
            ].
    ]
    
    #method append &caption : aValue
    [    
        $self set &caption:($self caption + aValue wide).
    ]

    #method caption
    [
        ($nil == theHandle)
            ! [
                theCaption := theHandle getCaption.
            ].
            
        ^ theCaption.            
    ]

    #method setLocation &int:x &int:y
    [
        theRegion writeLocation &int:x &int:y.
        
        ($nil == theHandle)
            ! [
                theHandle setPosition &int:x &int:y &int:0 &int:0 &int:SWP_NOSIZE.
            ].
    ]
    
    #method(stacksafe) setDimension &int:aWidth &int:aHeight
    [
        theRegion writeDimension &int:aWidth &int:aHeight.
        
        ($nil == theHandle)
            ! [
                theHandle setPosition &int:0 &int:0 &int:aWidth &int:aHeight &int:SWP_NOMOVE.
            ].
    ]
    
    #method refresh
    [
        ($nil != theHandle)
            ? [ theHandle refresh. ].
    ]
    
    #method(stacksafe) $resize &int:aWidth &int:aHeight
    [
        theRegion writeDimension &int:aWidth &int:aHeight.
    ]  
}    

// --- ControlCollection ---

#class(sealed) ControlCollection :: BaseControlCollection
{
    #field theParent.
    
    #constructor new : aParent
        <= (new)
    [
        theParent := aParent.
    ]
    
    #method append : aControl
    [
        $super append:(aControl control).
        
        aControl $set &parent:theParent.
        
        (theParent is &opened)?
        [
            aControl open.
        ].
    ]
}

// --- BaseWinContainer ---

#class BaseWinContainer :: BaseWinControl
{
    #field(controls) theControls.
    #field(func1)    theResize.
    
    #constructor new
        <= (new)
    [
        theControls := ControlCollection new:$self.
    ]

    #method set &onResize:anAction
    [
        theResize := anAction func1.
    ]

    #method retrieve &hwnd:aHandle
    [
        (theHandle == aHandle)
            ? [ ^ $self. ].

        #var(control)aCurrent.
        #var(enumerator)it := theControls enumerator.
        #var aRetVal := $nil.
        #loop (it next)?
        [ 
            aCurrent := it get control.
        
            aRetVal := aCurrent retrieve &hwnd:aHandle.
            ($nil != aRetVal)
                ? [ ^ aRetVal. ].
        ].
                                    
        ^ $nil.            
    ]
        
    #method open
    [
        $super open.
        
        #var(enumerator)it := theControls enumerator.
        #loop (it next)?
            [ it get open. ].
    ]
        
    #method close
    [
        #var(enumerator)it := theControls enumerator.
        #loop (it next)?
            [ it get close. ].
    ]
    
    #method(stacksafe) $resize &int:aWidth &int:aHeight
    [
        $super $resize &int:aWidth &int:aHeight.
        
        ($nil == theResize)
            ! [ theResize eval:$self. ].
    ]  
}

// --- Frame ---

#class(sealed,icontrolof:frame) Frame :: BaseWinControl
{
    #constructor new
        <= (new)
    [
    ]
    
    #method $createHandle
    [
        #var(hwnd)aParent := theParent hwnd.
        
        #var(int)Styles := WS_CHILD || BS_GROUPBOX.
        theVisible
            ? [ Styles := Styles || WS_VISIBLE. ].
        
        #var(int)anX.
        #var(int)anY.
        #var(int)aWidth.
        #var(int)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:WS_EX_TRANSPARENT
                        &wide:ButtonClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:aParent &hinstance:CurrentInstance &object:$nil.
    ]
}

// --- Label ---

#class(limited,icontrolof:label) Label :: BaseWinControl
{
    #constructor new
        <= (new)
    [
    ]
    
    #method $createHandle
    [
        #var(hwnd)aParent := theParent hwnd.
        
        #var(int)Styles := WS_CHILD.
        theVisible
            ? [ Styles := Styles || WS_VISIBLE. ].
        
        #var(int)anX.
        #var(int)anY.
        #var(int)aWidth.
        #var(int)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:0
                        &wide:StaticClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:aParent &hinstance:CurrentInstance &object:$nil.
    ]
}

// --- Label ---

#class StaticLabel :: Label
{
    #constructor new
        <= (new)
    [
    ]
    
    #method $createHandle
    [
        #var(hwnd)aParent := theParent hwnd.
        (0 == aParent)
            ? [ #throw InvalidOperationException new:"The parent window is closed". ].
        
        #var(int)Styles := WS_CHILD || SS_SIMPLE.
        theVisible
            ? [ Styles := Styles || WS_VISIBLE. ].
        
        #var(int)anX.
        #var(int)anY.
        #var(int)aWidth.
        #var(int)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:0
                        &wide:StaticClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:aParent &hinstance:CurrentInstance &object:$nil.
    ]
}

// --- BaseEdit ---

#class BaseEdit :: BaseWinControl
{
    #field(func1)theChanged.
    
    #constructor new
        <= (new)
    [
    ]
    
    #method set &onChange:aFunction
    [
        theChanged := aFunction func1.
    ]
    
    #method wide = $self caption wide.
          
    #method literal = $self caption literal.
          
    #method value = $self caption.
    
    #method append : aValue
    [
        $self set &caption:($self caption + aValue wide).
    ]
    
    #method write : aValue
    [
        $self set &caption:(aValue wide).
    ]          
    
    #method(stacksafe) $updateStyles &vint:aRetVal
    [
        #var(int)Styles := 0.
        
        theVisible
            ? [ Styles := Styles || WS_VISIBLE. ].
        theTabStop
            ? [ Styles := Styles || WS_TABSTOP. ].
            
        aRetVal := (aRetVal || Styles).
    ]
    
    #method change 
    [
        ($nil != theChanged)
            ? [ theChanged eval:$self. ].
    ]
}

// --- Edit ---

#class(sealed,icontrolof:edit) Edit ::  BaseEdit
{
    #constructor new
        <= (new)
    [
        theTabStop := true.
    ]
    
    #method $createHandle
    [
        #var(hwnd)aParent := theParent hwnd.
        
        #var(int)Styles := WS_CHILD || WS_BORDER.
        $self $updateStyles &vint:Styles.
        
        #var(int)anX.
        #var(int)anY.
        #var(int)aWidth.
        #var(int)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:WS_EX_CLIENTEDGE
                        &wide:EditClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:aParent &hinstance:CurrentInstance &object:$nil.
    ]
}

// --- Memo ---

#class(sealed,icontrolof:memo) Memo :: BaseEdit
{
    #constructor new
        <= (new)
    [
        theTabStop := true.
    ]
 
    #method wide = $self caption wide.
          
    #method literal = $self caption literal.
    
    #method $createHandle
    [
        #var(hwnd)aParent := theParent hwnd.
        
        #var(int)Styles := WS_CHILD || WS_BORDER || ES_MULTILINE || ES_WANTRETURN || ES_AUTOHSCROLL || ES_AUTOVSCROLL || WS_VSCROLL || WS_HSCROLL.
        $self $updateStyles &vint:Styles.
        
        #var(int)anX.
        #var(int)anY.
        #var(int)aWidth.
        #var(int)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:WS_EX_CLIENTEDGE
                        &wide:EditClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:aParent &hinstance:CurrentInstance &object:$nil.
    ]
}

// --- BaseButton ---

#class BaseButton :: BaseWinControl
{
    #field(func1)theClick.
    
    #constructor new
        <= (new)
    [
        theTabStop := true.
    ]
    
    #method set &onClick:aFunction
    [
        theClick := aFunction func1.
    ]
    
    #method(stacksafe) $updateStyles &vint:aRetVal
    [
        #var(int)Styles := 0.
        
        theVisible
            ? [ Styles := Styles || WS_VISIBLE. ].
        theTabStop
            ? [ Styles := Styles || WS_TABSTOP. ].
            
        aRetVal := (aRetVal || Styles).
    ]
    
    #method click 
    [
        ($nil != theClick)
            ? [ theClick eval:$self. ].
    ]
}

// --- Button ---

#class(sealed,icontrolof:button) Button :: BaseButton
{
    #constructor new
        <= (new)
    [
    ]
    
    #method $createHandle
    [
        #var(hwnd)aParent := theParent hwnd.
        
        #var(int)Styles := 0.
        Styles := (WS_CHILD || BS_PUSHBUTTON).
        $self $updateStyles &vint:Styles.
                
        #var(int)anX.
        #var(int)anY.
        #var(int)aWidth.
        #var(int)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:0
                        &wide:ButtonClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:aParent &hinstance:CurrentInstance &object:$nil.
    ]
}

// --- RadioButton ---

#class(limited,icontrolof:radiobutton) RadioButton :: BaseButton
{
    #field(bool) theChecked.
    
    #constructor new
        <= (new)
    [
        theChecked := false.
    ]
    
    #method is &checked
    [
        ($nil == theHandle)
            ! [ theChecked := theHandle isChecked. ].
            
        ^ theChecked.            
    ]
 
    #method set &checked:aValue
    [
        theChecked := aValue bool.
        
        ($nil == theHandle)!
        [
            theChecked
                ? [ theHandle setChecked &int:BST_CHECKED. ]
                ! [ theHandle setChecked &int:BST_UNCHECKED. ].
        ].
    ]

    #method $createHandle
    [
        #var(hwnd)aParent := theParent hwnd.
        
        #var(int)Styles := 0.
        Styles := (WS_CHILD || BS_AUTORADIOBUTTON).
        $self $updateStyles &vint:Styles.
                
        #var(int)anX.
        #var(int)anY.
        #var(int)aWidth.
        #var(int)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:0
                        &wide:ButtonClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:aParent &hinstance:CurrentInstance &object:$nil.
                        
        theChecked
            ? [ $self set &checked:true. ].
    ]
}

// --- BaseList ---

#class BaseList :: BaseWinControl
{
    #field(array_list)theList.
    #field(index_changed,event:index_changed)theChangeAction.
    
    #constructor new
        <= (new)
    [
        theList := ArrayList new.
    ]
    
    #method items =
    {
        append : anObject &as:aCaption
        [
            theList append:(KeyValue new &key:aCaption &value:anObject).
            
            self $appendItem &wide:(aCaption wide).
        ]
    }.
    
    #method(stacksafe) $updateStyles &vint:aRetVal
    [
        #var(int)Styles := 0.
        
        theVisible
            ? [ Styles := Styles || WS_VISIBLE. ].
        theTabStop
            ? [ Styles := Styles || WS_TABSTOP. ].
            
        aRetVal := (aRetVal || Styles).
    ]
}

// --- Combobox ---

#class(sealed,icontrolof:combobox) Combobox :: BaseList
{
    #constructor new
        <= (new)
    [
    ]
    
    #method(stacksafe,outnprop:selected_index) readSelectedIndex &vint:retVal
    [
        #var(int)index := system'external'USER32 SendMessageW &int:theHandle &int:CB_GETCURSEL &int:0 &int:0.
        
        retVal := index.
    ]
    
    #method(stacksafe,innprop:selected_index) setSelectedIndex &int:index
    [
        system'external'USER32 SendMessageW &int:theHandle &int:CB_SETCURSEL &int:index &int:0.
        
        $self change.
    ]
    
    #method selected 
    [
        #var(int)index := $self selected_index.
            
        ^ theList getAt &int:index.
    ]
    
    #method change 
    [
        ($nil != theChangeAction)?
        [
            #var(int)index := $self selected_index.
            
            theChangeAction onIndexChange:$self &int:index.
        ].
    ]
    
    #method $appendItem &wide:aCaption
    [
        ($nil == theHandle)
            ? [ ^ $self. ].
        
        theHandle sendMessage &int:CB_ADDSTRING &int:0 &wide:aCaption.
    ]
    
    #method $loadStrings
    [
        #var(int)aLength := theList length.
        
        #var(int)i := 0.
        #loop (i < aLength)?
        [
            #var aKeyValue := theList getAt &int:i.
            
            theHandle sendMessage &int:CB_ADDSTRING &int:0 &wide:(aKeyValue key wide).
            
            i += 1.
        ].
    ]
    
    #method $createHandle
    [
        #var(hwnd)aParent := theParent hwnd.
        
        #var(int)Styles := 0.
        Styles := (WS_CHILD || CBS_DROPDOWNLIST).
        $self $updateStyles &vint:Styles.
                
        #var(int)anX.
        #var(int)anY.
        #var(int)aWidth.
        #var(int)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:0
                        &wide:ComboboxClass &wide:emptyWideLiteralValue &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:aParent &hinstance:CurrentInstance &object:$nil.
                        
        $self $loadStrings.                        
    ]
}

// --- Paintbox ---

#class PaintboxListener :: WindowCallback
{
    #field(paintbox)theOwner.
    
    #constructor new &paintbox:anOwner
    [
        theOwner := anOwner.
    ]
    
    #method onpaint &hwnd:aControl &vint:aResult
    [
        theOwner $paint.
        
        aResult := 0.
    ]
    
    #method onsize &hwnd:aControl &int:width &int:height &vint:aResult
    [
        theOwner $resize &hwnd:aControl &int:width &int:height.
        
        aResult := 0.
    ]
    
    #method ondestroy &hwnd:aControl &vint:aResult
    [
        theOwner $destroy.
        
        $super ondestroy &hwnd:aControl &vint:aResult.
    ]
}

#class(limited) PaintAction
{
    #method(action)eval:sender &canvas:canvas []
}

#class(limited,icontrolof:paintbox) Paintbox :: BaseWinControl
{
    #field(canvas)    theCanvas.
    #field(paintfunc) thePaint.
    
    #constructor new
        <= (new)
    [
        theCanvas := Canvas new.
    ]

    #method set &onPaint:aFunction
    [
        thePaint := aFunction paintfunc.
    ]
   
    #method $destroy
    [
        theCanvas free.
    ]
 
    #method $paint
    [
        #var(win_paintstruct)struct.
        #var(hdc)dc.
        
        struct begin &hwnd:theHandle.
        struct read &vhdc:dc.
        
        theCanvas open &hdc:dc.
        
        ($nil == thePaint)
            ! [ thePaint eval:$self &canvas:theCanvas. ].
        
        theCanvas close.

        struct end &hwnd:theHandle.
    ]
 
    #method(stacksafe) $resize &hwnd:aControl &int:aWidth &int:aHeight
    [
    ]
 
    #method $resize &int:aWidth &int:aHeight
    [
        $self $resize &hwnd:theHandle &int:aWidth &int:aHeight.
    ]
 
    #method $createHandle
    [
        #var(hwnd)aParent := theParent hwnd.

        #var(int)Styles := WS_CHILD.
        theVisible
            ? [ Styles := Styles || WS_VISIBLE. ].
                        
        #var(int)anX.
        #var(int)anY.
        #var(int)aWidth.
        #var(int)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:WS_EX_TRANSPARENT
                        &wide:PaintboxClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:aParent &hinstance:CurrentInstance &object:(PaintboxListener new &paintbox:$self).
    ]
}

#class Imagebox :: Paintbox
{
    #field(hbitmap)  theImage.
    #field(hdc)      theImageDC.
    #field(bool)     theNeedToRefresh.
    
    #constructor new
        <= (new)
    [
        theNeedToRefresh := false.
    ]

    #method $destroy
    [
        $super $destroy.
        
        theImage free.
        theImageDC free.
    ]
    
    #method $paint
    [
        (theNeedToRefresh)
            ? [ $self refresh. ].
        
        #var(win_paintstruct)struct.
        
        struct begin &hwnd:theHandle.
        
        #var(int)aWidth.
        #var(int)aHeight.
        theRegion readDimension &vint:aWidth &vint:aHeight.
        
        #var(hdc)dc.
        struct read &vhdc:dc.
        
        dc copy &int:0 &int:0 &int:aWidth &int:aHeight &hdc:theImageDC &int:0 &int:0 &int:SRCCOPY.
        #var(int)err := system'external'KERNEL32 GetLastError.
                
        struct end &hwnd:theHandle.
    ]
 
    #method refresh
    [
        ($nil == theHandle)
            ? [ ^ $self. ].
        
        ($nil == thePaint)
            ! [ thePaint eval:$self &canvas:theCanvas. ].

        $super refresh.
        theNeedToRefresh := false.
    ]
 
    #method $resize &hwnd:aHandle &int:aWidth &int:aHeight
    [
        ($nil == theImage)
            ! [
                theImage free.
                theImageDC free.
            ].
            
        #var(hdc)dc.
        aHandle read &vhdc:dc.
            
        theImage := HBITMAP new &hdc:dc &int:aWidth &int:aHeight.
        theImageDC := HDC newCompatible &hdc:dc.
        theImage select &hdc:theImageDC.

        theCanvas open &hdc:theImageDC.

        theNeedToRefresh := true.
    ]
}

// --- BasePanel ---

#class BasePanel :: BaseWinContainer
{
    #field(frame) theFrame.
    
    #constructor new
        <= (new)
    [
        theFrame := Frame new.
        
        // NOTE : a frame should be the first element
        theControls append:theFrame.
    ]
    
    #method setDimension &int:aWidth &int:aHeight
    [
        $super setDimension &int:aWidth &int:aHeight.
        
        theFrame setDimension &int:aWidth &int:aHeight.
    ]
    
    #method set &caption:aCaption
    [
        theFrame set &caption:aCaption.
    ]
    
    #method $resize &int:aWidth &int:aHeight
    [
        #var(dimension)aSize.
        aSize write &int:aWidth &int:aHeight.
        
        theFrame setDimension &int:aWidth &int:aHeight.
    ]    
}

// --- Panel ---

#class PanelListener :: WindowCallback
{
    #field(panel)theOwner.
    
    #constructor new &panel:anOwner
    [
        theOwner := anOwner.
    ]
    
    #method onsize &hwnd:aControl &int:width &int:height &vint:aResult
    [
        theOwner $resize &int:width &int:height.
        
        aResult := 0.
    ]
    
    #method onclick &hwnd:aHandle &vint:aResult
    [
        #var aControl := theOwner retrieve &hwnd:aHandle.
        ($nil != aControl)
            ? [ aControl click. ].
        
        $super onclick &hwnd:aHandle &vint:aResult.
    ]
    
    #method onchange &hwnd:aHandle &vint:aResult
    [
        #var aControl := theOwner retrieve &hwnd:aHandle.
        ($nil != aControl)
            ? [ aControl change. ].
        
        $super onchange &hwnd:aHandle &vint:aResult.
    ]
}

#class(sealed,icontrolof:panel) Panel :: BasePanel
{
    #constructor new
        <= (new)
    [
    ]
    
    #method controls = theControls.
            
    #method $createHandle
    [
        #var(hwnd)aParent := theParent hwnd.

        #var(int)Styles := WS_CHILD || WS_CLIPSIBLINGS.
        theVisible
            ? [ Styles := Styles || WS_VISIBLE. ].
                        
        #var(int)anX.
        #var(int)anY.
        #var(int)aWidth.
        #var(int)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:WS_EX_CONTROLPARENT
                        &wide:PanelClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:aParent &hinstance:CurrentInstance &object:(PanelListener new &panel:$self).
    ]
}

// --- RadioButtonGroup ---

#class RadioButtonGroupListener :: WindowCallback
{
    #field(radiogroup)theOwner.
    
    #method onclick &hwnd:aHandle &vint:aResult
    [
        theOwner $onClick &hwnd:aHandle.
        
        aResult := 0.
    ]
    
    #method onsize &hwnd:aControl &int:width &int:height &vint:aResult
    [
        theOwner $resize &int:width &int:height.
        
        aResult := 0.
    ]
    
    #constructor new &radiogroup:anOwner
    [
        theOwner := anOwner.
    ]
}

#class(sealed,icontrolof:radiogroup) RadioButtonGroup :: BasePanel
{
    #field(int)theSelected.
    #field(func1)theIndexChanged.
    
    #constructor new
        <= (new)
    [
        theSelected := -1.
    ]

    #method selected_index = theSelected.

    #method $setChecked : aValue
    [
        #var(int)anIndex := -1. // !! to skip a frame
        
        #var aCurrent.
        #var(enumerator)it := theControls enumerator.
        #loop (it next)?
        [ 
            aCurrent := it get.
        
            (theSelected == anIndex)
                ? [
                    aCurrent object set &checked:aValue.
                    
                    ^ $self.
                ].
                
            anIndex := anIndex + 1.                
        ].
    ]

    #method set &selected_index : anIndex
    [
        (-1 == theSelected)
            ! [ $self $setChecked:false. ].
        
        theSelected := anIndex int.
        
        $self $setChecked:true.
        
        ($nil != theIndexChanged)?
            [ theIndexChanged eval:$self. ].
    ]
    
    #method set &onIndexChanged:aFunction
    [
        theIndexChanged := aFunction func1.
    ]

    #method items =
    {
        append : anItem
        [
            theControls append:(RadioButton new set &caption:anItem).
            
            self $resize.
        ]
    }.

    #method setDimension &int:aWidth &int:aHeight
    [
        $super setDimension &int:aWidth &int:aHeight.
        
        $self $resize.
    ]

    #method(stacksafe) $onClick &hwnd:aHandle
    [
        #var(int)anIndex := -1. // !! to skip a frame
        
        #var aRetVal := $nil.
        #var(control)aCurrent.
        #var(enumerator)it := theControls enumerator.
        #loop (it next)?
        [ 
            aCurrent := it get control.
        
            aRetVal := aCurrent retrieve &hwnd:aHandle.
            ($nil != aRetVal) ?
            [
                theSelected := IntNumber new &int:anIndex.
                
                ($nil != theIndexChanged)?
                    [ theIndexChanged eval:$self. ].
                    
                ^ $self.                    
            ].
                
            anIndex := anIndex + 1.                
        ].
    ]

    #method $resize
    [
        #var(int)aWidth.
        #var(int)aHeight.
        theRegion readDimension &vint:aWidth &vint:aHeight.
        
        $self $resize &int:aWidth &int:aHeight.
    ]
            
    #method $resize &int:aWidth &int:aHeight
    [
        #var(control)aCurrent.
        #var(enumerator)it := theControls enumerator.
        #var(int)i := 0.
        
        it next. // NOTE : skip the frame
        
        #loop (it next)?
        [ 
            aCurrent := it get control.
        
            aCurrent setLocation &int:4 &int:(18 + i * 20).
            aCurrent setDimension &int:(aWidth - 6) &int:20.
        
            i :=  i + 1.
        ].
        
        $super $resize &int:aWidth &int:aHeight.
    ]
    
    #method $createHandle
    [
        #var(hwnd)aParent := theParent hwnd.

        #var(int)Styles := WS_CHILD || WS_CLIPSIBLINGS.
        theVisible
            ? [ Styles := Styles || WS_VISIBLE. ].
                        
        #var(int)anX.
        #var(int)anY.
        #var(int)aWidth.
        #var(int)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:WS_EX_CONTROLPARENT
                        &wide:PanelClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:aParent &hinstance:CurrentInstance &object:(RadioButtonGroupListener new &radiogroup:$self).
                        
        ((theSelected != -1)and:($nil != theIndexChanged))?
            [ theIndexChanged eval:$self. ].
    ]
}

// --- ImageList ---

#class(sealed) ImageList
{
    #field(dimension)theSize.
    #field theParent.
    #field theImages.
    #field(int)theIndex.
 
    #constructor new
    [
        theIndex := 0.
        theImages := List new.
        
        theSize :=  Dimension new &width:50 &height:50.
    ]
 
    #method assign : aControl
       <= assign &paintbox:(aControl paintbox).
 
    #method assign &paintbox:aPaintbox
    [
        aPaintbox set &onPaint:(:sender &canvas:aCanvas)
        [
            #var(int)aWidth.
            #var(int)aHeight.
            theSize dimension read &vint:aWidth &vint:aHeight.
            
            #var anImage := theImages getAt &int:(theIndex int).
            
            aCanvas write &image:(anImage image) &int:0 &int:0 &int:aWidth &int:aHeight.
        ].
    ]

    #method selected_index = theIndex.

    #method set &selected_index : anIndex
    [
        theIndex := anIndex int.
    ] 
   
    #method control = IControl 
    {
        #method object = $owner.
 
        #method open [ $owner open. ]
          
        #method close [ $owner close. ]
          
        #method retrieve &hwnd:aHandle = $nil.
    
        #method setDimension &int:aWidth &int:aHeight [ $owner setDimension &int:aWidth &int:aHeight. ]
    }.
 
    #method $set &parent:aParent
    [
        theParent := aParent.
    ]
          
    #method dimension = theSize.
    
    #method(stacksafe) setDimension &int:aWidth &int:aHeight
    [
        theSize write &int:aWidth &int:aHeight.
    ]
    
    #method set &width:aWidth &height:aHeight
    [
        theSize set &width:aWidth.
        theSize set &height:aHeight.
    ]
    
    #method append &path:aPath
    [        
        theImages append:(Image new &path:aPath &dimension:theSize).
    ]
    
    #method open
    [
        #var(hdc)aParentDC := theParent hwnd hdc.        
        
        #var(enumerator)it := theImages enumerator.
        #loop (it next)? [
            it get image open &hdc:aParentDC.
        ].
    ]
    
    #method close
    [
        #var(enumerator)it := theImages enumerator.
        #loop (it next)? [
            it get free.
        ].
    ]    
}

// --- DockingBox ---

#class(sealed) DockingBox
{
    #field theParent.
    #field(control)theLeft.
    #field(control)theRight.
    #field(control)theTop.
    #field(control)theBottom.
    #field(control)theClient.

    #constructor new
    [
    ]
 
    #method assign &client:aControl
    [
        theClient := aControl control.
    ]

    #method assign &left:aControl
    [
        theLeft := aControl control.
    ]

    #method assign &right:aControl
    [
        theRight := aControl control.
    ]

    #method assign &top:aControl
    [
        theTop := aControl control.
    ]

    #method assign &bottom:aControl
    [
        theBottom := aControl control.
    ]
   
    #method control = IControl 
    {
        #method object = $owner.
 
        #method open [ $owner open. ]
          
        #method close [ $owner close. ]
          
        #method retrieve &hwnd:aHandle = $nil.
    }.
 
    #method $set &parent:aParent
    [
        theParent := aParent.
        
        theParent set &onResize: control 
        [
            self refresh.
        ].
    ]

    #method open
    [
        $self refresh.
    ]
     
    #method close
    [
    ]        
              
    #method refresh
    [
        #var(int)tmp.
        
        #var(dimension)theSize := theParent dimension.

        #var(int)height := 0.
        #var(int)width := 0.
        #var(int)x := 0.
        #var(int)y := 0.
        #var(int)aTotalWidth.
        #var(int)aTotalHeight.
        theSize read &vint:aTotalWidth &vint:aTotalHeight.

        // Bottom
        ($nil != theBottom)?
        [
            theBottom readDimension &vint:tmp &vint:height.
            theBottom setDimension &int:aTotalWidth &int:height.
            
            aTotalHeight := aTotalHeight - height.
        ].
        
        // Top
        ($nil != theTop)?
        [
            
            theTop readDimension &vint:tmp &vint:height.
            theTop setDimension &int:aTotalWidth &int:height.
            
            aTotalHeight := aTotalHeight - height.
            
            y := height.
        ].
        
        // Left
        ($nil != theLeft)?
        [
            theLeft readDimension &vint:width &vint:tmp.
            theLeft setDimension &int:width &int:aTotalHeight.
            
            aTotalWidth := aTotalWidth - width.
            
            x := width.
        ].
        
        // Right
        ($nil != theRight)?
        [
            theRight readDimension &vint:width &vint:tmp.
            theRight setDimension &int:width &int:aTotalHeight.
            
            aTotalWidth := aTotalWidth - width.
        ].

        // Client
        ($nil != theClient)?
        [
            theClient setDimension &int:aTotalWidth &int:aTotalHeight.
            theClient setLocation &int:x &int:y.            
        ].
        
        x := x + aTotalWidth.
        y := y + aTotalHeight.

        ($nil != theRight)        
            ? [ theRight setLocation &int:x &int:height. ].
            
        ($nil != theBottom)
            ? [ theBottom setLocation &int:0 &int:y. ].
    ]    
}
