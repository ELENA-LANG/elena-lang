import system'text;

namespace io
{
#if (__project["_Win"]) // Windows

   const IntNumber FILE_TYPE_CHAR = 2;

   packed KEY_EVENT_RECORD
   {
      int   bKeyDown : prop;
      short wRepeatCount : prop;
      short wVirtualKeyCode : prop;
      short wVirtualScanCode : prop;
      short uChar : prop;
      int   dwControlKeyState : prop;
   }

   packed INPUT_RECORD
   {
      short eventType : prop;
      short dummy;
      KEY_EVENT_RECORD KeyEvent : prop;
   } 

   sealed packed CONSOLE_SCREEN_BUFFER_INFO
   {
      COORD      dwSize;    
      COORD      dwCursorPosition;    
      short      wAttributes;
      SMALL_RECT srWindow;
      COORD      dwMaximumWindowSize;

      constructor()
      {
      }

      short SizeX
      {
         get()
         {
            short retVal := dwSize.X;
                
            ^ retVal
         }
      }
    
      short SizeY
      {
         get()
         {
            short retVal := dwSize.Y;
                
            ^ retVal
         }
      }
    
      readSizeTo(COORD r)
      {
         short x := dwSize.X;
         short y := dwSize.Y;
            
         r.write(x, y)
      }
    
      readCursorPositionTo(COORD r)
      {
         COORD copy := dwCursorPosition;
            
         short x := copy.X;
         short y := copy.Y;
            
         r.write(x, y)
      }
   }


   singleton ConsoleFactory
   {
      get handle OutputHandle()
      {
         handle h := extern KERNEL32.GetStdHandle(0FFFFFFF5h);

         ^ h;
      }

      get handle InputHandle()
      {
         handle h := extern KERNEL32.GetStdHandle(0FFFFFFF6h);

         ^ h;
      }

      IConsoleWriter newWriter()
      {
         handle h := ConsoleFactory.OutputHandle;
         int    dwMode := extern KERNEL32.GetFileType(h);
         if (dwMode == FILE_TYPE_CHAR) {
            ^ new ConsoleWriter(h)
         };

         ^ new ConsoleFileWriter(h)            
      }

      IConsoleReader newReader()
         = new ConsoleReader(ConsoleFactory.InputHandle);
   }

   sealed class ConsoleFileWriter : IConsoleWriter
   {
      handle h;

      private write(byte[] buffer, int length)
      {
         int written := 0;
         extern KERNEL32.WriteFile
            (h, buffer, length, ref written, 0)
      }

      constructor(handle h)
      {
         this h := h
      }

      write(wide s)
         <= write(cast string(s));

      write(string literal)
      {
         byte buffer[256];
         int length := literal.Length;
         int index := 0;
         int buf_len := 256;
         int subs_len := 256;
         while (length > 0)
         {
            buf_len := 256;

            if (length < 128)
            {
               subs_len := length
            }
            else
            {
               subs_len := 128
            };

            buf_len := OEMEncoder.toByteArray(literal, index, ref subs_len, buffer, 0, buf_len);

            self.write(buffer, buf_len);

            index += subs_len;
            length -= subs_len
         }
      }

      setCursorPosition(int x, int y)
      {
      }

      clear()
      {
      }

      refresh()
      {
      }
   }

   public class ConsoleWriter : IConsoleWriter
   {
      handle h;

      constructor(handle h)
      {
         this h := h
      }

      write(wide s)
      {
         int len := s.Length;
            
         extern KERNEL32.WriteConsoleW(h,s,len,0,0);
    
         // !!check the result of the operation
      }

      write(string s)
      {
         self.write(cast wide(s))
      }

      clear()
      {
         CONSOLE_SCREEN_BUFFER_INFO buffer := new CONSOLE_SCREEN_BUFFER_INFO();
         int  length := 0;
         int  retVal := 0;
    
         extern KERNEL32.GetConsoleScreenBufferInfo(h, buffer);
    
         short sizeX := buffer.SizeX;
         short sizeY := buffer.SizeY;
                    
         length := sizeX * sizeY;
            
         extern KERNEL32.FillConsoleOutputCharacterW(h, cast short(32), length, 0, ref retVal);
            
         extern KERNEL32.SetConsoleCursorPosition(h, 0)
      }

      setCursorPosition(int x, int y)
      {
         int coord := y * 10000h + x;
                    
         extern KERNEL32.SetConsoleCursorPosition(h, coord)
      }

      refresh()
      {
      }
   }

   public class ConsoleReader : IConsoleReader
   {
      handle h;

      constructor(handle h)
      {
         this h := h
      }

      char read()
      {
         INPUT_RECORD dump := default;
         KEY_EVENT_RECORD rec := default;

         int   numberRead := 0;
         short eventType := 0;
         short charCode := 0;
         int   keyDown := 0;
    
         bool found := false;
         until (found)
         {
            extern
            {
               extern KERNEL32.ReadConsoleInputW(h, dump, 1, ref numberRead);
            };

            eventType := dump.eventType;
                  
            if (eventType == 1)
            {
               rec := dump.KeyEvent;

               keyDown := rec.bKeyDown;

               found := keyDown == 1
            }
            else
            {
               found := false 
            };
         };

         charCode := rec.uChar;
    
         ^ system'text'UTF16Encoding.toChar(charCode)
      }

