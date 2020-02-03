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
    
                extern libc.putchar(vc);
    
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
    
        #method(stacksafe) read &vbyte:aRetVal
        [    
            #var(int) ch := system'external'libc getchar.
    
            byteConvertor convert &int:ch &vbyte:aRetVal. 
        ]
    
        #method(stacksafe) read &bytearray:aBuffer &vint:aRetVal
        [
            #var(int) i := 0.
            #loop (aRetVal > i) ?
            [
                #var(byte)ch.
                $self read &vbyte:ch.
    
                aBuffer@i := ch.
    
                i += 1.
    
                (ch == 0Ah)
                   ? [ aRetVal := i.  ^ $self. ].
            ].
    
            aRetVal := i.
        ]
    
        #method read &wide
            = $self read &literal wide.
    
        #method read &literal
        [
            #var(byte:127)aBuffer.
            #var(int)aLength.
            #var anOutput := emptyLiteralValue.
            
            #var(bool) continue := true.
            #loop continue ?
            [
                aLength := 127.
                $self read &bytearray:aBuffer &vint:aLength.
    
                #var(byte)ch := aBuffer@(aLength - 1).
    
                (ch == 0Ah)
                    ? [
                        aLength := aLength - 1.
                        
                        continue := false.
                    ].
                
                #var aTemp := LiteralValue $new &int:0 &int:aLength &bytearray:aBuffer.
                
                anOutput := anOutput + aTemp.
            ].
    
            ^ anOutput.
        ]
    
        #method read &vchar:aRetVal
        [
           #var(byte)ch.
           $self read &vbyte:ch.
    
           charConvertor convert &byte:ch &vchar:aRetVal.  
        ]
              
        #method char
        [
            #var(char)aRetVal.
            $self read &vchar:aRetVal.
            
            ^ CharValue new &char:aRetVal.
        ]
    
        #method close
        [
        ]
    }

    // --- Console Standard Symbols
    
    symbol stdConsoleWriter = new ConsoleWriter(globalVariables.stdout());

    symbol stdConsoleReader = new ConsoleReader();

}