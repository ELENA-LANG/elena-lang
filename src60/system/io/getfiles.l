import system'text;
import system'collections;
    
namespace io
{

#if (__project["_Win"]) // Windows

   const int INVALID_HANDLE_VALUE = -1;
   const int ERROR_FILE_NOT_FOUND = 2;
   const int ERROR_NO_MORE_FILES = 012h;
    
   struct FILETIME
   {
      int dwLowDateTime;
      int dwHighDateTime;
   }
    
   packed WIN32_FIND_DATA
   {
      int      dwFileAttributes;
      FILETIME ftCreationTime;
      FILETIME ftLastAccessTime;
      FILETIME ftLastWriteTime;
      int      nFileSizeHigh;
      int      nFileSizeLow;
      int      dwReserved0;
      int      dwReserved1;
      short    cFileName[260];
      short    cAlternateFileName[14]; 
        
      bool isFile()
      {
         int attr := dwFileAttributes;
            
         attr := attr & FILE_ATTRIBUTE_DIRECTORY;
            
         ^ attr == 0
      }
        
      bool isDirectory()
      {
         int attr := dwFileAttributes;
            
         attr := attr & FILE_ATTRIBUTE_DIRECTORY;
            
         ^ attr != 0
      }
        
      get string FileName()
      {
         short[] s := cFileName;
         int len := extern KERNEL32.lstrlenW(s);
            
         // ^ UTF16Encoding.toString(0, len, s)
         var w := WideString.fromShortArray(0, len, s);
         ^ cast string(w)        
      }
   }
    
   public sealed class FileEnumerator : Enumerator<WIN32_FIND_DATA>
   {
      WIN32_FIND_DATA? ffd;
      handle           hFind;  
      bool             open;
      wide             mask;
        
      constructor assign(string mask)
      {
         this mask := mask;
      }
        
      bool next()
      {
         if (ffd == nil)
         {
            ffd := new WIN32_FIND_DATA();
                
            hFind := extern KERNEL32.FindFirstFileW(mask, ffd);
            if (INVALID_HANDLE_VALUE == hFind)
            {
               open := false;
                    
               int err := extern KERNEL32.GetLastError();
               if (err != ERROR_FILE_NOT_FOUND)
               {
                  IOException.new("FindFirstFile - " + err.toPrintable()).raise()
               }                
            }
            else
            {
               open := true
            }
         }
         else if(open) 
         {
            int retVal := extern KERNEL32.FindNextFileW(hFind, ffd);
            if (retVal == 0)
            {
               open := false;
               extern KERNEL32.FindClose(hFind);
                    
               int err := extern KERNEL32.GetLastError();
               if (err != ERROR_NO_MORE_FILES)
               {
                  IOException.new("FindFirstFile - " + err.toPrintable()).raise()
               }                
            }    
         };
                
         ^ open                        
      }
        
      get WIN32_FIND_DATA? Value()
         = ffd;
            
      enumerable()
         = mask;            
            
      reset()
      {
         open := false;
         ffd := nil
      }
        
      Enumerator cast() = new Enumerator{ embeddable dispatch() => self; };
   }
    
   public SearchFiles(string path, string mask, List<string> output)
   {
      WIN32_FIND_DATA ffd := default;
      handle          hFind;  
        
      wide criteria := path + "\" + mask;
        
      hFind := extern KERNEL32.FindFirstFileW(criteria, ffd);
      var b := true;
      if (INVALID_HANDLE_VALUE == hFind)
      {
         int err := extern KERNEL32.GetLastError();
         if (err != ERROR_FILE_NOT_FOUND)
         {
            IOException.new("FindFirstFile - " + err.toPrintable()).raise();
         };
            
         b := false;
      };
        
      auto buffer := new TextBuilder();
      while (b)
      {        
         if (ffd.isFile())
         {
            buffer.clear();
            buffer.append(path);
            buffer.append("\");
            buffer.append(ffd.FileName);
                
            output.append(buffer.Value)
         };
            
         int retVal := extern KERNEL32.FindNextFileW(hFind, ffd);
         if (retVal == 0)
         {
            extern KERNEL32.FindClose(hFind);
                
            int err := extern KERNEL32.GetLastError();
            if (err != ERROR_NO_MORE_FILES)
            {
               IOException.new("FindFirstFile - " + err.toPrintable()).raise()
            };
                
            b := false
         }
      }
   }
    
   public SearchDirectories(string path, string mask, List<string> output)
   {
      WIN32_FIND_DATA ffd := default;
      handle          hFind;  
        
      wide criteria := path + "\" + mask;
        
      hFind := extern KERNEL32.FindFirstFileW(criteria, ffd);
      var b := true;
      if (INVALID_HANDLE_VALUE == hFind)
      {
         int err := extern KERNEL32.GetLastError();
         if (err != ERROR_FILE_NOT_FOUND)
            IOException.new("FindFirstFile - " + err.toPrintable()).raise();
            
         b := false;
      };
        
      auto buffer := new TextBuilder();
      while (b)
      {
         if:not (ffd.isFile())
         {
            buffer.clear();
            buffer.append(path);
            buffer.append("\");
            buffer.append(ffd.FileName);
                
            output.append(buffer.Value)
         };
            
         int retVal := extern KERNEL32.FindNextFileW(hFind, ffd);
         if (retVal == 0)
         {
            extern KERNEL32.FindClose(hFind);
                
            int err := extern KERNEL32.GetLastError();
            if (err != ERROR_NO_MORE_FILES)
            {
               IOException.new("FindFirstFile - " + err.toPrintable()).raise()
            };
                
            b := false
         }
      }
   }

#elif (__project["_Linux"] || __project["_FreeBSD"])

   public SearchFiles(string path, string mask, List<string> output)
   {
      NotSupportedException.raise()
   }

}
