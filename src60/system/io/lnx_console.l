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

   public class ConsoleWriter
   {
      method write(string s)
      {
         int l := s.Length;
         extern libc.write(1, s, l);
      }
   }

   public class ConsoleReader
   {
      char read()
      {
//         termios savedState;   
//         termios newState;

//         extern libc.tcgetattr(STDIN_FILENO, savedState);

//         newState := savedState;

         /* disable canonical input and disable echo.  set minimal input to 1. */
         //newState.LFlag := newState.LFlag & ~(ECHO | ICANON);
//         newState.LFlag := newState.LFlag & ~(ECHO | ICANON);
//         newState[VMIN] := 1;

//         extern libc.tcsetattr(STDIN_FILENO, TCSANOW, newState);

         int ch := extern libc.getchar();      /* block (withot spinning) until we get a keypress */

            /* restore the saved state */
//         extern libc.tcsetattr(STDIN_FILENO, TCSANOW, savedState);
                        
         ^ CharValue.load(ch);
      }
   }
}

public stdConsoleWriter = new ConsoleWriter();
public stdConsoleReader = new ConsoleReader();
