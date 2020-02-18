namespace io
{    
    // --- globalVariables ---
    
    singleton globalVariables
    {
        handle stdout()
        {
           /*handle libHandle := extern libdl.dlopen(
                                              "libc.so.6",
                                              1);
    
           /*dirty_ptr variable := extern libdl.dlsym(
                            libHandle,
                            "stdout");
*/    
            int handle/* := variable.Value*/;
    
           ^ handle
        }
    }

    // --- ConsoleWriter ---
    
    sealed class ConsoleWriter
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
            int n := 0;
            byte vc := 0;
            
            int l := s.Length;
            while(n < l)
            {
                s.read(n, ref vc);
            
                //extern libc.putchar(vc);
            
                n := n + 1
            }
    
            // !!check the result of the operation
        }
        
//        #method clear
//        [
//        ]
    
//        #method refresh
//        [
//            system'external'libc fflush &int:theHandle.
//        ]
                
        close()
        {
        }
    }

    // --- ConsoleReader ---
    
    sealed class ConsoleReader
    {
        constructor()
        {
        }
    
        read(ref char retVal)
        {
           self.read(ref byte ch);
    
           charConvertor.convert(ch, ref retVal)
        }
              
        read(ref byte retVal)
        {
           // int ch := extern libc.getchar();

           // byteConvertor.convert(ch, ref retVal)
        }
    
        read(byte[] buffer, ref int retVal)
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