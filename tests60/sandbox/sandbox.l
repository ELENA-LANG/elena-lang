
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
   KEY_EVENT_RECORD KeyEvent : prop;
} 


readChar()
{
//   handle h := extern KERNEL32.GetStdHandle(0FFFFFFF6h);

   INPUT_RECORD dump;

//            int   numberRead := 0;
//            short eventType := 0;
//            short charCode := 0;
//            int   keyDown := 0;
    
//            bool found := false;
//            until (found)
//            while(!found)
//            {
//                extern
//                {
//                    extern KERNEL32.ReadConsoleInputW(handle, dump, 1, ref numberRead);
//                };

                  var w := dump.eventType;
                  //console.writeLine(w);

                  //console.writeLine(dump.eventType);

//                dump.read(0, ref eventType);
//                if (eventType == 1)
//                {
//                    dump.read(4, ref keyDown);
//                    
//                    found := keyDown == 1
//                }
//                else
//                {
//                    found := false 
//                };
//            };
    
//            dump.read(14, ref charCode);
            
//            ^ system'text'UTF16Encoding.toChar(charCode)
}

public program() 
{
   readChar();

    // wait for any key
//    console.readChar()
}
