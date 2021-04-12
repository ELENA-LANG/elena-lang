namespace io
{    
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
            int  n  := 0;
            int  cl := 0;
            char c  := 0;
            
            int l := s.Length;
            while(n < l)
            {
                c := s[n];

                extern libc.putchar(c);

                cl := c.Length;
                n := n + cl
            }

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
           self.read(ref byte ch);
    
           ^ charConvertor.convert(ch)
        }
              
        private read(ref byte retVal)
        {
           int ch := extern libc.getchar();

           retVal := byteConvertor.convert(ch)
        }
    
        private read(byte[] buffer, ref int retVal)
        {
            int i;
            for (i := 0, i < retVal, i += 1)
            {
                self.read(ref byte ch);
    
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
            byte buffer[127];
            int length := 0;
            var output := emptyString;
            
            bool continue := true;
            while(continue)
            {
                length := 127;
                self.read(buffer, ref length);
    
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