// ==== ver 3.4.1 ===

#import system.
//#import system'drawing.

// --- HInstance ---

#class(sealed,struct:embeddable) HInstance
{
    #field(type:int)theHandle.
    
    #constructor new &int:aHandle
    [
        theHandle := aHandle.
    ]
        
    #constructor current
    //    <= new &hinstance:(system'external'KERNEL32 GetModuleHandleW &int:0).
    [
        #var(type:int)aCurrent := system'external'KERNEL32 GetModuleHandleW &int:0.
        
        theHandle := aCurrent.
    ]

    #method hinstance = $self.
}

// --- HINSTANCE ---

#static(type:hinstance) CurrentInstance = HInstance current.

// --- MSG ---

#class(sealed,struct:embeddable) MSG 
{
    #field(type:handle)theHWND.
    #field(type:int)   theMessage.
    #field(type:int)   theWParam.
    #field(type:int)   theLParam.
    #field(type:int)   theTime.
    #field(type:int)   theX.
    #field(type:int)   theY.    
    
    #method readMessage &vint:aMessage
    [
    ]
    
    #method win_msg = $self.
}

// --- CREATESTRUCT ---

#class(sealed,struct:embeddable)CREATESTRUCT
{
    #field(type:int)       theCreateParam.
    #field(type:hinstance) theInstance.    
    #field(type:int)       theMenu.
    #field(type:int)       theParent.
    #field(type:int)       theHeight.
    #field(type:int)       theWidth.
    #field(type:int)       theY.
    #field(type:int)       theX.
    #field(type:int)       theStyle.
    #field(type:int)       theNamePtr.
    #field(type:int)       theClassPtr.
    #field(type:int)       theExStyle.
    
    #method(stacksafe) readCreateParam &vint:aRetVal
        => system'internal'int_save.
}

// --- HMENU ---

#class(sealed,struct:embeddable) HMENU
{
    #field(type:int)theHandle.
    
    #constructor new
    [
        #var(type:int)h := system'external'USER32 CreateMenu.
        
        theHandle := h.
    ]

    #constructor newPopup
    [
        #var(type:int)h := system'external'USER32 CreatePopupMenu.
        
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
