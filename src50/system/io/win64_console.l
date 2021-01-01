namespace io
{    
    import system'text;

    const int FILE_TYPE_CHAR = 2;

    sealed struct CONSOLE_SCREEN_BUFFER_INFO
    {
        COORD      dwSize;    
        COORD      dwCursorPosition;    
        short      wAttributes;
        SMALL_RECT srWindow;
        COORD      dwMaximumWindowSize;
        
        embeddable constructor()
        {
        }
    
        short SizeX
        {
            get()
            {
                short retVal := dwSize.X;
                
                ^ retVal
            }
        }
    
        short SizeY
        {
            get()
            {
                short retVal := dwSize.Y;
                
                ^ retVal
            }
        }
    
        readSizeTo(COORD r)
        {
            short x := dwSize.X;
            short y := dwSize.Y;
            
            r.write(x, y)
        }
    
        readCursorPositionTo(COORD r)
        {
            COORD copy := dwCursorPosition;
            
            short x := copy.X;
            short y := copy.Y;
            
            r.write(x, y)
        }
    }

    // --- ConsoleHandleFactory --
    
    singleton ConsoleHandleFactory
    {
        handle new(int attrs)
        {
            handle h := extern KERNEL64.GetStdHandle(attrs);
            
            ^ h
        }
        
        newWriter()
        {
            handle h := ConsoleHandleFactory.new(0FFFFFFF5h);
            int    dwMode := extern KERNEL64.GetFileType(h);
            if (dwMode == FILE_TYPE_CHAR)
            {
                ^ new ConsoleWriter(h)
            }
            else
            {
                ^ new FileWriter(h)
            }
        }
    }
    
    // --- ConsoleWriter ---
    
    sealed class ConsoleWriter
    {
        handle handle;
        wide   newLine;
    
        constructor(handle h)
        {
            handle := h;
            newLine := forward newLine;
        }
    
        writeLine(string s)
        {
            self.write(cast wide(s));
            self.write(newLine);
        }
    
        write(string s)
        {
            self.write(cast wide(s))
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
    
        write(wide s)
        {
            int len := s.Length;
            
            extern KERNEL64.WriteConsoleW(handle,s,len,0,0)
    
            // !!check the result of the operation
        }
        
        clear()
        {
            auto buffer := new CONSOLE_SCREEN_BUFFER_INFO();
            int  length := 0;
            int  retVal := 0;
    
            extern KERNEL64.GetConsoleScreenBufferInfo(handle, buffer);
    
            short sizeX := buffer.SizeX;
            short sizeY := buffer.SizeY;
                    
            length := sizeX * sizeY;
            
            extern KERNEL64.FillConsoleOutputCharacterW(handle, cast short(32), length, 0, ref retVal);
            
            extern KERNEL64.SetConsoleCursorPosition(handle, 0)
        }
    
        setCursorPosition(int x, int y)
        {
            int coord := y * 10000h + x;
                    
            extern KERNEL64.SetConsoleCursorPosition(handle, coord)
        }
    
        refresh()
        {
        }    
    
        close()
        {
            extern KERNEL64.CloseHandle(handle)
        }
    }

    // --- FileWriter ---
    
    sealed class FileWriter
    {
        handle theHandle;
        string theNewLineSymbol;
    
        constructor(handle h)
        {
            theHandle := h;
            theNewLineSymbol := forward newLine;
        }
    
        write(byte[] buffer, int length)
        {
            int written := 0;
            extern KERNEL64.WriteFile
                (theHandle, buffer, length, ref written, 0)
        }
    
        write(wide literal)
        {
            byte buffer[256];
            int  length := literal.Length;
            int  index := 0;
            int  buf_len := 256;
            int  subs_len := 256;
            while (length > 0)
            {
                buf_len := 256;
                
                subs_len := length < 128 ? length : 128;
                
                OEMEncoder.convert(index, ref subs_len, literal, buffer, ref buf_len);
                
                self.write(buffer, buf_len);
                
                index := index + subs_len;
                length := length - subs_len
            }
        }
        
        writeLine(wide s)
        {
            self.write(s);
            self.write(theNewLineSymbol);
        }
        
        writeLine()
        {
            self.write(theNewLineSymbol);
        }
        
        writeLine(string s)
        {
            self.write(s);
            self.write(theNewLineSymbol);
        }
        
        write(string literal)
        {
            byte buffer[256];
            int length := literal.Length;
            int index := 0;
            int buf_len := 256;
            int subs_len := 256;
            while (length > 0)
            {
                buf_len := 256;
                
                if (length < 128)
                {
                    subs_len := length
                }
                else
                {
                    subs_len := 128
                };
                
                OEMEncoder.convert(index, ref subs_len, literal, buffer, ref buf_len);
                
                self.write(buffer, buf_len);
                
                index += subs_len;
                length -= subs_len
            }
        }
        
        clear()
        {
        }
    
        refresh()
        {
        }
    
        close()
        {
            extern KERNEL64.CloseHandle(theHandle)
        }
    }

    // --- ConsoleReader ---
    
    sealed /*struct*/class ConsoleReader : Object, interface<IConsoleReader>
    {
        handle handle;
        
        constructor(handle h)
        {
            handle := h
        }
    
        read(ref char retVal)
        {
            byte  dump[20];
            int   numberRead := 0;
            short eventType := 0;
            short charCode := 0;
            int   keyDown := 0;
    
            bool found := false;
            until (found)
            {
                extern
                {
                    extern KERNEL64.ReadConsoleInputW(handle, dump, 1, ref numberRead);
                };
    
                dump.read(0, ref eventType);
                if (eventType == 1)
                {
                    dump.read(4, ref keyDown);
                    
                    found := keyDown == 1
                }
                else
                {
                    found := false 
                };
            };
    
            dump.read(14, ref charCode);
            
            retVal := system'text'UTF16Encoding.toChar(charCode)
        }
        
        get bool KeyAvailable()
        {
            byte  dump[20];
            int   numberRead := 0;
            short eventType := 0;
            int   keyDown := 0;
            
            while (true)
            {
                numberRead := 0;
                
                extern
                {
                    extern KERNEL64.PeekConsoleInputW(handle, dump, 1, ref numberRead)
                };
                            
                if (0 == numberRead)
                    { ^ false };
                                
                dump.read(0, ref eventType);
                if (eventType == 1)
                {
                    dump.read(4, ref keyDown);
                        
                    if (keyDown == 1)
                        { ^ true }
                };
    
                extern
                {
                    extern KERNEL64.ReadConsoleInputW(handle, dump, 1, ref numberRead)
                }                
            };
            
            ^ false
        }

        read(short[] dump, ref int retVal)
        {
            int len := retVal;
            
            extern
            {
                extern KERNEL64.ReadConsoleW(handle, dump, len, ref retVal, 0)
            }
        }
    
        wide readWideLine()
        {
            short dump[127];
            int   len := 0;
            wide  output := emptyWideString;
            
            bool continue := true;
            while (continue)
            {
                len := 127;
                self.read(dump, ref len);
                
                short sh := dump[len - 1];
                if (sh == 0Ah)
                    {
                        len := len - 2;
                        
                        continue := false
                    };
                
                wide temp := WideString.fromShortArray(0, len, dump);
                
                output := output + temp;
            };
    
            ^ output
        }
    
        string readLine()
            = self.readWideLine();
    
        close()
        {
            extern KERNEL64.CloseHandle(handle)
        }
    }

    // --- Console Standard Symbols
    
    public symbol stdConsoleWriter = ConsoleHandleFactory.newWriter();
    
    public symbol stdConsoleReader = new ConsoleReader(ConsoleHandleFactory.new(0FFFFFFF6h));

}