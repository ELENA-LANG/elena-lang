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
        byte c_cc[NCCS];		/* control characters */
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

    // --- globalVariables ---
    
    singleton globalVariables
    {
        handle stdout()
        {
           handle libHandle := extern libdl.dlopen(
                                              "libc.so.6",
                                              1);
    
           dirty_ptr variable := extern libdl.dlsym(
                            libHandle,
                            "stdout");

            int handle := variable.Value;
    
           ^ handle
        }
    }

    // --- ConsoleWriter ---
    
    sealed class ConsoleWriter : IConsoleWriter
    {
        handle handle;
        string newLine;
    
        constructor(handle h)
        {
            handle := h;
            newLine := forward newLine;
        }
    
        write(wide s)
        {
            self.write(cast string(s))
        }
    
        write(string s)
        {
            int l := s.Length;
            extern libc.write(STDOUT_FILENO, s, l);
            
            
//            int  n  := 0;
//            int  cl := 0;
//            char c  := 0;
//            
//            int l := s.Length;
//            while(n < l)
//            {
//                c := s[n];
//
//                extern libc.putchar(c);
//
//                cl := c.Length;
//                n := n + cl
//            }

            // !!check the result of the operation
        }

        writeLine(string s)
        {
            self.write(s);
            self.write(newLine);
        }

        writeLine(wide s)
        {
            self.write(s);
            self.write(newLine);
        }

        writeLine()
        {
            self.write(newLine);
        }

        refresh()
        {
            extern libc.fflush(handle)
        }
                
        //close()
        //{
        //}
    }

    // --- ConsoleReader ---
    
    sealed class ConsoleReader : /*Object, interface<*/IConsoleReader//>
    {
        constructor()
        {
        }
    
        char read()
        {
           self.readChar(ref char ch);
    
           ^ ch
        }
              
        private readChar(ref char retVal)
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
                        
            retVal := ch;
        }
    
        private read(char[] buffer, ref int retVal)
        {
            int i;
            for (i := 0, i < retVal, i += 1)
            {
                self.readChar(ref char ch);
    
                buffer[i] := ch;
    
                if(ch == 0Ah)
                {
                    retVal := i + 1;  
                    
                    ^ self
                }
            };
    
            retVal := i
        }
    
        wide readWideLine()
            = cast wide(self.readLine());
    
        string readLine()
        {
            char buffer[127];
            int length := 0;
            var output := emptyString;
            
            bool continue := true;
            while(continue)
            {
                length := 127;
                self.read(buffer, ref length);
    
                char ch := buffer[length - 1];
    
                if(ch == 0Ah)
                {
                    length := length - 1;
                    
                    continue := false
                };
                
                var temp := String.copy(0, length, buffer);
                
                output := output + temp;
            };
    
            ^ output
        }
    
//        #method char
//        [
//            #var(char)aRetVal.
//            $self read &vchar:aRetVal.
//            
//            ^ CharValue new &char:aRetVal.
//        ]
//    
//        #method close
//        [
//        ]
    }

    // --- Console Standard Symbols
    
    symbol stdConsoleWriter = new ConsoleWriter(globalVariables.stdout());

    symbol stdConsoleReader = new ConsoleReader();

}