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
    
    constructor new(parent)
    {
        theParent := parent
    }
}

// --- OpenFileDialog ---

public sealed class OpenFileDialog : FileDialog
{
    constructor new(parent)
        <= new(parent);
    
    bool run()
    {
        var dialog := new system'winforms'OpenFileDialog(theParent, theFilters, 0, 255, theTitle);
   
        theSelectedPath := dialog.select();
        
        ^ (nil != theSelectedPath)
    }    
}

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
    showInformation(parent, caption, text)
    {
        messageBox.open(cast WindowHandle(parent), cast string(caption), cast string(text), MB_ICONINFORMATION)
    }
    
    showInformation(caption, text)
    {
        messageBox.open(cast string(caption), cast string(text), MB_ICONINFORMATION)
    }
    
    showExclamation(parent, caption, text)
    {
        messageBox.open(cast WindowHandle(parent), cast string(caption), cast string(text), MB_ICONEXCLAMATION)
    }
    
    showExclamation(caption, text)
    {
        messageBox.open(cast string(caption), cast string(text), MB_ICONEXCLAMATION)
    }
    
    showWarning(parent, caption, text)
    {
        messageBox.open(cast WindowHandle(parent), cast string(caption), cast string(text), MB_ICONWARNING)
    }
    
    showWarning(caption, text)
    {
        messageBox.open(cast string(caption), cast string(text), MB_ICONWARNING)
    }
    
    showQuestion(parent, caption, text)
    {
        var result := messageBox.open(cast WindowHandle(parent), cast string(caption), cast string(text), MB_ICONQUESTION || MB_YESNO);
        
        ^ result == IDYES
    }
    
    showQuestion(string caption, string text)
    {
        int result := messageBox.open(caption, text, MB_ICONQUESTION || MB_YESNO);
        
        ^ result == IDYES
    }
}
