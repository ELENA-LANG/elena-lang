namespace io
{
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
}
