import system'text;
import system'collections;

namespace io
{
#if (__project["_Win"]) // Windows

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
    
      int read(byte[] dump, int len)
      {
         extern KERNEL32.ReadFile(
            _handle,
            dump,
            len,
            //ref retVal,
            out int temp,
            0);

         ^ temp
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
      }

      indexed internal setLength(int length)
      {
         int pos := self.Index;
             
         self.Index := length;
         extern KERNEL32.SetEndOfFile(_handle);
             
         if(length > pos)
            { self.Index := pos }
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

      TextWriter newWriter(path) = StreamWriter.new(FileStream.openForReWrite(path), UTF8Encoder);

      TextWriter newWriter(string path, Encoder encoder) = StreamWriter.new(FileStream.openForReWrite(path), encoder);

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
 
      bool delete(path)
      {
         wide s := cast wide(path); // !! temporal
            
         int ret := extern KERNEL32.RemoveDirectoryW(/*cast wide(path)*/s);
            
         ^ ret != 0
      }
   }

#elif (__project["_Linux"] || __project["_FreeBSD"])

   const int F_OK     = 0;

   const int SEEK_SET = 0;
   const int SEEK_CUR = 1;
   const int SEEK_END = 2;

   const int S_IRWXU  = 448;
   const int S_IRGRP  = 32; 
   const int S_IXGRP  = 8;

   // --- FileStream ---
   public struct FileStream : Stream
   {
      handle _handle;
        
      constructor openForRead(path)
         <= new(path, "r+");
        
      constructor openForReWrite(path)
         <= new(path, "w+");
    
      constructor openForAppend(path)
         <= new(path, "a");

      constructor openForEdit(path)
            <= new(path, "w");
        
      constructor new(string path, string attributes)
      {
         _handle := extern libc.fopen
                          (path,
                          attributes);
                          
         if(-1 == _handle)
                { IOException.new("Cannot open the file").raise() }
      }
        
      write(byte[] dump, int length)
      {
         extern libc.fwrite(
            dump,
            1,
            length,
            _handle)
      }
    
      int read(byte[] dump, int length)
      {
         int n := extern libc.fread(
                     dump,
                     1,
                     length,
                     _handle);
    
         ^ n
      }
    
      int Index
      {    
         get()
         {
            int ret := extern libc.lseek(
                        _handle,
                        0,
                        SEEK_CUR);
                    
            ^ ret
         } 
            
         set(index)
         {
            int ret := extern libc.lseek(
                        _handle,
                        index,
                        SEEK_SET);
         }
      }
        
      int Length
      {
         get() 
         { 
            int ret := extern libc.lseek(
                        _handle,
                        0,
                        SEEK_END);
                    
                ^ ret

         }
      }

      indexed internal setLength(int length)
      {
         NotSupportedException.raise()
      }

      close()
      {
         extern libc.fclose(_handle)
      }
   }

   singleton fileControl
   {
      TextReader newReader(path) = TextStreamReader.new(path);

      TextWriter newWriter(path) = StreamWriter.new(FileStream.openForReWrite(path), UTF8Encoder);

      TextWriter newWriter(string path, Encoder encoder) = StreamWriter.new(FileStream.openForReWrite(path), encoder);

      TextWriter logger(path) = StreamWriter.new(FileStream.openForAppend(path), UTF8Encoder);

      bool isAvailable(string path)
      {
         int ret := extern libc.access(path, F_OK);
            
         ^ ret == 0
      }      

      bool delete(string path)
      {
         int ret := extern libc.remove(path);
            
         ^ ret == 0
      }
   }

   singleton directoryControl
   {
      create(string path)
      {
         int ret := extern libc.mkdir(path, S_IRWXU|S_IRGRP|S_IXGRP);

         if(ret != 0)
            { IOException.new("Cannot create a directory").raise() }
      }

      bool delete(string path)
      {
         int ret := extern libc.rmdir(path);

         ^ ret == 0
      }

      bool isAvailable(string path)
      {
         string s := path;
         if:not(path.endingWith("/"))
            s := path + "/";

         int ret := extern libc.access(path, F_OK);
            
         ^ ret == 0
      }      
   }

#endif

   public sealed class File
   {
      string _path;

      constructor assign(string path)
      {
         _path := path
      }

      string cast() = _path;
    
      wide cast() = _path;

      bool Available
         = fileControl.isAvailable(_path);

      TextReader textreader() = fileControl.newReader(_path);

      TextWriter textwriter() = fileControl.newWriter(_path);

      TextWriter logger() = fileControl.logger(_path);

      get int Length()
      {
         int    fileLen := 0;
         using(Stream stream := FileStream.openForRead(_path)) {
            fileLen := stream.Length;        
         };
            
         ^ fileLen
      }

      saveContent(string content)
      {
         File.saveContent(_path, content);
      }    

      string readContent()
         = File.readContent(_path);
        
      wide readWideContent()
         = File.readWideContent(_path);

      delete()
         = fileControl.delete(_path);

      static saveContent(string path, string content)
      {
         using(TextWriter writer := fileControl.newWriter(path)) {
            writer.write(content);
         }         
      }

      static saveContent(string path, string content, Encoder encoder)
      {
         using(TextWriter writer := fileControl.newWriter(path, encoder)) {
            writer.write(content);
         }         
      }

      static readContentTo(string path, TextBuilder output)
      {
         using(TextReader reader := fileControl.newReader(path)) {
            char       buffer[128];
            int        read := 0;
            while (reader.Available)
            {
               read := 128;
               reader.read(buffer, ref read);
                   
               output.write(buffer, read)
            };            
         }         
      }

      static string readContent(string path)
      {
         auto output := new TextBuilder();
            
         File.readContentTo(path, output);
            
         ^ output.Value
      }

      static wide readWideContent(string path)
      {
         auto output := new TextBuilder();
            
         File.readContentTo(path, output);
            
         ^ output.Value
      }
      
      static TextWriter textwriter(string path) 
         = fileControl.newWriter(path);         
      
      static TextReader textreader(string path) 
         = fileControl.newReader(path);         
      
   }

   // --- Directory ---
    
   public sealed class Directory
   {
      string _path;

      constructor assign(string path)
      {
         _path := path
      }

      bool Available
         = directoryControl.isAvailable(_path);    

      string[] getFiles()
         <= getFiles("*.*");
        
      string[] getFiles(string mask)
         = Directory.getFiles(_path, mask);

      create()
      {
         directoryControl.create(_path);
      }     

      delete()
         = directoryControl.delete(_path);

      static string[] getFiles(string path)
         = getFiles(path, "*.*");

      static string[] getFiles(string path, string mask)
      {
         auto l := new List<string>();
         SearchFiles(path, mask, l);
            
         ^l.Value;         
      }
   }   
}