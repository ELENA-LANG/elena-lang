#define system.

// --- classes ---
#symbol StaticClass = "Static" wide. // !! temporal
#symbol EditClass   = "Edit" wide. // !! temporal
#symbol ButtonClass = "Button" wide. // !! temporal

// --- ControlHandle ---

#class ControlHandle
{
    #field(type:handle)theHandle.
    
    #constructor new &handle:aHandle
    [
        theHandle := Handle new &handle:aHandle.
    ]

    #method handle = theHandle.

    #constructor new
    [
        theHandle := 0.
    ]

    #method create &int:dwExStyle &wide:aClassName &wide:aWindowName &int:aStyle &int:x &int:y &int:width &int:height &handle:aParent &hinstance:instance &object:aParam
    [
        #var(type:int)lparam := 0.
        (nil != aParam) ?
        [
            #var(type:rt_ptr)aPointer := RTPointer new:aParam.
    
            #var(type:vint)aParamIndex.
            aPointer read &vint:aParamIndex.
            
            lparam := aParamIndex.
        ].
                
        #var(type:handle)aHandle := system'external'USER32 CreateWindowExW
            &int:       dwExStyle
            &wide:      aClassName
            &wide:      aWindowName  
            &int:       aStyle
            &int:       x
            &int:       y
            &int:       width
            &int:       height
            &handle:    aParent
            &int:       0
            &hinstance: instance
            &int:       lparam.

        (aHandle==0)
            ? [ #throw WinFormsException new. ].

        theHandle := Handle new &handle:aHandle.
    ]
            
    #method setVisible &int:aValue
    [
        system'external'USER32 ShowWindow &handle:theHandle &int:aValue.
    ]
    
    #method isVisible
    [
        #var(type:int)visible := system'external'USER32 IsWindowVisible &handle:theHandle.
        
        ^ (visible != 0).
    ]
    
    #method setEnabled &int:aValue
    [
        system'external'USER32 EnableWindow &handle:theHandle &int:aValue.
    ]            
    
    #method isEnabled
    [
        #var(type:int)visible := system'external'USER32 IsWindowEnabled &handle:theHandle.
        
        ^ (visible != 0).
    ]
    
    #method setCaption &wide:aValue
    [
        system'external'USER32 SendMessageW &handle:theHandle &int:WM_SETTEXT &int:0 &wide:aValue.
    ]
    
    #method getCaption
    [
        #var(type:int)aLength := system'external'USER32 SendMessageW &handle:theHandle &int:WM_GETTEXTLENGTH &int:0 &int:0.
        
        (aLength == 0)
            ? [ ^ emptyWideLiteralValue. ]
            ! [
                #var(type:wide)aCaption := WideLiteralValue new &length:aLength.
                aLength := aLength + 1.
                
                system'external'USER32 SendMessageW &handle:theHandle &int:WM_GETTEXT &int:aLength &wide:aCaption.
                
                ^ aCaption.
            ].
    ]
}

// --- BaseLabelHandle ---

#class BaseLabelHandle :: ControlHandle
{
}

// --- StaticLabelHandle ---

#class(sealed) StaticLabelHandle :: BaseLabelHandle
{
    #constructor new &handle:aParent &wide:aCaption &int:x &int:y &int:width &int:height
    [
        $self create 
            &int:0 &wide:StaticClass &wide:aCaption 
            &int:(WS_CHILD or:SS_SIMPLE or:WS_VISIBLE) 
            &int:x &int:y &int:width &int:height 
            &handle:aParent 
            &hinstance:CurrentInstance
            &object:nil.
    ]
}

// --- BaseEditHandle ---

#class BaseEditHandle :: ControlHandle
{
}

// --- EditHandle ---

#class(sealed) EditHandle :: BaseEditHandle
{
    #constructor new &handle:aParent &wide:aText &int:x &int:y &int:width &int:height
    [
        $self create 
            &int:WS_EX_CLIENTEDGE
            &wide:EditClass &wide:aText
            &int:(WS_CHILD or:WS_BORDER or:WS_VISIBLE or:WS_TABSTOP) 
            &int:x &int:y &int:width &int:height 
            &handle:aParent 
            &hinstance:CurrentInstance
            &object:nil.
    ]
}

// --- BaseButtonHandle ---

#class BaseButtonHandle :: ControlHandle
{
}

// --- ButtonHandle ---

#class(sealed) ButtonHandle :: BaseButtonHandle
{
    #constructor new &handle:aParent &wide:aText &int:x &int:y &int:width &int:height
    [
        $self create 
            &int:0
            &wide:ButtonClass &wide:aText
            &int:(WS_CHILD or:BS_PUSHBUTTON or:WS_VISIBLE or:WS_TABSTOP) 
            &int:x &int:y &int:width &int:height 
            &handle:aParent 
            &hinstance:CurrentInstance
            &object:nil.
    ]
}
