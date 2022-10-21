


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

readChar()
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

                   console.writeLine(keyDown);
                    
                   found := keyDown == 1
                }
                else
                {
                    found := false 
                };
            };
    
//            ^ system'text'UTF16Encoding.toChar(charCode)
}

public program() 
{
//   INPUT_RECORD dump;
//   KEY_EVENT_RECORD rec;

//   rec := dump.KeyEvent;


   readChar();

    // wait for any key
//    console.readChar()
}
