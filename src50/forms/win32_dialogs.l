import system'winforms;

// --- File Dialog ---

class FileDialog
{
    WindowHandle theParent;
    wide         theTitle;
    wide[]       theFilters;
    object       theSelectedPath;

    string Caption
    {
        get()
            = theTitle;
            
        set(string s)
        {
            theTitle := s
        }
        
        set(wide s)
        {
            theTitle := s
        }
    }
            
    addFilter(string caption, string mask)
    {
        wide[] subarr := new wide[]{ cast wide(caption), cast wide(mask) };
        
        if (theFilters == nil)
        {
            theFilters := subarr
        }
        else
        {
            theFilters := theFilters + subarr
        }
    }
    
    string Selected
        = theSelectedPath;
    
    constructor(parent)
    {
        theParent := parent
    }
}

////// --- OpenFileDialog ---
////
////#class OpenFileDialog
////{
////    #method run°
////    [
////    ]    
////}

// --- SaveFileDialog ---

public sealed class SaveFileDialog : FileDialog
{
    constructor new(parent)
        <= new(parent);
    
    bool run()
    {
        var dialog := new system'winforms'SaveFileDialog(theParent, theFilters, 0, 255, theTitle);
   
        theSelectedPath := dialog.select();
        
        ^ (nil != theSelectedPath)
    }    
}

// --- messageDialog ---

public singleton messageDialog
{
//    open parent:aParent caption:aCaption information:aText
//    [
//        messageBox open hwnd:(aParent hwnd) literal:aCaption literal:aText int:MB_ICONINFORMATION.
//    ]
//    
//    open caption:aCaption information:aText
//    [
//        messageBox open literal:aCaption literal:aText int:MB_ICONINFORMATION.
//    ]
//    
//    open parent:aParent caption:aCaption exclamation:aText
//    [
//        messageBox open hwnd:(aParent hwnd) literal:aCaption literal:aText int:MB_ICONEXCLAMATION.
//    ]    
//    
//    open caption:aCaption exclamation:aText
//    [
//        messageBox open literal:aCaption literal:aText int:MB_ICONEXCLAMATION.
//    ]    
//    
//    open parent:aParent caption:aCaption warning:aText
//    [
//        messageBox open hwnd:(aParent hwnd) literal:aCaption literal:aText int:MB_ICONWARNING
//    ]    
//    
//    open caption:aCaption warning:aText
//    [
//        messageBox open literal:aCaption literal:aText int:MB_ICONWARNING.
//    ]    
//    
//    open parent:aParent caption:aCaption question:aText
//    [
//        var result := messageBox open hwnd:(aParent hwnd) literal:aCaption literal:aText int:(MB_ICONQUESTION || MB_YESNO).
//        
//        ^ result == IDYES
//    ]
    
    showQuestion(string caption, string text)
    {
        int result := messageBox.open(caption, text, MB_ICONQUESTION || MB_YESNO);
        
        ^ result == IDYES
    }
}
