namespace io
{
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
      get /*handle*/ OutputHandle()
      {
         handle h := extern KERNEL32.GetStdHandle(0FFFFFFF5h);

         ^ h;
      }

      get /*handle*/ InputHandle()
      {
         handle h := extern KERNEL32.GetStdHandle(0FFFFFFF6h);

         ^ h;
      }
   }

   public class ConsoleWriter
   {
      handle h;

      constructor(handle h)
      {
         this h := h
      }

      write(string s)
      {
          int l := s.Length;
          extern KERNEL32.WriteConsoleA(h,s,l,0,0);
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
   }

   public stdConsoleWriter = new ConsoleWriter(ConsoleFactory.OutputHandle);
   public stdConsoleReader = new ConsoleReader(ConsoleFactory.InputHandle);
}
