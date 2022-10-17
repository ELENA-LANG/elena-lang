public inline info<text>()
{
   #new super string description;

   #let description[$name __decl] := text;
}

B<T>
{
   T get() = nil;
}

A : B<int> // : info<"Class A">
{
   b() 
     : info<"Method b">
   {
   }
}

//class WithField<T>
//{
//   T field;
//}

//A : WithField<int>
//{
//   constructor()
//   {
//      field := 2;
//   }
//}

//A
//{
//   x := 2;

//   test()
//   {
//      console.writeLine(x)
//   }
//}

//A
//{
//   b 
//   {
//      get() = 2;
//   }
//}

//struct XY
//{
//
//}

//struct KEY_EVENT_RECORD
//{
//   int   bKeyDown;
//   short wRepeatCount;
//   short wVirtualKeyCode;
//   short wVirtualScanCode;
//   short uChar;
//   int   dwControlKeyState;
//}

//struct INPUT_RECORD
//{
//   short eventType;
//   KEY_EVENT_RECORD KeyEvent;
//} 

//readChar()
//{
//   handle h := extern KERNEL32.GetStdHandle(0FFFFFFF6h);

//            byte  dump[20];
//            int   numberRead := 0;
//            short eventType := 0;
//            short charCode := 0;
//            int   keyDown := 0;
    
//            bool found := false;
//            until (found)
//            {
//                extern
//                {
//                    extern KERNEL32.ReadConsoleInputW(handle, dump, 1, ref numberRead);
//                };

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
//}

public program() 
{
    var a := new A();
    a.test();

//    var b := a.b;

//    console.writeLine(b);

//    console.writeLine("Hello World!");
    
    // wait for any key
//    console.readChar()
}
