import system;
import system'text;

public singleton messageBox
{
   int open(WindowHandle parent, wide caption, wide message, int flags)
   {
      int retVal := extern USER32.MessageBoxW(parent.Handle, message, caption, flags);
            
      ^ retVal
   }
    
   int open(string caption, string message, int flags)
   {
      WindowHandle handle := cast WindowHandle(forward program.MainWindow) ?? defaultHandle;
        
      ^ messageBox.open(handle.Handle, cast wide(caption), cast wide(message), flags)
   }
}

struct tagOFNA
{
   int         lStructSize : prop;
   Handle      hwndOwner : prop;
   handle      hInstance : prop;
   pointer     lpstrFilter : prop;
   pointer     lpstrCustomFilter : prop;
   int         nMaxCustFilter : prop;
   int         nFilterIndex : prop;
   pointer     lpstrFile : prop;
   int         nMaxFile : prop;
   pointer     lpstrFileTitle : prop;
   int         nMaxFileTitle : prop;
   pointer     lpstrInitialDir : prop;
   pointer     lpstrTitle : prop;
   int         Flags : prop;
   short       nFileOffset : prop;
   short       nFileExtension : prop;
   pointer     lpstrDefExt : prop;
   int         lCustData : prop;
   pointer     lpfnHook : prop;
   pointer     lpTemplateName : prop;
   pointer     lpEditInfo : prop;
   pointer     lpstrPrompt : prop;
   pointer     pvReserved : prop;
   int         dwReserved : prop;
   int         FlagsEx : prop;
}

// --- BaseFileDialog ---

class BaseFileDialog
{
   WindowHandle _parent;
   short[]      _filters;
   short[]      _fileName;
   int          _filterIndex;
   int          _maxFile;
   wide         _caption;
   int          _flags;
     
   constructor create(WindowHandle parent, wide[] filters, int filterIndex, int maxFile, wide caption, int flags)
   {
      int   s_length := 0;
      int   buf_size := 0;
        
      _parent := parent;
      _filterIndex := filterIndex;
        
      _fileName := new short[](maxFile + 1);
      _fileName[0] := 0;
        
      _maxFile := maxFile;
      _caption := caption;
      _flags := flags;
        
      // calculate the filter buffer size
      int length := filters.Length;
      for (int i := 0; i < length; i += 1)
      {
         wide filter := filters[i];
         s_length := filter.Length;
         buf_size := buf_size + s_length;
         buf_size := buf_size + 1
      };
      buf_size := buf_size + 2;

      // fill the filter buffer        
      _filters := new short[](buf_size);
      buf_size := 0;
      for (int i := 0; i < length; i += 1)
      {
         wide filter := filters[i];
         s_length := filter.Length;
            
         UTF16Encoding.toShortArray(filter, 0, s_length, _filters, buf_size);

         buf_size := buf_size + s_length;
            
         _filters[buf_size] := 0;
            
         buf_size := buf_size + 1
      };
      _filters[buf_size] := 0;
      _filters[buf_size + 1] := 0;
   }    
    
   internal prepare(tagOFNA struct)
   {
      struct.lStructSize := 76;
      struct.hwndOwner := _parent.Handle;
      struct.hInstance := CurrentInstance.Handle;
      struct.lpstrFilter := _filters;
      struct.nFilterIndex := _filterIndex;
      struct.lpstrFile := _fileName;
      struct.nMaxFile := _maxFile;
      struct.lpstrTitle := _caption;
      struct.FlagsEx := _flags;
   }
}

// --- OpenFileDialog ---

public sealed class OpenFileDialog : BaseFileDialog
{
   constructor(WindowHandle parent, wide[] filters, int filterIndex, int maxFile, wide caption)
      <= super create(parent, filters, filterIndex, maxFile, caption, 
            OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_LONGNAMES | DS_CENTER | OFN_HIDEREADONLY);
    
   select()
   {
      tagOFNA struct := default;

      self.prepare(struct);
      // NOTE: !! it should be any object creation between two lines
      int retVal := extern Comdlg32.GetOpenFileNameW(struct);

      if (retVal == 0)
      { 
         ^ nil 
      }
      else
      {
         int length := extern KERNEL32.lstrlenW(_fileName);
            
         var s := UTF16Encoding.toWideString(0, length, _fileName); 
            
         ^ s
      }                
    }
}

// --- SaveFileDialog ---

public sealed class SaveFileDialog : BaseFileDialog
{
   constructor(WindowHandle parent, wide[] filters, int filterIndex, int maxFile, wide caption)
      <= super create(parent, filters, filterIndex, maxFile, caption,
         OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_LONGNAMES | DS_CENTER | OFN_HIDEREADONLY);
    
   select()
   {
      tagOFNA struct := default;

      self.prepare(struct);
      // NOTE: !! it should be any object creation between two lines
      int retVal := extern Comdlg32.GetSaveFileNameW(struct);
        
      if (retVal == 0)
      { 
         ^ nil 
      }
      else
      {
         int length := extern KERNEL32.lstrlenW(_fileName);
            
         var s := UTF16Encoding.toWideString(0, length, _fileName); 
            
         ^ s
      }
    }
}