      private read(short[] dump, ref int retVal)
      {
         int len := retVal;
            
         extern
         {
            extern KERNEL32.ReadConsoleW(h, dump, len, ref /*retVal*/len, 0)
         };

         retVal := len // !! temporal
      }

      get bool KeyAvailable()
      {
         INPUT_RECORD     dump := default;
         KEY_EVENT_RECORD rec := default;

         int          numberRead := 0;
         short        eventType := 0;
         int          keyDown := 0;

         while (true)
         {
            numberRead := 0;

            extern
            {
               extern KERNEL32.PeekConsoleInputW(h, dump, 1, ref numberRead)
            };

            if (0 == numberRead)
               { ^ false };

            eventType := dump.eventType;
            if (eventType == 1)
            {
               rec := dump.KeyEvent;

               keyDown := rec.bKeyDown;

               if (keyDown == 1)
                  { ^ true }
            };

            extern
            {
               extern KERNEL32.ReadConsoleInputW(h, dump, 1, ref numberRead)
            }                
         };

         ^ false
      }

      wide readWideLine()
      {
         short dump[127];
         int   len := 0;
         wide  output := emptyWideString;

         bool continue := true;
         while (continue)
         {
            len := 127;
            self.read(dump, ref len);

            short sh := dump[len - 1];
            if (sh == 0Ah)
            {
               len := len - 2;

               continue := false
            };

            wide temp := WideString.fromShortArray(0, len, dump);

            output := output + temp;
         };

         ^ output
      }

      string readLine()
         = self.readWideLine();
   }

   public IConsoleWriter stdConsoleWriter = ConsoleFactory.newWriter();
   public IConsoleReader stdConsoleReader = ConsoleFactory.newReader();


#elif (__project["_Linux"] || __project["_FreeBSD"])

   const int STDIN_FILENO = 0;
   const int STDOUT_FILENO = 1;
    
   const int NCCS = 32;
   const int ICANON = 0000002;   /* Canonical input (erase and kill processing).  */
   const int VMIN = 6;
   const int TCSANOW = 0;
   const int ECHO = 0000010;   /* Enable echo.  */

   struct termios
   {
      int c_iflag;		/* input mode flags */
      int c_oflag;		/* output mode flags */
      int c_cflag;		/* control mode flags */
      int c_lflag;		/* local mode flags */
      int c_line;		/* line discipline */
      byte c_cc[/*NCCS*/32];		/* control characters */
      int c_ispeed;		/* input speed */
      int c_ospeed;		/* output speed */
    
      int LFlag
      {
         get()
            = c_lflag;
                
         set(v)
         {
            c_lflag := v
         }
      }
        
      setAt(int index, byte value)
      {
         c_cc[index] := value
      }
   }

   public class ConsoleWriter : IConsoleWriter
   {
      method write(string s)
      {
         int l := s.Length;
         extern libc.write(1, s, l);
      }

      write(wide s)
        <= write(cast string(s));

      setCursorPosition(int x, int y)
      {
      }

      clear()
      {
      }      

      refresh()
      {
//         extern libc.fflush(handle)
      }
   }

   public class ConsoleReader : IConsoleReader
   {
      char read()
      {
         termios savedState;   
         termios newState;

         extern libc.tcgetattr(STDIN_FILENO, savedState);

         newState := savedState;

         /* disable canonical input and disable echo.  set minimal input to 1. */
         newState.LFlag := newState.LFlag & ~(ECHO | ICANON);
         newState[VMIN] := 1;

         extern libc.tcsetattr(STDIN_FILENO, TCSANOW, newState);

         int ch := extern libc.getchar();      /* block (withot spinning) until we get a keypress */

            /* restore the saved state */
         extern libc.tcsetattr(STDIN_FILENO, TCSANOW, savedState);
                        
         ^ CharValue.load(ch);
      }

      private int read(byte[] buffer, int len)
      {
         int retVal := 0;

         for (int i := 0; i < len; i++)
         {
            retVal += 1;

            int ch := extern libc.getchar();

            byte b := ByteConvertor.convert(ch);
    
            buffer[i] := b;
    
            if(ch == 0Ah)
            {
               ^ retVal
            }
         };

         ^ retVal
      }

      string readLine()
      {
         byte buffer[127];
         int length := 0;
         var output := EmptyString;
            
         bool continue := true;
         while(continue)
         {
            length := 127;
            length := self.read(buffer, length);
    
            byte ch := buffer[length - 1];
    
            if(ch == 0Ah)
            {
               length := length - 1;
                    
               continue := false
            };
                
            var temp := String.fromByteArray(0, length, buffer);
                
            output := output + temp;
         };
    
         ^ output
      }

      wide readWideLine()
         = cast wide(self.readLine());

      get bool KeyAvailable()
      {
         ^ false
      }
   }

   public stdConsoleWriter = new ConsoleWriter();
   public stdConsoleReader = new ConsoleReader();

#endif
}
