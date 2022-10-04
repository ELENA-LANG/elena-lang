namespace io
{
   public class ConsoleWriter
   {
      write(string s)
      {
          int h := extern KERNEL32.GetStdHandle(0FFFFFFF5h);
          int l := s.Length;
          extern KERNEL32.WriteConsoleA(h,s,l,0,0);
      }
   }

   public class ConsoleReader
   {
      char read()
      {
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
      }
   }
}