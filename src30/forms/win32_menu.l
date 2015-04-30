#define system.
#define system'collections.
#define system'winforms.

// --- BaseMenu ---

#class BaseMenu
{
    #field theParent.
    #field(type:list) theItems.
    
    #method items =
    {
        append &caption:aCaption &onClick:anAction
        [
            ($nil == theItems)
                ? [ theItems := List new. ].
            
            theItems append:(MenuItem new &parent:self &caption:aCaption &onClick:anAction).
        ]
    }.
    
    #method open
    [
        #var(type:enumerator)enum := theItems enumerator.
        #loop (enum next)?
        [
            enum get open.
        ].
    ]    
    
    #method close
    [
        #var(type:enumerator)enum := theItems enumerator.
        #loop (enum next)?
        [
            enum get close.
        ].
    ]    
}

// --- MenuItem ---

#class MenuItem :: BaseMenu
{
    #field(type:func1)theClick.
    #field(type:wide) theCaption.
    
    #constructor new &parent:aParent &caption:aCaption &onClick:anAction
        <= (new)
    [
        theParent := aParent.
        theCaption := aCaption wide.
        theClick := anAction.
    ]
    
    #method open
    [
        #var(type:hmenu)hmenu := theParent hmenu.
        
        ($nil == theItems)
            ? [
                
            ]
            ! [
                $super open.
            ]
    ]
}

// --- Menu ---

#class Menu :: BaseMenu
{
    #field theParent.
    #field(type:hmenu)theHandle.
    
    #constructor new
        <= (new)
    [
    ]
    
    #method control = IControl
    {
        #method object = self.
 
        #method open [ self open. ]
          
        #method close [ self close. ]
          
        #method retrieve &hwnd:aHandle = $nil.
    }.
    
    #method parent = theParent.
    
    #method hmenu = theHandle.
    
    #method $set &parent : aParent
    [
        theParent := aParent.
    ]
    
    #method open
    [
        theHandle := HMENU newRoot.
        
        $super open.
        
        theParent hwnd set &hmenu:theHandle.
    ]
    
    #method close
    [
        theHandle free.
        theHandle := $nil.
    ]
}