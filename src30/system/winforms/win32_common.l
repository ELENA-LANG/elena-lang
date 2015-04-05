#define system.

// --- ControlHandle ---

#class ControlHandle
{
    #field(type:handle)theHandle.
    
    #constructor new &handle:aHandle
    [
        theHandle := Handle new &handle:aHandle.
    ]

    #constructor new &int:dwExStyle &wide:aClassName &wide:aWindowName &int:aStyle &int:x &int:y &int:width &int:height &handle:aParent &hinstance:instance
    [
        #var(type:ptr)aPointer := Pointer new:$self.

        #var(type:vint)aParamIndex.
        aPointer read &vint:aParamIndex.
        
        #var(type:int)tmp := aParamIndex. // !! temporal
        
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
            &int:       tmp.
            
        theHandle := Handle new &handle:aHandle.
    ]
            
    #method close
    [
        theHandle close.
    ]
}

