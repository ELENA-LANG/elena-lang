import system'text;

const int OFN_PATHMUSTEXIST = 00000800h;
const int OFN_EXPLORER      = 00080000h;
const int OFN_LONGNAMES     = 00200000h;
const int OFN_HIDEREADONLY  = 00000004h;

const int DS_CENTER         = 0800h;

// --- messageBox ---

WindowHandle defaultHandle
    = new WindowHandle();

public singleton messageBox
{
    int open(WindowHandle parent, wide caption, wide message, int flags)
    {
        int retVal := extern USER32.MessageBoxW(parent, message, caption, flags);
            
        ^ new IntNumber(retVal)
    }
    
    int open(string caption, string message, int flags)
    {
        WindowHandle handle := cast WindowHandle(forward program.MainWindow) /*\ back:*/ ?? defaultHandle;
        
        ^ messageBox.open(handle, cast wide(caption), cast wide(message), flags)
    }
}

// --- BaseFileDialog ---

class BaseFileDialog
{
    WindowHandle theParent;
    short[]      theFilters;
    short[]      theFileName;
    int          theFilterIndex;
    int          theMaxFile;
    wide         theCaption;
    int          theFlags;
     
    constructor create(WindowHandle parent, wide[] filters, int filterIndex, int maxFile, wide caption, int flags)
    {
        int   s_length := 0;
        int   buf_size := 0;
        
        theParent := parent;
        theFilterIndex := filterIndex;
        
        theFileName := new short[](maxFile + 1);
        theFileName[0] := 0;
        
        theMaxFile := maxFile;
        theCaption := caption;
        theFlags := flags;
        
        // calculate the filter buffer size
        int length := filters.Length;
        for (int i := 0, i < length, i += 1)
        {
            wide filter := filters[i];
            s_length := filter.Length;
            buf_size := buf_size + s_length;
            buf_size := buf_size + 1
        };
        buf_size := buf_size + 2;

        // fill the filter buffer        
        theFilters := new short[](buf_size);
        buf_size := 0;
        for (int i := 0, i < length, i += 1)
        {
            wide filter := filters[i];
            s_length := filter.Length;
            
            UTF16Encoding.convert(buf_size, s_length, filter, theFilters);
            
            buf_size := buf_size + s_length;
            
            theFilters[buf_size] := 0;
            
            buf_size := buf_size + 1
        };
        theFilters[buf_size] := 0;
        theFilters[buf_size + 1] := 0;
    }    
    
    internal prepare(byte[] struct)
    {
        dirty_ptr ptr := 0;
        
        byte b := 0;
        struct.fill(0, 76, b);
        
//      lStructSize;
        struct.write(0, 76);        
          
//      hwndOwner;
        ptr := new UnsafePointer(theParent);
        ptr.saveTo(struct, 4);
          
//      hInstance;
        ptr := new UnsafePointer(CurrentInstance);
        ptr.saveTo(struct, 8);

//        lpstrFilter;
        ptr := new UnsafePointer(theFilters);
        ptr.savePtrTo(struct, 12);

//        nFilterIndex;
        struct.write(24, theFilterIndex);

//        lpstrFile;
        ptr := new UnsafePointer(theFileName);
        ptr.savePtrTo(struct, 28);

//        nMaxFile;
        struct.write(32, theMaxFile);

//        lpstrTitle;
        ptr := new UnsafePointer(theCaption);
        ptr.savePtrTo(struct, 48);

//        Flags;
        struct.write(52, theFlags)
    }
}

// --- OpenFileDialog ---

public sealed class OpenFileDialog : BaseFileDialog
{
    constructor(WindowHandle parent, wide[] filters, int filterIndex, int maxFile, wide caption)
        <= create(parent, filters, filterIndex, maxFile, caption, 
                    OFN_PATHMUSTEXIST || OFN_EXPLORER || OFN_LONGNAMES || DS_CENTER || OFN_HIDEREADONLY);
    
    select()
    {
        byte struct[76];
        
        self.prepare(struct);
        // NOTE: !! it should be any object creation between two lines
        int retVal := extern Comdlg32.GetOpenFileNameW(struct);

        if (retVal == 0)
        { 
            ^ nil 
        }
        else
        {
            int length := extern KERNEL32.lstrlenW(theFileName);
            
            var s := UTF16Encoding.toWide(0, length, theFileName); 
            
            ^ s
        }                
    }
}

// --- SaveFileDialog ---

public sealed class SaveFileDialog : BaseFileDialog
{
    constructor(WindowHandle parent, wide[] filters, int filterIndex, int maxFile, wide caption)
        <= create(parent, filters, filterIndex, maxFile, caption,
            OFN_PATHMUSTEXIST || OFN_EXPLORER || OFN_LONGNAMES || DS_CENTER || OFN_HIDEREADONLY);
    
    select()
    {
        byte struct[76];
        
        self.prepare(struct);
        // NOTE: !! it should be any object creation between two lines
        int retVal := extern Comdlg32.GetSaveFileNameW(struct);
        
        if (retVal == 0)
        { 
            ^ nil 
        }
        else
        {
            int length := extern KERNEL32.lstrlenW(theFileName);
            
            var s := UTF16Encoding.toWide(0, length, theFileName); 
            
            ^ s
        }
    }
}
