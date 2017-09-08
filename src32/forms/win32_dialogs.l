import system'winforms.

// --- File Dialog ---

class FileDialog
{
    object theParent.
    object theTitle.
    object theFilters.
    object theSelectedPath.
    
    caption = theTitle.

    selected = theSelectedPath.
            
    set caption:aTitle
    [
        theTitle := aTitle.
    ]
    
    addFilter caption:aCaption extension_mask:aMask
    [
        if($nil == theFilters)
            [ theFilters := (aCaption wide, aMask wide) ];
            [
                theFilters := theFilters + (aCaption wide, aMask wide)
            ]
    ]
    
    constructor new : aParent
    [
        theParent := aParent.
    ]
}

//// --- OpenFileDialog ---
//
//#class OpenFileDialog
//{
//    #method run°
//    [
//    ]    
//}

// --- SaveFileDialog ---

sealed class SaveFileDialog :: FileDialog
{
    constructor new : aParent
        <= new:aParent.
    
    run
    [
        var aDialog := system'winforms'SaveFileDialog new hwnd:(theParent hwnd) array:theFilters int:0 int:255 wide:(theTitle wide).
        
        theSelectedPath := aDialog select.
        
        ^ ($nil != theSelectedPath)
    ]    
}

// --- messageDialog ---

singleton messageDialog =
{
    open parent:aParent caption:aCaption information:aText
    [
        messageBox open hwnd:(aParent hwnd) literal:aCaption literal:aText int:MB_ICONINFORMATION.
    ]
    
    open caption:aCaption information:aText
    [
        messageBox open literal:aCaption literal:aText int:MB_ICONINFORMATION.
    ]
    
    open parent:aParent caption:aCaption exclamation:aText
    [
        messageBox open hwnd:(aParent hwnd) literal:aCaption literal:aText int:MB_ICONEXCLAMATION.
    ]    
    
    open caption:aCaption exclamation:aText
    [
        messageBox open literal:aCaption literal:aText int:MB_ICONEXCLAMATION.
    ]    
    
    open parent:aParent caption:aCaption warning:aText
    [
        messageBox open hwnd:(aParent hwnd) literal:aCaption literal:aText int:MB_ICONWARNING
    ]    
    
    open caption:aCaption warning:aText
    [
        messageBox open literal:aCaption literal:aText int:MB_ICONWARNING.
    ]    
    
    open parent:aParent caption:aCaption question:aText
    [
        var result := messageBox open hwnd:(aParent hwnd) literal:aCaption literal:aText int:(MB_ICONQUESTION || MB_YESNO).
        
        ^ result == IDYES
    ]
    
    open caption:aCaption question:aText
    [
        var result := messageBox open literal:aCaption literal:aText int:(MB_ICONQUESTION || MB_YESNO).
        
        ^ result == IDYES
    ]
}.
