import extensions;

public Program()
{
         int ch := 0; 

         /*termios*/int savedState;   
         /*termios*/int newState;

//         extern libc.tcgetattr(STDIN_FILENO, savedState);

         newState := savedState;

//         extern libc.tcsetattr(STDIN_FILENO, TCSANOW, newState);
   
         extern {
            ch := extern libc.getchar();      /* block (withot spinning) until we get a keypress */
         };
                        
         ^ CharValue.load(ch);
}