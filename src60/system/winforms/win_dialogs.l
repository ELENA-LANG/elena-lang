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
   pointer     lCustData : prop;
   pointer     lpfnHook : prop;
   pointer     lpTemplateName : prop;
   pointer     pvReserved : prop;
   int         dwReserved : prop;
   int         FlagsEx : prop;
}

// --- BaseFileDialog ---

class BaseFileDialog
{
   WindowHandle _parent;
   wide[]       _filters;
   int          _filterIndex;
   wide         _caption;
   int          _flags;
     
   constructor create(WindowHandle parent, wide[] filters, int filterIndex, wide caption, int flags)
   {
      _parent := parent;
      _filters := filters;
      _filterIndex := filterIndex;
        
      _caption := caption;
      _flags := flags;        
   }    
    
   internal fillFilters(short[] buffer, int size)
   {
      int   s_length := 0;
      int   buf_size := 0;
        
      // calculate the filter buffer size
      int length := _filters.Length;
      for (int i := 0; i < length; i += 1)
      {
         wide filter := _filters[i];
         s_length := filter.Length;
         buf_size := buf_size + s_length;
         buf_size := buf_size + 1
      };
      buf_size := buf_size + 2;
      if (buf_size > size) {
         InvalidArgumentException.raise("Invalid argument size - filters are too long");
      };

      // fill the filter buffer        
      buf_size := 0;
      for (int i := 0; i < length; i += 1)
      {
         wide filter := _filters[i];
         s_length := filter.Length;
            
         UTF16Encoding.toShortArray(filter, 0, s_length, buffer, buf_size);

         buf_size := buf_size + s_length;
            
         buffer[buf_size] := 0;
            
         buf_size := buf_size + 1
      };
      buffer[buf_size] := 0;
      buffer[buf_size + 1] := 0;
   }

   internal prepare(tagOFNA struct, short[] filters, short[] fileName, int maxFile)
   {
      struct.lStructSize := $size struct;
      struct.hwndOwner := _parent.Handle;
      struct.hInstance := CurrentInstance.Handle;
      struct.lpstrFilter := filters;
      struct.nFilterIndex := _filterIndex;
      struct.lpstrFile := fileName;
      struct.nMaxFile := maxFile;
      struct.lpstrTitle := _caption;
      struct.Flags := _flags;
   }
}

// --- OpenFileDialog ---

public sealed class OpenFileDialog : BaseFileDialog
{
   constructor(WindowHandle parent, wide[] filters, int filterIndex, wide caption)
      <= super create(parent, filters, filterIndex, caption, 
            OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_LONGNAMES | OFN_HIDEREADONLY);
    
   select()
   {
      short filters[1024];
      self.fillFilters(filters, 1024);

      short fileName[1024];
      fileName[0] := 0;

      tagOFNA struct := default;
      self.prepare(struct, filters, fileName, 1024);

      int retVal := 0;
      extern 
      {
         retVal := extern Comdlg32.GetOpenFileNameW(struct);
      };

      if (retVal == 0)
      { 
         ^ nil 
      }
      else
      {
         int length := extern KERNEL32.lstrlenW(fileName);
            
         var s := UTF16Encoding.toWideString(0, length, fileName); 
            
         ^ s
      }                
    }
}

// --- SaveFileDialog ---

public sealed class SaveFileDialog : BaseFileDialog
{
   constructor(WindowHandle parent, wide[] filters, int filterIndex, wide caption)
      <= super create(parent, filters, filterIndex, caption,
         OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_LONGNAMES | OFN_HIDEREADONLY);
    
   select()
   {
      short filters[1024];
      self.fillFilters(filters, 1024);

      short fileName[1024];
      fileName[0] := 0;

      tagOFNA struct := default;
      self.prepare(struct, filters, fileName, 1024);

      int retVal := 0;
      extern 
      {
         retVal := extern Comdlg32.GetSaveFileNameW(struct);
      };
        
      if (retVal == 0)
      { 
         ^ nil 
      }
      else
      {
         int length := extern KERNEL32.lstrlenW(fileName);
            
         var s := UTF16Encoding.toWideString(0, length, fileName); 
            
         ^ s
      }
    }
}
