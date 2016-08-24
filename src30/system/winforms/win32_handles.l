#import system.
//#import system'drawing.

// --- HInstance ---

#class(sealed,reference,const) HInstance
{
    #field(int)theHandle.
    
    #constructor(stacksafe) new &int:aHandle
    [
        theHandle := aHandle.
    ]
        
    #constructor current
    [
        #var(int)h := system'external'KERNEL32 GetModuleHandleW &int:0.
        
        theHandle := h.
    ]
//        <= new &int:(system'external'KERNEL32 GetModuleHandleW &int:0).

    #method hinstance = $self.
}

// --- HINSTANCE ---

#static(hinstance) CurrentInstance = HInstance current.

// --- MSG ---

#class(sealed,embeddable) MSG 
{
    #field(handle)theHWND.
    #field(int)   theMessage.
    #field(int)   theWParam.
    #field(int)   theLParam.
    #field(int)   theTime.
    #field(int)   theX.
    #field(int)   theY.    
    
    #method(stacksafe) readMessage &vint:aMessage
    [
        aMessage := theMessage.
    ]
}

// --- CREATESTRUCT ---

#class(sealed,embeddable)CREATESTRUCT
{
    #field(int)       theCreateParam.
    #field(hinstance) theInstance.    
    #field(int)       theMenu.
    #field(int)       theParent.
    #field(int)       theHeight.
    #field(int)       theWidth.
    #field(int)       theY.
    #field(int)       theX.
    #field(int)       theStyle.
    #field(int)       theNamePtr.
    #field(int)       theClassPtr.
    #field(int)       theExStyle.
    
    #method(stacksafe) readCreateParam &vint:aRetVal
    [
        aRetVal := theCreateParam.
    ]
}

// --- HMENU ---

#class(sealed,reference,const) HMENU
{
    #field(int)theHandle.
    
    #constructor new
    [
        #var(int)h := system'external'USER32 CreateMenu.
        
        theHandle := h.
    ]

    #constructor newPopup
    [
        #var(int)h := system'external'USER32 CreatePopupMenu.
        
        theHandle := h.
    ]

    #method(stacksafe) append &int:Flags &int:anID &wide:aCaption
    [
        system'external'USER32 AppendMenuW &int:theHandle &int:Flags &int:anID &wide:aCaption.
    ]
            
    #method(stacksafe) append &int:Flags &handle:aParent &wide:aCaption
    [
        system'external'USER32 AppendMenuW &int:theHandle &int:(Flags || MF_POPUP) &int:aParent &wide:aCaption.
    ]
            
    #method(stacksafe) free
    [
        system'external'USER32 DestroyMenu &int:theHandle.
    ]
}
