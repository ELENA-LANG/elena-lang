#define system.
#define system'winforms.

// --- File Dialog ---

#class FileDialog
{
    #field theParent.
    #field theTitle.
    #field theFilters.
    #field theSelectedPath.
    
    #method caption = theTitle.

    #method selected = theSelectedPath.
            
    #method set &caption:aTitle
    [
        theTitle := aTitle.
    ]
    
    #method addFilter &caption:aCaption &extension:aMask
    [
        (theFilters == nil)
            ? [ theFilters := (aCaption wide, aMask wide). ]
            ! [
                theFilters := theFilters + (aCaption wide, aMask wide).
            ].
    ]
    
    #constructor new : aParent
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

#class(sealed) SaveFileDialog :: FileDialog
{
    #method run
    [
        #var aDialog := system'winforms'SaveFileDialog new &hwnd:(theParent hwnd) &filters:theFilters &filter_index:0 &length:255 &wide:(theTitle wide).
        
        theSelectedPath := aDialog select.
        
        ^ (nil != theSelectedPath).
    ]    
}
