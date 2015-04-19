#define system.
#define system'winforms.

// --- IControl ---

#class(limited) IControl
{
    #method control = $self.
    
    #method object = nil.
    
    #method retrieve &hwnd:aHandle = nil.
    
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
            
        ^ nil.            
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
        (nil != theHandle)
            ? [ #throw InvalidOperationException new &message:"The form is already open". ].
            
        $self $createHandle.
        
        $self $onCreate.
    ]

    #method is &opened = (nil != theHandle).
    
    #method hwnd = theHandle.
            
    #method set &enabled : aValue
    [
        $super set &enabled:aValue.
        
        (nil == theHandle)
            ! [
                theEnabled
                    ? [ theHandle setEnabled &int:-1. ]
                    ! [ theHandle setEnabled &int:0. ].
            ].
    ]
    
    #method set &caption : aCaption
    [
        $super set &caption:aCaption.
        
        (nil == theHandle)
            ! [
                theHandle setCaption &wide:(aCaption wide).
            ].
    ]

    #method set &location:aLocation
    [
        $super set &location:aLocation.
        
        (nil == theHandle)
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
        
        (nil == theHandle)
            ! [
                #var(type:vint)aWidth.
                #var(type:vint)aHeight.
                aSize read &vint:aWidth &vint:aHeight.
        
                theHandle setPosition &int:0 &int:0 &int:aWidth &int:aHeight &int_attributes:SWP_NOMOVE.
            ].
    ]
    
    #method refresh
    [
        theHandle refresh.
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
        #var aRetVal := nil.
        #loop (it next)?
        [ 
            aCurrent := it get control.
        
            aRetVal := aCurrent retrieve &hwnd:aHandle.
            (nil != aRetVal)
                ? [ ^ aRetVal. ].
        ].
                                    
        ^ nil.            
    ]
        
    #method open
    [
        $super open.
        
        #var(type:enumerator)it := theControls enumerator.
        #loop (it next)?
            [ it get open. ].
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
        
        #var(type:vint)anX.
        #var(type:vint)anY.
        #var(type:vint)aWidth.
        #var(type:vint)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:0
                        &wide:StaticClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:aParent &hinstance:CurrentInstance &object:nil.
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
        
        #var(type:vint)anX.
        #var(type:vint)anY.
        #var(type:vint)aWidth.
        #var(type:vint)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:0
                        &wide:StaticClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:aParent &hinstance:CurrentInstance &object:nil.
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
        
        #var(type:vint)anX.
        #var(type:vint)anY.
        #var(type:vint)aWidth.
        #var(type:vint)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:WS_EX_CLIENTEDGE
                        &wide:EditClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:aParent &hinstance:CurrentInstance &object:nil.
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
}

#class(sealed) Button :: BaseWinControl
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
    
    #method control = IButton new &button:$self.
    
    #method button = $self.
 
    #method $createHandle
    [
        #var(type:hwnd)aParent := theParent hwnd.
        
        #var(type:int)Styles := WS_CHILD || BS_PUSHBUTTON.
        theVisible
            ? [ Styles := Styles || WS_VISIBLE. ].
        theTabStop
            ? [ Styles := Styles || WS_TABSTOP. ].
        
        #var(type:vint)anX.
        #var(type:vint)anY.
        #var(type:vint)aWidth.
        #var(type:vint)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:0
                        &wide:ButtonClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:aParent &hinstance:CurrentInstance &object:nil.
    ]
    
    #method click 
    [
        (nil != theClick)
            ? [ theClick eval:$self. ].
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
        #var(type:vint)height.
        width := lParam.
        width := width && 0FFFFh.
        
        lParam shift &index:16 &vint:height.
        
        theOwner $resize &int:width &int:height.
        
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
        
        (nil == thePaint)
            ! [ thePaint eval:$self &canvas:theCanvas. ].
        
        theCanvas close.

        struct end &hwnd:theHandle.
    ]
 
    #method $resize &int:aWidth &int:aHeight
    [
    ]
  
    #method $createHandle
    [
        #var(type:hwnd)aParent := theParent hwnd.

        #var(type:int)Styles := WS_CHILD.
        theVisible
            ? [ Styles := Styles || WS_VISIBLE. ].
                        
        #var(type:vint)anX.
        #var(type:vint)anY.
        #var(type:vint)aWidth.
        #var(type:vint)aHeight.
        
        theRegion read &vint:anX &vint:anY &vint:aWidth &vint:aHeight.
        
        theHandle := WindowHandle new &int:WS_EX_TRANSPARENT
                        &wide:PaintboxClass &wide:(theCaption wide) &int:Styles
                        &int:anX &int:anY &int:aWidth &int:aHeight 
                        &hwnd:aParent &hinstance:CurrentInstance &object:(PaintboxCallback new &win_listener:(PaintboxListener new &paintbox:$self)).
    ]
}

#class Imagebox :: Paintbox
{
    #field(type:hbitmap) theImage.
    #field(type:hdc)     theImageDC.
    
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
        
        #var(type:vint)aWidth.
        #var(type:vint)aHeight.
        theRegion readDimension &vint:aWidth &vint:aHeight.
        
        #var(type:hdc)dc.
        struct read &hdc:dc.
        
        dc copy &int:0 &int:0 &int:aWidth &int:aHeight &hdc:theImageDC &int:0 &int:0 &int:SRCCOPY.
                
        struct end &hwnd:theHandle.
    ]
 
    #method refresh
    [
        (nil == thePaint)
            ! [ thePaint eval:$self &canvas:theCanvas. ].

        $super refresh.
    ]
 
    #method $resize &int:aWidth &int:aHeight
    [
        (nil == theImage)
            ! [
                theImage free.
                theImageDC free.
            ].
            
        #var(type:hdc)dc.
        theHandle read &hdc:dc.
            
        theImage := HBITMAP new &hdc:dc &int:aWidth &int:aHeight.
        theImageDC := HDC newCompatible &hdc:dc.
        theImage select &hdc:theImageDC.

        theCanvas open &hdc:theImageDC.

        $self refresh.
    ]
}