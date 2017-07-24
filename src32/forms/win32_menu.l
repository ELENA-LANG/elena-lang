import system'collections.
import system'winforms.

// --- BaseMenu ---

class BaseMenu
{
    object theParent.
    list   theItems.
    
    items =
    {
        getAt : anIndex = $self getAt int:(anIndex int).
        
        getAt int:anIndex => theItems.        
        
        append caption:aCaption onClick:anAction
        [
            if($nil == theItems)
                [ theItems := List new ].
            
            theItems append:(MenuItem new parent:$owner caption:aCaption onClick:anAction).
        ]
        
        append caption:aCaption
        [
            $self append caption:aCaption onClick:$nil.
        ]
    }.
    
    open
    [
        enumerator enum := theItems enumerator.
        while (enum next)
        [
            enum get; open
        ].
    ]    
    
    retrieveMenu int:anID
    [
        var aRetVal := $nil.
        enumerator enum := theItems enumerator.
        while (enum next)
        [
            aRetVal := enum get; retrieveMenu int:anID.
            ifnot ($nil == aRetVal)
                [ ^ aRetVal ]
        ].
       
        ^ $nil. 
    ]    
            
    close
    [
        enumerator enum := theItems enumerator.
        while (enum next)
        [
            enum get; close
        ].
    ]    
}

// --- MenuItem ---

class MenuItem :: BaseMenu
{
    func1 theClick.
    wide  theCaption.
    hmenu theMenuHandle.
    int   theID.
    
    constructor new parent:aParent caption:aCaption onClick:anAction
        <= new;
    [
        theParent := aParent.
        theCaption := aCaption wide.
        
        ifnot($nil == anAction)
            [ theClick := anAction func1 ].
        
        theID := theParent newID; int.
    ]
    
    hmenu = theMenuHandle.
    
    newID => theParent.
    
    retrieveMenu int:anID
    [
        if(theID == anID int)
            [ ^ $self ].
            
        if($nil != theItems)
            [ ^ $super retrieveMenu int:anID ].
            
        ^ $nil
    ]
    
    click 
    [
        if($nil != theClick)   
            [ theClick eval:$self ].
    ]
    
    open
    [
        hmenu hmenu := theParent hmenu.
        
        if ($nil == theItems)
            [
                theMenuHandle := hmenu.
                
                theMenuHandle append int:MF_STRING int:theID wide:theCaption.
            ];
            [
                theMenuHandle := HMENU new.
                hmenu append int:MF_STRING handle:theMenuHandle wide:theCaption.
                
                $super open.
            ].
    ]
    
    close
    [
        ifnot ($nil == theItems)
            [
                $super close.
                
                theMenuHandle free.
                theMenuHandle := $nil
            ]
    ]
}

// --- Menu ---

class Menu :: BaseMenu
{
    hmenu theHandle.
    object theSeed.
    
    constructor new
        <= new;
    [
        theSeed := Integer new.
    ]
    
    newID
    [
        theSeed append int:1.
        
        ^ theSeed int.
    ]
    
    control = IControl::
    {
        control_object = $owner.
 
        open [ $owner open ]
          
        close [ $owner close ]
          
        retrieve hwnd:aHandle = $nil.
    }.
    
    parent = theParent.
    
    hmenu = theHandle.
    
    $set parent : aParent
    [
        theParent := aParent.
        
        theParent set menu:$self.
    ]

    open
    [
        theHandle := HMENU new.
        
        $super open.
        
        theParent hwnd; set hmenu:theHandle.
    ]
    
    close
    [
        theHandle free.
        theHandle := $nil.
    ]
}