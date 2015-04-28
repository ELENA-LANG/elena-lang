#define system.
#define system'winforms.
#define system'collections.

// --- IControl ---

#class(limited) IControl
{
    #method control = $self.
    
    #method object = $nil.
    
    #method retrieve &hwnd:aHandle = $nil.
    
    #method set &dimension:aDimension []
    
    #method set &location:aPoint []
    
    #method open []
    
    #method close []
}

// --- BaseWindowControl ---

#class BaseWinControl :: BaseControl
{
    #field(type:hwnd)theHandle.

    #method retrieve &hwnd:aHandle
    [
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

    #method open
    [
        ($nil != theHandle)
            ? [ #throw InvalidOperationException new &message:"The form is already open". ].
            
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

    #method set &location:aLocation
    [
        $super set &location:aLocation.
        
        ($nil == theHandle)
            ! [
                #var(type:vint)x.
                #var(type:vint)y.
                aLocation read &vint:x &vint:y.
        
                theHandle setPosition &int:x &int:y &int:0 &int:0 &int_attributes:SWP_NOSIZE.
            ].
    ]
    
    #method set &dimension:aSize
    [
        $super set &dimension:aSize.
        
        ($nil == theHandle)
            ! [
                #var(type:vint)aWidth.
                #var(type:vint)aHeight.
                aSize read &vint:aWidth &vint:aHeight.
        
                theHandle setPosition &int:0 &int:0 &int:aWidth &int:aHeight &int_attributes:SWP_NOMOVE.
            ].
    ]
    
    #method refresh
    [
        ($nil != theHandle)
            ? [ theHandle refresh. ].
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
    #field(type:controls) theControls.
    
    #constructor new
        <= (new)
    [
        theControls := ControlCollection new:$self.
    ]

    #method retrieve &hwnd:aHandle
    [
        (theHandle == aHandle)
            ? [ ^ $self. ].

        #var(type:control)aCurrent.
        #var(type:enumerator)it := theControls enumerator.
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
        
        #var(type:enumerator)it := theControls enumerator.
        #loop (it next)?
            [ it get open. ].
    ]
        
    #method close
    [
        #var(type:enumerator)it := theControls enumerator.
        #loop (it next)?
            [ it get close. ].
    ]
}

// --- Frame ---

#class IFrame :: IControl
{
    #field(type:frame)theFrame.
    
    #constructor new &frame:aFrame
    [
        theFrame := aFrame.
    ]
    
    #method object = theFrame.
    
    #method open [ theFrame open. ]
          
    #method retrieve &hwnd:aHandle = theFrame retrieve &hwnd:aHandle.
    
    #method set &dimension:aDimension [ theFrame set &dimension:aDimension. ]
    
    #method set &location:aPoint [ theFrame set &location:aPoint. ]
}

#class(sealed) Frame :: BaseWinControl
{
    #method control = IFrame new &frame:$self.
    
    #method frame = $self.
    
    #method $createHandle
    [
        #var(type:hwnd)aParent := theParent hwnd.
        
        #var(type:int)Styles := WS_CHILD || BS_GROUPBOX.
        theVisible
            ? [ Styles := Styles || WS_VISIBLE. ].
        
        #var(type:int)anX.
        #var(type:int)anY.
        #var(type:int)aWidth.
        #var(type:int)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:WS_EX_TRANSPARENT
                        &wide:ButtonClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:aParent &hinstance:CurrentInstance &object:$nil.
    ]
}

// --- Label ---

#class ILabel :: IControl
{
    #field(type:label)theLabel.
    
    #constructor new &label:aLabel
    [
        theLabel := aLabel.
    ]
    
    #method object = theLabel.
    
    #method open [ theLabel open. ]
          
    #method retrieve &hwnd:aHandle = theLabel retrieve &hwnd:aHandle.
    
    #method set &dimension:aDimension [ theLabel set &dimension:aDimension. ]
    
    #method set &location:aPoint [ theLabel set &location:aPoint. ]
}

#class(limited) Label :: BaseWinControl
{
    #method control = ILabel new &label:$self.
    
    #method label = $self.
    
    #method $createHandle
    [
        #var(type:hwnd)aParent := theParent hwnd.
        
        #var(type:int)Styles := WS_CHILD.
        theVisible
            ? [ Styles := Styles || WS_VISIBLE. ].
        
        #var(type:int)anX.
        #var(type:int)anY.
        #var(type:int)aWidth.
        #var(type:int)aHeight.
        
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
    #method $createHandle
    [
        #var(type:hwnd)aParent := theParent hwnd.
        (aParent == 0)
            ? [ #throw InvalidOperationException new &message:"The parent window is closed". ].
        
        #var(type:int)Styles := WS_CHILD || SS_SIMPLE.
        theVisible
            ? [ Styles := Styles || WS_VISIBLE. ].
        
        #var(type:int)anX.
        #var(type:int)anY.
        #var(type:int)aWidth.
        #var(type:int)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:0
                        &wide:StaticClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:aParent &hinstance:CurrentInstance &object:$nil.
    ]
}

// --- Edit ---

#class IEdit :: IControl
{
    #field(type:edit)theEdit.
    
    #constructor new &edit:anEdit
    [
        theEdit := anEdit.
    ]
    
    #method object = theEdit.
    
    #method open [ theEdit open. ]
          
    #method set &dimension:aDimension [ theEdit set &dimension:aDimension. ]
    
    #method set &location:aPoint [ theEdit set &location:aPoint. ]
    
    #method retrieve &hwnd:aHandle = theEdit retrieve &hwnd:aHandle.
}

#class(sealed) Edit :: BaseWinControl
{
    #constructor new
        <= (new)
    [
        theTabStop := true.
    ]
    
    #method control = IEdit new &edit:$self.
    
    #method edit = $self.
 
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
          
    #method $createHandle
    [
        #var(type:hwnd)aParent := theParent hwnd.
        
        #var(type:int)Styles := WS_CHILD || WS_BORDER.
        theVisible
            ? [ Styles := Styles || WS_VISIBLE. ].
        theTabStop
            ? [ Styles := Styles || WS_TABSTOP. ].
        
        #var(type:int)anX.
        #var(type:int)anY.
        #var(type:int)aWidth.
        #var(type:int)aHeight.
        
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
    #field(type:func1)theClick.
    
    #constructor new
        <= (new)
    [
        theTabStop := true.
    ]
    
    #method set &onClick:aFunction
    [
        theClick := aFunction.
    ]
    
    #method $updateStyles &vint:aRetVal
    [
        #var(type:int)Styles := 0.
        
        theVisible
            ? [ Styles := Styles || WS_VISIBLE. ].
        theTabStop
            ? [ Styles := Styles || WS_TABSTOP. ].
            
        aRetVal << (aRetVal || Styles).
    ]
    
    #method click 
    [
        ($nil != theClick)
            ? [ theClick eval:$self. ].
    ]
}

// --- Button ---

#class IButton :: IControl
{
    #field(type:button)theButton.
    
    #constructor new &button:aButton
    [
        theButton := aButton.
    ]
    
    #method object = theButton.
 
    #method open [ theButton open. ]

    #method retrieve &hwnd:aHandle = theButton retrieve &hwnd:aHandle.
    
    #method set &dimension:aDimension [ theButton set &dimension:aDimension. ]
    
    #method set &location:aPoint [ theButton set &location:aPoint. ]
}

#class(sealed) Button :: BaseButton
{
    #method control = IButton new &button:$self.
    
    #method button = $self.
 
    #method $createHandle
    [
        #var(type:hwnd)aParent := theParent hwnd.
        
        #var(type:vint)Styles := 0.
        Styles << (WS_CHILD || BS_PUSHBUTTON).
        $self $updateStyles &vint:Styles.
                
        #var(type:int)anX.
        #var(type:int)anY.
        #var(type:int)aWidth.
        #var(type:int)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:0
                        &wide:ButtonClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:aParent &hinstance:CurrentInstance &object:$nil.
    ]
}

// --- RadioButton ---

#class IRadioButton :: IControl
{
    #field(type:radiobutton)theButton.
    
    #constructor new &radiobutton:aButton
    [
        theButton := aButton.
    ]
    
    #method object = theButton.
 
    #method open [ theButton open. ]
          
    #method retrieve &hwnd:aHandle = theButton retrieve &hwnd:aHandle.
    
    #method set &dimension:aDimension [ theButton set &dimension:aDimension. ]
    
    #method set &location:aPoint [ theButton set &location:aPoint. ]
    
    #method => theButton.
}

#class(limited) RadioButton :: BaseButton
{
    #field theChecked.
    
    #constructor new
        <= (new)
    [
        theChecked := false.
    ]
    
    #method control = IRadioButton new &radiobutton:$self.
    
    #method radiobutton = $self.
 
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
        #var(type:hwnd)aParent := theParent hwnd.
        
        #var(type:vint)Styles := 0.
        Styles << (WS_CHILD || BS_AUTORADIOBUTTON).
        $self $updateStyles &vint:Styles.
                
        #var(type:int)anX.
        #var(type:int)anY.
        #var(type:int)aWidth.
        #var(type:int)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:0
                        &wide:ButtonClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:aParent &hinstance:CurrentInstance &object:$nil.
                        
        theChecked
            ? [ $self set &checked:true. ].
    ]
}

// --- Paintbox ---

#class PaintboxListener :: WindowListener
{
    #field(type:paintbox)theOwner.
    
    #constructor new &paintbox:anOwner
    [
        theOwner := anOwner.
    ]
    
    #method onpaint &hwnd:aControl &vint:aResult
    [
        theOwner $paint.
        
        aResult << 0.
    ]
    
    #method onsize &hwnd:aControl &int:wParam &int:lParam &vint:aResult
    [
        #var(type:int)width.
        #var(type:int)height.
        width := lParam.
        width := width && 0FFFFh.
        
        lParam shift &index:16 &vint:height.
        
        theOwner $resize &hwnd:aControl &int:width &int:height.
        
        aResult << 0.
    ]
    
    #method ondestroy &hwnd:aControl &vint:aResult
    [
        theOwner $destroy.
        
        $super ondestroy &hwnd:aControl &vint:aResult.
    ]
}

#class IPaintbox :: IControl
{
    #field(type:paintbox)thePaintbox.
    
    #constructor new &paintbox:aPaintbox
    [
        thePaintbox := aPaintbox.
    ]
    
    #method object = thePaintbox.
 
    #method open [ thePaintbox open. ]
    
    #method retrieve &hwnd:aHandle = thePaintbox retrieve &hwnd:aHandle.
    
    #method set &dimension:aDimension [ thePaintbox set &dimension:aDimension. ]
    
    #method set &location:aPoint [ thePaintbox set &location:aPoint. ]
}

#class(limited) Paintbox :: BaseWinControl
{
    #field(type:canvas) theCanvas.
    #field(type:onPaint)thePaint.
    
    #constructor new
        <= (new)
    [
        theCanvas := Canvas new.
    ]
    
    #method control = IPaintbox new &paintbox:$self.
    
    #method paintbox = $self.

    #method set &onPaint:aFunction
    [
        thePaint := aFunction.
    ]
   
    #method $destroy
    [
        theCanvas free.
    ]
 
    #method $paint
    [
        #var(type:win_paintstruct)struct.
        
        struct begin &hwnd:theHandle.
        
        theCanvas open &win_paintstruct:struct.
        
        ($nil == thePaint)
            ! [ thePaint eval:$self &canvas:theCanvas. ].
        
        theCanvas close.

        struct end &hwnd:theHandle.
    ]
 
    #method $resize &hwnd:aHandle &int:aWidth &int:aHeight
    [
    ]
  
    #method $createHandle
    [
        #var(type:hwnd)aParent := theParent hwnd.

        #var(type:int)Styles := WS_CHILD.
        theVisible
            ? [ Styles := Styles || WS_VISIBLE. ].
                        
        #var(type:int)anX.
        #var(type:int)anY.
        #var(type:int)aWidth.
        #var(type:int)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:WS_EX_TRANSPARENT
                        &wide:PaintboxClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:aParent &hinstance:CurrentInstance &object:(PaintboxCallback new &win_listener:(PaintboxListener new &paintbox:$self)).
    ]
}

#class Imagebox :: Paintbox
{
    #field(type:hbitmap)  theImage.
    #field(type:hdc)      theImageDC.
    
    #constructor new
        <= (new)
    [
    ]

    #method $destroy
    [
        $super $destroy.
        
        theImage free.
        theImageDC free.
    ]
    
    #method $paint
    [
        #var(type:win_paintstruct)struct.
        
        struct begin &hwnd:theHandle.
        
        #var(type:int)aWidth.
        #var(type:int)aHeight.
        theRegion readDimension &vint:aWidth &vint:aHeight.
        
        #var(type:hdc)dc.
        struct read &hdc:dc.
        
        dc copy &int:0 &int:0 &int:aWidth &int:aHeight &hdc:theImageDC &int:0 &int:0 &int:SRCCOPY.
                
        struct end &hwnd:theHandle.
    ]
 
    #method refresh
    [
        ($nil == theHandle)
            ? [ ^ $self. ].
        
        ($nil == thePaint)
            ! [ thePaint eval:$self &canvas:theCanvas. ].

        $super refresh.
    ]
 
    #method $resize &hwnd:aHandle &int:aWidth &int:aHeight
    [
        ($nil == theImage)
            ! [
                theImage free.
                theImageDC free.
            ].
            
        #var(type:hdc)dc.
        aHandle read &hdc:dc.
            
        theImage := HBITMAP new &hdc:dc &int:aWidth &int:aHeight.
        theImageDC := HDC newCompatible &hdc:dc.
        theImage select &hdc:theImageDC.

        theCanvas open &hdc:theImageDC.

        $self refresh.
    ]
}

// --- BasePanel ---

#class BasePanel :: BaseWinContainer
{
    #field(type:frame) theFrame.
    
    #constructor new
        <= (new)
    [
        theFrame := Frame new.
        
        // NOTE : a frame should be the first element
        theControls append:theFrame.
    ]
    
    #method set &dimension:aSize
    [
        $super set &dimension:aSize.
        
        theFrame set &dimension:aSize.
    ]
    
    #method set &caption:aCaption
    [
        theFrame set &caption:aCaption.
    ]
    
    #method $resize &int:aWidth &int:aHeight
    [
        #var(type:dimension)aSize.
        aSize write &int:aWidth &int:aHeight.
        
        theFrame set &dimension:aSize.
    ]    
}

// --- Panel ---

#class PanelListener :: WindowListener
{
    #field(type:panel)theOwner.
    
    #constructor new &panel:anOwner
    [
        theOwner := anOwner.
    ]
    
    #method onclick &hwnd:aHandle &vint:aResult
    [
        #var aControl := theOwner retrieve &hwnd:aHandle.
        ($nil != aControl)
            ? [ aControl click. ].
        
        $super onclick &hwnd:aHandle &vint:aResult.
    ]
}

#class IPanel :: IControl
{
    #field(type:panel)thePanel.
    
    #constructor new &panel:aPanel
    [
        thePanel := aPanel.
    ]
    
    #method object = thePanel.
 
    #method open [ thePanel open. ]
    
    #method close [ thePanel close. ]
          
    #method retrieve &hwnd:aHandle = thePanel retrieve &hwnd:aHandle.
    
    #method set &dimension:aDimension [ thePanel set &dimension:aDimension. ]
    
    #method set &location:aPoint [ thePanel set &location:aPoint. ]
}

#class(sealed) Panel :: BasePanel
{
    #method panel = $self.

    #method control = IPanel new &panel:$self.
    
    #method controls = theControls.
            
    #method $createHandle
    [
        #var(type:hwnd)aParent := theParent hwnd.

        #var(type:int)Styles := WS_CHILD || WS_CLIPSIBLINGS.
        theVisible
            ? [ Styles := Styles || WS_VISIBLE. ].
                        
        #var(type:int)anX.
        #var(type:int)anY.
        #var(type:int)aWidth.
        #var(type:int)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:WS_EX_CONTROLPARENT
                        &wide:PanelClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:aParent &hinstance:CurrentInstance &object:(PanelCallback new &win_listener:(PanelListener new &panel:$self)).
    ]
}

// --- RadioButtonGroup ---

#class RadioButtonGroupListener :: WindowListener
{
    #field(type:radiogroup)theOwner.
    
    #method onclick &hwnd:aHandle &vint:aResult
    [
        theOwner $onClick &hwnd:aHandle.
        
        aResult << 0.
    ]
    
    #constructor new &radiogroup:anOwner
    [
        theOwner := anOwner.
    ]
}

#class IRadioButtonGroup :: IControl
{
    #field(type:radiogroup)theGroup.
    
    #constructor new &radiogroup:aGroup
    [
        theGroup := aGroup.
    ]
    
    #method object = theGroup.
 
    #method open [ theGroup open. ]
          
    #method retrieve &hwnd:aHandle = theGroup retrieve &hwnd:aHandle.
    
    #method set &dimension:aDimension [ theGroup set &dimension:aDimension. ]
    
    #method set &location:aPoint [ theGroup set &location:aPoint. ]
}

#class(sealed) RadioButtonGroup :: BasePanel
{
    #field(type:int)theSelected.
    #field(type:func1)theIndexChanged.
    
    #constructor new
        <= (new)
    [
        theSelected := -1.
    ]
    
    #method radiogroup = $self.

    #method control = IRadioButtonGroup new &radiogroup:$self.

    #method selected_index = theSelected.

    #method $setChecked : aValue
    [
        #var(type:int)anIndex := -1. // !! to skip a frame
        
        #var aCurrent.
        #var(type:enumerator)it := theControls enumerator.
        #loop (it next)?
        [ 
            aCurrent := it get.
        
            (theSelected == anIndex)
                ? [
                    aCurrent set &checked:aValue.
                    
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
        theIndexChanged := aFunction.
    ]

    #method items =
    {
        append : anItem
        [
            theControls append:(RadioButton new set &caption:anItem).
            
            self $resize.
        ]
    }.

    #method set &dimension:aSize
    [
        $super set &dimension:aSize.
        
        $self $resize.
    ]

    #method $onClick &hwnd:aHandle
    [
        #var(type:int)anIndex := -1. // !! to skip a frame
        
        #var aRetVal := $nil.
        #var(type:control)aCurrent.
        #var(type:enumerator)it := theControls enumerator.
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
        #var(type:int)aWidth.
        #var(type:int)aHeight.
        theRegion readDimension &vint:aWidth &vint:aHeight.
        
        $self $resize &int:aWidth &int:aHeight.
    ]
            
    #method $resize &int:aWidth &int:aHeight
    [
        #var(type:point)aPoint.
        #var(type:dimension)aSize.
        aSize write &int:(aWidth - 6) &int:20.
        
        #var(type:control)aCurrent.
        #var(type:enumerator)it := theControls enumerator.
        #var(type:int)i := 0.
        
        it next. // NOTE : skip the frame
        
        #loop (it next)?
        [ 
            aCurrent := it get control.
        
            aPoint write &int:4 &int:(18 + i * 20).
            
            aCurrent set &location:aPoint.
            aCurrent set &dimension:aSize.
        
            i :=  i + 1.
        ].
        
        $super $resize &int:aWidth &int:aHeight.
    ]
    
    #method $createHandle
    [
        #var(type:hwnd)aParent := theParent hwnd.

        #var(type:int)Styles := WS_CHILD || WS_CLIPSIBLINGS.
        theVisible
            ? [ Styles := Styles || WS_VISIBLE. ].
                        
        #var(type:int)anX.
        #var(type:int)anY.
        #var(type:int)aWidth.
        #var(type:int)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:WS_EX_CONTROLPARENT
                        &wide:PanelClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:aParent &hinstance:CurrentInstance &object:(PanelCallback new &win_listener:(RadioButtonGroupListener new &radiogroup:$self)).
                        
        ((theSelected != -1)and:($nil != theIndexChanged))?
            [ theIndexChanged eval:$self. ].
    ]
}

// --- ImageList ---

#class(sealed) ImageList
{
    #field(type:dimension)theSize.
    #field theParent.
    #field theImages.
    #field(type:int)theIndex.
 
    #constructor new
    [
        theIndex := 0.
        theImages := List new.
        
        theSize :=  Dimension new &width:50 &height:50.
    ]
 
    #method assign &paintbox:aPaintbox
    [
        aPaintbox set &onPaint:(:sender &canvas:aCanvas)
        [
            // !! temporal, stack optimized version should be used
            #var(type:int)aWidth := theSize width.
            #var(type:int)aHeight := theSize height.
            
            //theSize read &vint:aWidth &vint:aHeight.
            
            #var anImage := theImages getAt &index:(theIndex int).
            
            aCanvas write &image:anImage &int:0 &int:0 &int:aWidth &int:aHeight.
        ].
    ]

    #method selected_index = theIndex.

    #method set &selected_index : anIndex
    [
        theIndex := anIndex int.
    ] 
   
    #method control = IControl
    {
        #method object = self.
 
        #method open [ self open. ]
          
        #method close [ self close. ]
          
        #method retrieve &hwnd:aHandle = $nil.
    
        #method set &dimension:aDimension [ self set &dimension:aDimension. ]
    }.
 
    #method $set &parent:aParent
    [
        theParent := aParent.
    ]
          
    #method dimension = theSize.
    
    #method set &dimension:aSize
    [
        theSize := aSize.
    ]
    
    #method set &width:aWidth &height:aHeight
    [
        theSize write &width:aWidth.
        theSize write &height:aHeight.
    ]
    
    #method append &path:aPath
    [        
        theImages append:(Image new &path:aPath &dimension:theSize).
    ]
    
    #method open
    [
        #var(type:hwnd)aHandle := theParent hwnd.
        
        #var(type:enumerator)it := theImages enumerator.
        #loop (it next)? [
            it get open &hwnd:aHandle.
        ].
    ]
    
    #method close
    [
        #var(type:enumerator)it := theImages enumerator.
        #loop (it next)? [
            it get free.
        ].
    ]    
}