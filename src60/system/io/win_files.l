import system'text;

namespace io
{
   // --- File Access Modes ---
   const IntNumber FILE_READ_DATA     = 00000001h;
   const IntNumber FILE_APPEND_DATA   = 00000004h;
   const IntNumber FILE_GENERIC_READ  = 80000000h;
   const IntNumber FILE_GENERIC_WRITE = 40000000h;
    
   const IntNumber OPEN_EXISTING      = 0003h;
   const IntNumber CREATE_ALWAYS      = 0002h;
   const IntNumber OPEN_ALWAYS        = 0004h;
    
   const IntNumber FILE_SHARE_READ    = 00000001h;
   const IntNumber FILE_SHARE_WRITE   = 00000002h;
    
   const IntNumber FILE_BEGIN = 0;
   const IntNumber FILE_CURRENT = 1;
   const IntNumber FILE_END = 2;
    
   const IntNumber FILE_ATTRIBUTE_DIRECTORY = 10h;

   // --- FileStream ---
   public struct FileStream : Stream
   {
      handle _handle;
        
      constructor openForRead(path)
            <= new(cast wide(path), FILE_GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, 0);
        
      constructor openForReWrite(path)
            <= new(cast wide(path), FILE_GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS, 0);
        
      constructor openForAppend(path)
            <= new(cast wide(path), FILE_APPEND_DATA, FILE_SHARE_READ, OPEN_ALWAYS, 0);
        
      constructor openForEdit(path)
            <= new(cast wide(path), FILE_GENERIC_WRITE, FILE_SHARE_READ, OPEN_EXISTING, 0);
        
      constructor new(wide path, int dwDesiredAccess, int dwShareMode, int dwCreationDisposition, int dwFlagsAndAttributes)
      {
            _handle := extern KERNEL32.CreateFileW(
                          path,
                          dwDesiredAccess,
                          dwShareMode,
                          0,
                          dwCreationDisposition,
                          dwFlagsAndAttributes,
                          0);
                          
            if(-1 == _handle)
                { IOException.new("Cannot open the file").raise() }
      }
        
      write(byte[] dump, int length)
      {
            int written := 0;
            extern KERNEL32.WriteFile(
                _handle,
                dump,
                length,
                ref written,
                0)
      }
    
      read(byte[] dump, ref int retVal)
      {
         int toRead := retVal;
            
         extern KERNEL32.ReadFile(
            _handle,
            dump,
            toRead,
            //ref retVal,
            out int temp,
            0);

         retVal := temp
      }
    
      int Index
      {    
         get()
         {
            int ret := extern KERNEL32.SetFilePointer(
                    _handle,
                    0,
                    0,
                    FILE_CURRENT);
                    
            ^ ret
         } 
            
         set(index)
         {
            int ret := extern KERNEL32.SetFilePointer(
                        _handle,
                        index,
                        0,
                        FILE_BEGIN)
         }
      }
        
      int Length
      {
         get() 
         { 
            int n := extern KERNEL32.GetFileSize(
                              _handle,
                              0);
            
            ^ n 
         }
            
         set(length)
         {
            int pos := self.Index;
                
            self.Index := length;
            extern KERNEL32.SetEndOfFile(_handle);
                
            if(length > pos)
               { self.Index := pos }
         }
      }

      close()
      {
         extern KERNEL32.CloseHandle(_handle)
      }
   }

   singleton fileStreamHelper
   {
      Stream openForWrite(string path, bool appendMode)
      {
         if(appendMode) { ^ FileStream.openForAppend(path) };
            
         ^ FileStream.openForReWrite(path)
      }
   }

   singleton fileControl
   {
      TextReader newReader(path) = TextStreamReader.new(path);

      TextWriter writer(path) = StreamWriter.new(FileStream.openForReWrite(path), UTF8Encoder);

      TextWriter logger(path) = StreamWriter.new(FileStream.openForAppend(path), UTF8Encoder);

      bool isAvailable(path)
      {
         int ret := extern KERNEL32.GetFileAttributesW(cast wide(path));
            
         if(ret != 0FFFFFFFFh)
         {
            ret := ret & FILE_ATTRIBUTE_DIRECTORY;
                
            ^ ret == 0
         };
                
         ^ false
      }      

      bool delete(path)
      {
         wide s := cast wide(path); // !! temporal
            
         int ret := extern KERNEL32.DeleteFileW(/*cast wide(path)*/s);
            
         ^ ret != 0
      }
   }

   singleton directoryControl
   {
      bool isAvailable(path)
      {
         int ret := extern KERNEL32.GetFileAttributesW(cast wide(path));
         
         if(ret != 0FFFFFFFFh)
            {
               ret := ret & FILE_ATTRIBUTE_DIRECTORY;
                    
               ^ ret != 0
            };
                
         ^ false
      }    

      create(path)
      {
         wide s := cast wide(path); // !! temporal
            
         int ret := extern KERNEL32.CreateDirectoryW(
                /*cast wide(path)*/s,
                0);
                
         if(ret == 0)
            { IOException.new("Cannot create a directory").raise() }
      }
 
      delete(path)
      {
         wide s := cast wide(path); // !! temporal
            
         int ret := extern KERNEL32.RemoveDirectoryW(/*cast wide(path)*/s);
            
         ^ ret != 0
      }
   }
}