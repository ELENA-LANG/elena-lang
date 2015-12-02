// ==== ver 3.4.1 ===

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
    
    #method close []
}

// --- BaseWindowControl ---

#class BaseWinControl :: BaseControl
{
    #field(type:hwnd)theHandle.

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
    #field(type:controls) theControls.
    #field(type:func1)    theResize.
    
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
    
    #method(stacksafe) $resize &int:aWidth &int:aHeight
    [
        $super $resize &int:aWidth &int:aHeight.
        
        ($nil == theResize)
            ! [ theResize eval:$self. ].
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
    
    #method setDimension &int:aWidth &int:aHeight [ theFrame setDimension &int:aWidth &int:aHeight. ]
    
    #method setLocation &int:aWidth &int:aHeight [ theFrame setLocation &int:aWidth &int:aHeight. ]
    
    #method readDimension &vint:aWidth &vint:aHeight [ theFrame readDimension &vint:aWidth &vint:aHeight. ]
    
    #method readLocation &vint:aWidth &vint:aHeight [ theFrame readLocation &vint:aWidth &vint:aHeight. ]
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
    
    #method setDimension &int:aWidth &int:aHeight [ theLabel setDimension &int:aWidth &int:aHeight. ]
    
    #method setLocation &int:aWidth &int:aHeight [ theLabel setLocation &int:aWidth &int:aHeight. ]
    
    #method readDimension &vint:aWidth &vint:aHeight [ theLabel readDimension &vint:aWidth &vint:aHeight. ]
    
    #method readLocation &vint:aWidth &vint:aHeight [ theLabel readLocation &vint:aWidth &vint:aHeight. ]
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
            ? [ #throw InvalidOperationException new:"The parent window is closed". ].
        
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

// --- BaseEdit ---

#class BaseEdit :: BaseWinControl
{
    #field(type:func1)theChanged.
    
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
        #var(type:int)Styles := 0.
        
        theVisible
            ? [ Styles := Styles || WS_VISIBLE. ].
        theTabStop
            ? [ Styles := Styles || WS_TABSTOP. ].
            
        aRetVal << (aRetVal || Styles).
    ]
    
    #method change 
    [
        ($nil != theChanged)
            ? [ theChanged eval:$self. ].
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
          
    #method setDimension &int:aWidth &int:aHeight [ theEdit setDimension &int:aWidth &int:aHeight. ]
    
    #method setLocation &int:aWidth &int:aHeight [ theEdit setLocation &int:aWidth &int:aHeight. ]
    
    #method readDimension &vint:aWidth &vint:aHeight [ theEdit readDimension &vint:aWidth &vint:aHeight. ]
    
    #method readLocation &vint:aWidth &vint:aHeight [ theEdit readLocation &vint:aWidth &vint:aHeight. ]
    
    #method retrieve &hwnd:aHandle = theEdit retrieve &hwnd:aHandle.
}

#class(sealed) Edit ::  BaseEdit
{
    #constructor new
        <= (new)
    [
        theTabStop := true.
    ]
    
    #method control = IEdit new &edit:$self.
    
    #method edit = $self.
 
    #method $createHandle
    [
        #var(type:hwnd)aParent := theParent hwnd.
        
        #var(type:int)Styles := WS_CHILD || WS_BORDER.
        $self $updateStyles &vint:Styles.
        
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

// --- Memo ---

#class IMemo :: IControl
{
    #field(type:memo)theEdit.
    
    #constructor new &memo:anEdit
    [
        theEdit := anEdit.
    ]
    
    #method object = theEdit.
    
    #method open [ theEdit open. ]
          
    #method setDimension &int:aWidth &int:aHeight [ theEdit setDimension &int:aWidth &int:aHeight. ]
    
    #method setLocation &int:aWidth &int:aHeight [ theEdit setLocation &int:aWidth &int:aHeight. ]
    
    #method readDimension &vint:aWidth &vint:aHeight [ theEdit readDimension &vint:aWidth &vint:aHeight. ]
    
    #method readLocation &vint:aWidth &vint:aHeight [ theEdit readLocation &vint:aWidth &vint:aHeight. ]
    
    #method retrieve &hwnd:aHandle = theEdit retrieve &hwnd:aHandle.
}

#class(sealed) Memo :: BaseEdit
{
    #constructor new
        <= (new)
    [
        theTabStop := true.
    ]
    
    #method control = IMemo new &memo:$self.
    
    #method memo = $self.
 
    #method wide = $self caption wide.
          
    #method literal = $self caption literal.
    
    #method $createHandle
    [
        #var(type:hwnd)aParent := theParent hwnd.
        
        #var(type:int)Styles := WS_CHILD || WS_BORDER || ES_MULTILINE || ES_WANTRETURN || ES_AUTOHSCROLL || ES_AUTOVSCROLL || WS_VSCROLL || WS_HSCROLL.
        $self $updateStyles &vint:Styles.
        
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
        theClick := aFunction func1.
    ]
    
    #method(stacksafe) $updateStyles &vint:aRetVal
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
    
    #method setDimension &int:aWidth &int:aHeight [ theButton setDimension &int:aWidth &int:aHeight. ]
    
    #method setLocation &int:aWidth &int:aHeight [ theButton setLocation &int:aWidth &int:aHeight. ]
    
    #method readDimension &vint:aWidth &vint:aHeight [ theButton readDimension &vint:aWidth &vint:aHeight. ]
    
    #method readLocation &vint:aWidth &vint:aHeight [ theButton readLocation &vint:aWidth &vint:aHeight. ]
}

#class(sealed) Button :: BaseButton
{
    #method control = IButton new &button:$self.
    
    #method button = $self.
 
    #method $createHandle
    [
        #var(type:hwnd)aParent := theParent hwnd.
        
        #var(type:int)Styles := 0.
        Styles := (WS_CHILD || BS_PUSHBUTTON).
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
    
    #method setDimension &int:aWidth &int:aHeight [ theButton setDimension &int:aWidth &int:aHeight. ]
    
    #method setLocation &int:aWidth &int:aHeight [ theButton setLocation &int:aWidth &int:aHeight. ]
    
    #method readDimension &vint:aWidth &vint:aHeight [ theButton readDimension &vint:aWidth &vint:aHeight. ]
    
    #method readLocation &vint:aWidth &vint:aHeight [ theButton readLocation &vint:aWidth &vint:aHeight. ]
    
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
        
        #var(type:int)Styles := 0.
        Styles := (WS_CHILD || BS_AUTORADIOBUTTON).
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

// --- BaseList ---

#class BaseList :: BaseWinControl
{
    #field(type:array_list)theList.
    
    #constructor new
        <= (new)
    [
        theList := ArrayList new.
    ]
    
    #method items =
    {
        append : anObject &as:aCaption
        [
            theList append:(KeyValue new &key:aCaption &object:anObject).
            
            $self $appendItem &wide:(aCaption wide).
        ]
    }.
    
    #method(stacksafe) $updateStyles &vint:aRetVal
    [
        #var(type:int)Styles := 0.
        
        theVisible
            ? [ Styles := Styles || WS_VISIBLE. ].
        theTabStop
            ? [ Styles := Styles || WS_TABSTOP. ].
            
        aRetVal << (aRetVal || Styles).
    ]
}

// --- Combobox ---

#class ICombobox :: IControl
{
    #field(type:combobox)theCombobox.
    
    #constructor new &combobox:aCombobox
    [
        theCombobox := aCombobox.
    ]
    
    #method object = theCombobox.
 
    #method open [ theCombobox open. ]
          
    #method retrieve &hwnd:aHandle = theCombobox retrieve &hwnd:aHandle.
    
    #method setDimension &int:aWidth &int:aHeight [ theCombobox setDimension &int:aWidth &int:aHeight. ]
    
    #method setLocation &int:aWidth &int:aHeight [ theCombobox setLocation &int:aWidth &int:aHeight. ]
    
    #method readDimension &vint:aWidth &vint:aHeight [ theCombobox readDimension &vint:aWidth &vint:aHeight. ]
    
    #method readLocation &vint:aWidth &vint:aHeight [ theCombobox readLocation &vint:aWidth &vint:aHeight. ]
    
    #method => theCombobox.
}

#class(sealed) Combobox :: BaseList
{
    #constructor new
        <= (new)
    [
    ]

    #method control = ICombobox new &combobox:$self.
    
    #method $appendItem &wide:aCaption
    [
        ($nil == theHandle)
            ? [ ^ $self. ].
        
        theHandle sendMessage &int:CB_ADDSTRING &int:0 &wide:aCaption.
    ]
    
    #method $loadStrings
    [
        #var(type:int)aLength.
        theList readLength &vint:aLength.
        
        #var(type:int)i := 0.
        #loop (i < aLength)?
        [
            #var aKeyValue := theList getAt &int:i.
            
            theHandle sendMessage &int:CB_ADDSTRING &int:0 &wide:(aKeyValue key wide).
        ].
    ]
    
    #method $createHandle
    [
        #var(type:hwnd)aParent := theParent hwnd.
        
        #var(type:int)Styles := 0.
        Styles := (WS_CHILD || CBS_DROPDOWNLIST).
        $self $updateStyles &vint:Styles.
                
        #var(type:int)anX.
        #var(type:int)anY.
        #var(type:int)aWidth.
        #var(type:int)aHeight.
        
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
    
    #method onsize &hwnd:aControl &int:width &int:height &vint:aResult
    [
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
    
    #method setDimension &int:aWidth &int:aHeight [ thePaintbox setDimension &int:aWidth &int:aHeight. ]
    
    #method setLocation &int:aWidth &int:aHeight [ thePaintbox setLocation &int:aWidth &int:aHeight. ]
    
    #method readDimension &vint:aWidth &vint:aHeight [ thePaintbox readDimension &vint:aWidth &vint:aHeight. ]
    
    #method readLocation &vint:aWidth &vint:aHeight [ thePaintbox readLocation &vint:aWidth &vint:aHeight. ]
    
}

#class(limited) Paintbox :: BaseWinControl
{
    #field(type:canvas) theCanvas.
    #field thePaint.
    
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
        #var(type:hdc)dc.
        
        struct begin &hwnd:theHandle.
        struct read &hdc:dc.
        
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
                        &hwnd:aParent &hinstance:CurrentInstance &object:(PaintboxListener new &paintbox:$self).
    ]
}

#class Imagebox :: Paintbox
{
    #field(type:hbitmap)  theImage.
    #field(type:hdc)      theImageDC.
    #field(type:bool)     theNeedToRefresh.
    
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
        theNeedToRefresh := false.
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

        theNeedToRefresh := true.
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
        #var(type:dimension)aSize.
        aSize write &int:aWidth &int:aHeight.
        
        theFrame setDimension &int:aWidth &int:aHeight.
    ]    
}

// --- Panel ---

#class PanelListener :: WindowCallback
{
    #field(type:panel)theOwner.
    
    #constructor new &panel:anOwner
    [
        theOwner := anOwner.
    ]
    
    #method onsize &hwnd:aControl &int:width &int:height &vint:aResult
    [
        theOwner $resize &int:width &int:height.
        
        aResult << 0.
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
    
    #method setDimension &int:aWidth &int:aHeight [ thePanel setDimension &int:aWidth &int:aHeight. ]
    
    #method setLocation &int:aWidth &int:aHeight [ thePanel setLocation &int:aWidth &int:aHeight. ]
    
    #method readDimension &vint:aWidth &vint:aHeight [ thePanel readDimension &vint:aWidth &vint:aHeight. ]
    
    #method readLocation &vint:aWidth &vint:aHeight [ thePanel readLocation &vint:aWidth &vint:aHeight. ]
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
                        &hwnd:aParent &hinstance:CurrentInstance &object:(PanelListener new &panel:$self).
    ]
}

// --- RadioButtonGroup ---

#class RadioButtonGroupListener :: WindowCallback
{
    #field(type:radiogroup)theOwner.
    
    #method onclick &hwnd:aHandle &vint:aResult
    [
        theOwner $onClick &hwnd:aHandle.
        
        aResult << 0.
    ]
    
    #method onsize &hwnd:aControl &int:width &int:height &vint:aResult
    [
        theOwner $resize &int:width &int:height.
        
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
    
    #method setDimension &int:aWidth &int:aHeight [ theGroup setDimension &int:aWidth &int:aHeight. ]
    
    #method setLocation &int:aWidth &int:aHeight [ theGroup setLocation &int:aWidth &int:aHeight. ]
    
    #method readDimension &vint:aWidth &vint:aHeight [ theGroup readDimension &vint:aWidth &vint:aHeight. ]
    
    #method readLocation &vint:aWidth &vint:aHeight [ theGroup readLocation &vint:aWidth &vint:aHeight. ]
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
        #var(type:control)aCurrent.
        #var(type:enumerator)it := theControls enumerator.
        #var(type:int)i := 0.
        
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
                        &hwnd:aParent &hinstance:CurrentInstance &object:(RadioButtonGroupListener new &radiogroup:$self).
                        
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
 
    #method assign : aControl
       <= assign &paintbox:(aControl paintbox).
 
    #method assign &paintbox:aPaintbox
    [
        aPaintbox set &onPaint:(:sender &canvas:aCanvas)
        [
            #var(type:int)aWidth.
            #var(type:int)aHeight.
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
        #method object = self.
 
        #method open [ self open. ]
          
        #method close [ self close. ]
          
        #method retrieve &hwnd:aHandle = $nil.
    
        #method setDimension &int:aWidth &int:aHeight [ $self setDimension &int:aWidth &int:aHeight. ]
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
        theSize write &width:aWidth.
        theSize write &height:aHeight.
    ]
    
    #method append &path:aPath
    [        
        theImages append:(Image new &path:aPath &dimension:theSize).
    ]
    
    #method open
    [
        #var(type:hdc)aParentDC := theParent hwnd hdc.        
        
        #var(type:enumerator)it := theImages enumerator.
        #loop (it next)? [
            it get image open &hdc:aParentDC.
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

// --- DockingBox ---

#class(sealed) DockingBox
{
    #field theParent.
    #field(type:control)theLeft.
    #field(type:control)theRight.
    #field(type:control)theTop.
    #field(type:control)theBottom.
    #field(type:control)theClient.

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
        #method object = self.
 
        #method open [ self open. ]
          
        #method close [ self close. ]
          
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
        
    #method refresh
    [
        #var(type:int)tmp.
        
        #var(type:dimension)theSize := theParent dimension.

        #var(type:int)height := 0.
        #var(type:int)width := 0.
        #var(type:int)x := 0.
        #var(type:int)y := 0.
        #var(type:int)aTotalWidth.
        #var(type:int)aTotalHeight.
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
