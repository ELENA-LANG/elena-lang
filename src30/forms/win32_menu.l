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
        getAt : anIndex = $self getAt &int:(anIndex int).
        
        getAt &int:anIndex => theItems.        
        
        append &caption:aCaption &onClick:anAction
        [
            ($nil == theItems)
                ? [ theItems := List new. ].
            
            theItems append:(MenuItem new &parent:self &caption:aCaption &onClick:anAction).
        ]
        
        append &caption:aCaption
        [
            $self append &caption:aCaption &onClick:$nil.
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
    
    #method retrieve &menu_id:anID
    [
        #var aRetVal.
        #var(type:enumerator)enum := theItems enumerator.
        #loop (enum next)?
        [
            aRetVal := enum get retrieve &menu_id:anID.
            ($nil == aRetVal)
                ! [ ^ aRetVal. ].
        ].
       
        ^ $nil. 
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
    #field(type:hmenu)theMenuHandle.
    #field(type:int) theID.
    
    #constructor new &parent:aParent &caption:aCaption &onClick:anAction
        <= (new)
    [
        theParent := aParent.
        theCaption := aCaption wide.
        
        ($nil == anAction)
            ! [ theClick := anAction func1. ].
        
        theID := theParent newID int.
    ]
    
    #method hmenu = theMenuHandle.
    
    #method newID => theParent.
    
    #method retrieve &menu_id:anID
    [
        (theID == anID int)
            ? [ ^ $self. ].
            
        ($nil != theItems)?
            [ ^ $super retrieve &menu_id:anID. ].
            
        ^ $nil.            
    ]
    
    #method click 
    [
        ($nil != theClick)
            ? [ theClick eval:$self. ].
    ]
    
    #method open
    [
        #var(type:hmenu)hmenu := theParent hmenu.
        
        ($nil == theItems)
            ? [
                theMenuHandle := hmenu.
                
                theMenuHandle append &int:MF_STRING &int:theID &wide:theCaption.
            ]
            ! [
                theMenuHandle := HMENU new.
                hmenu append &int:MF_STRING &handle:theMenuHandle &wide:theCaption.
                
                $super open.
            ].
    ]
    
    #method close
    [
        ($nil == theItems)
            ! [
                $super close.
                
                theMenuHandle free.
                theMenuHandle := $nil.
            ]
    ]
}

// --- Menu ---

#class Menu :: BaseMenu
{
    #field(type:hmenu)theHandle.
    #field theSeed.
    
    #constructor new
        <= (new)
    [
        theSeed := Integer new.
    ]
    
    #method newID
    [
        theSeed append &int:1.
        
        ^ theSeed int.
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
        
        theParent set &menu:$self.
    ]

    #method open
    [
        theHandle := HMENU new.
        
        $super open.
        
        theParent hwnd set &hmenu:theHandle.
    ]
    
    #method close
    [
        theHandle free.
        theHandle := $nil.
    ]
}