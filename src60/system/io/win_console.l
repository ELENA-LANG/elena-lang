namespace io
{
   import system'text;

   const IntNumber FILE_TYPE_CHAR = 2;

   struct KEY_EVENT_RECORD
   {
      int   bKeyDown : prop;
      short wRepeatCount : prop;
      short wVirtualKeyCode : prop;
      short wVirtualScanCode : prop;
      short uChar : prop;
      int   dwControlKeyState : prop;
   }

   struct INPUT_RECORD
   {
      short eventType : prop;
      short dummy;
      KEY_EVENT_RECORD KeyEvent : prop;
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

      newWriter()
      {
         handle h := ConsoleFactory.OutputHandle;
         int    dwMode := extern KERNEL32.GetFileType(h);
         if (dwMode == FILE_TYPE_CHAR) {
            ^ new ConsoleWriter(h)
         }
         else {
            ^ new ConsoleFileWriter(h)            
         }
      }

      newReader()
         = new ConsoleReader(ConsoleFactory.InputHandle);
   }

   sealed class ConsoleFileWriter
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

            OEMEncoder.toByteArray(literal, index, ref subs_len, buffer, 0, ref buf_len);

            self.write(buffer, buf_len);

            index += subs_len;
            length -= subs_len
         }
      }      
   }

   public class ConsoleWriter
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
   }

   public class ConsoleReader
   {
      handle h;

      constructor(handle h)
      {
         this h := h
      }

      char read()
      {
         handle h := extern KERNEL32.GetStdHandle(0FFFFFFF6h);

         INPUT_RECORD dump;
         KEY_EVENT_RECORD rec;

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

   public stdConsoleWriter = ConsoleFactory.newWriter();
   public stdConsoleReader = ConsoleFactory.newReader();
}
