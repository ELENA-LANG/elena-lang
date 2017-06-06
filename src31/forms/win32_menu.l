#import system.
#import system'collections.
#import system'winforms.

// --- BaseMenu ---

#class BaseMenu
{
    #field theParent.
    #field(list) theItems.
    
    #method items =
    {
        getAt : anIndex = $self getAt &int:(anIndex int).
        
        getAt &int:anIndex => theItems.        
        
        append &caption:aCaption &onClick:anAction
        [
            ($nil == theItems)
                ? [ theItems := List new. ].
            
            theItems append:(MenuItem new &parent:$owner &caption:aCaption &onClick:anAction).
        ]
        
        append &caption:aCaption
        [
            $self append &caption:aCaption &onClick:$nil.
        ]
    }.
    
    #method open
    [
        #var(enumerator)enum := theItems enumerator.
        #loop (enum next)?
        [
            enum get open.
        ].
    ]    
    
    #method retrieveMenu &int:anID
    [
        #var aRetVal.
        #var(enumerator)enum := theItems enumerator.
        #loop (enum next)?
        [
            aRetVal := enum get retrieveMenu &int:anID.
            ($nil == aRetVal)
                ! [ ^ aRetVal. ].
        ].
       
        ^ $nil. 
    ]    
            
    #method close
    [
        #var(enumerator)enum := theItems enumerator.
        #loop (enum next)?
        [
            enum get close.
        ].
    ]    
}

// --- MenuItem ---

#class MenuItem :: BaseMenu
{
    #field(func1)theClick.
    #field(wide) theCaption.
    #field(hmenu)theMenuHandle.
    #field(int) theID.
    
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
    
    #method retrieveMenu &int:anID
    [
        (theID == anID int)
            ? [ ^ $self. ].
            
        ($nil != theItems)?
            [ ^ $super retrieveMenu &int:anID. ].
            
        ^ $nil.            
    ]
    
    #method click 
    [
        ($nil != theClick)
            ? [ theClick eval:$self. ].
    ]
    
    #method open
    [
        #var(hmenu)hmenu := theParent hmenu.
        
        ($nil == theItems)
            ? [
                theMenuHandle := hmenu.
                
                theMenuHandle append &int:MF_STRING &int:theID &wide:theCaption.
            ]
            ! [
                theMenuHandle := HMENU new.
                hmenu append &int:MF_STRING &int:theMenuHandle &wide:theCaption.
                
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
    #field(hmenu)theHandle.
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
        #method object = $owner.
 
        #method open [ $owner open. ]
          
        #method close [ $owner close. ]
          
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