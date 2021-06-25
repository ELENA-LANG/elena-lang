namespace text
{
    
    const int TextPageSize = 128;
    
    // --- TextBuffer ---
    
    public sealed class TextBuilder
    {
        char[]    theBuffer;
        ref<int>  theLength;
    
        constructor()
        {
            theBuffer := new char[](TextPageSize);
            theLength := 0;
        }
    
        constructor load(value)
        {
            self.write(value)
        } 
    
        int Length
        {
            get()
            {
                int retVal := theLength.Value;
                
                ^ retVal
            }
                
            set(len)
            {
                theLength.Value := len
            }
        }
    
        clear()
        {
            theLength.Value := 0
        }
    
        char[] cast()
            = theBuffer;
    
        string cast()
            = String.copy(0, theLength.Value, theBuffer);
            
        string Value
            = String.copy(0, *theLength, theBuffer);
    
        get multiret Value(ref wide r)
        {
            r := WideString.copy(0, theLength.Value, theBuffer);
        }
    
        wide cast()
            = WideString.copy(0, theLength.Value, theBuffer);
            
        string toPrintable()
            = self.Value;
    
        writeLine()
        {
            self.write(forward newLine)
        }
    
        append(line)
        {
            self.write:line
        }
                                        
        private reserve(int len)
        {
            int newLength := len;
            
            newLength := newLength + theLength.Value;
            
            int bufferLength := theBuffer.Length;
            
            if (newLength >= bufferLength)
            {
                //aNewLength := (aNewLength / $TextPageSize + 1) * $TextPageSize.
                newLength := newLength / TextPageSize;
                newLength := newLength + 1;
                newLength := newLength * TextPageSize;
                
                char[] newBuffer := new char[](newLength);
                
                newBuffer.write(0, bufferLength, theBuffer);
                
                theBuffer := newBuffer
            }
        }
    
        write(string s)
        {
            int length := s.Length;
            
            ifnot (0 == length)
            {
                self.reserve(length);
                
                s.save(*theLength, theBuffer, ref length);
                
                theLength.append(length)
            }
        }
    
        write(wide s)
        {
            int length := s.Length;
            
            ifnot (0 == length)
            {
                self.reserve(length);
                
                s.save(theLength.Value, theBuffer, ref length);
                
                theLength.append(length)
            }
        }
    
        write(char ch)
        {
            self.reserve(1);
            
            theBuffer[theLength.Value] := ch;
            
            theLength.append(1)
        }
    
        write(int n)
        {
            byte tmp[16];
            char ch_tmp[16];
            int  len := 16;
            
            stringConvertor.convert(n, tmp, ref len);
            
            UTF8Encoder.convertFrom(0, ref len, tmp, ch_tmp, ref len);
            
            self.write(ch_tmp, len)
        }
    
        write(object)
        {
            self.write(object.toPrintable())
        }
    
        write(char[] array, int length)
        {
            ifnot (0 == length)
            {
                self.reserve(length);
    
                theBuffer.write(theLength.Value, length, array);
    
                theLength.append(length)
            }
        }

        private readIndex(string s, ref int i, ref int retVal)
        {
            char ch := $0;
            int  n := 0;
            int  v := 0;
            
            int i_val := i;
            
            s.read(i_val, ref ch);
            while (ch != $125)
            {
                n := intConvertor.convert(ch);
                if(n >= 30h && n <= 39h)
                {               
                    n := n - 30h;
                    
                    v := v * 10;
                    v := v + n
                }
                else
                { 
                    InvalidArgumentException.raise()
                };
                    
                i := i + 1;
                i_val := i;
                s.read(i_val, ref ch)
            };
            
            retVal := v
        }
    
        writeFormatted(string s, params object[] args)
        {
            int  len := s.Length;
            int  i := 0;
            char ch := $0;
            int  chlen := 0;
            int  index := 0;
            
            while (i < len)
            {
                s.read(i, ref ch);
                
                if (ch == $123)
                { 
                    i := i + 1;
                    
                    self.readIndex(s, ref i, ref index);
                    self.write(args[index]);
                    
                    chlen := 1
                }
                else
                {
                    self.write(ch);
                    chlen := ch.Length
                };
                
                i := i + chlen
            }
        }
    
        fill(string s, int counter)
        {
            for (int i := 0, i < counter, i += 1)
            {
                self.write(s)
            }
        }
    
        fill(wide s, int counter)
        {
            for (int i := 0, i < counter, i += 1)
            {
                self.write(s)
            }
        }
    
        fill(char ch, int counter)
        {
            int i := 0;
            while (i < counter)
            {
                self.write(ch);
                
                i += 1
            }
        }
    
        insert(int index, int length, char[] array)
        {
            self.reserve(length);
            
            theBuffer.move(index, theLength.Value, length);
            
            theBuffer.write(index, length, array);
            
            theLength.append(length)
        }

        insert(int index, string s)
        {
            int length := 0;
            UTF8Encoding.charCount(s, ref length);
    
            self.reserve(length);
            
            theBuffer.move(index, cast int(theLength), length);
            
            s.save(index, theBuffer, ref length);
            
            theLength.append(length)
        }

        insert(int index, wide s)
        {
            int length := 0;
            UTF16Encoding.charCount(s, ref length);
    
            self.reserve(length);
            
            theBuffer.move(index, cast int(theLength), length);
            
            s.save(index, theBuffer, ref length);
            
            theLength.append(length)
        }
        
        insert(int index, char ch)
        {
            self.reserve(1);
            
            theBuffer.move(index, cast int(theLength), 1);
            
            theBuffer.write(index, ch);
            
            theLength.append(1)
        }

        int indexOf(int position, char value)
        {
            int  index := 0;
            char current := $0;
            
            while (index < theLength.Value)
            {
                theBuffer.read(index, ref current);
                
                if (current == value)
                    { ^ index };
                    
                index += 1
            };
            
            ^ -1
        }
    
        int indexOf(int pos, string s)
        {
            int  strLen := 0;
            int  index := 0;
            int  firstChLen := 0;
            char current := $0;
            char firstCh := $0;
            char nextCh := $0;
            
            strLen := s.Length;
            
            firstCh := s[0];
            firstChLen := firstCh.Length;
            
            int maxIndex := theLength;
            maxIndex := maxIndex - strLen;
            
            while (index <= maxIndex)
            {
                current := theBuffer[index];
                
                if (current == firstCh)
                {
                    int j := firstChLen;
                    int chLen := 0;
                    while (j < strLen)
                    {
                        current := theBuffer[index + j];
                        nextCh := s[j];
                        
                        if (nextCh == current)
                        { 
                            chLen := nextCh.Length
                        }
                        else
                        { 
                            j := strLen; 
                            chLen := 1 
                        };
                        
                        j := j + chLen
                    };
                    
                    if (j == strLen)
                    { 
                        ^ index
                    }
                };
                    
                index += 1
            };
            
            ^ -1
        }

        int indexOf(int pos, wide s)
        {
            int  strLen := 0;
            int  index := 0;
            int  firstChLen := 0;
            char current := $0;
            char firstCh := $0;
            char nextCh := $0;
            
            strLen := s.Length;
            
            s.read(0, ref firstCh);
            firstChLen := firstCh.Length;
            
            int maxIndex := theLength;
            maxIndex := maxIndex - strLen;
            
            while (index <= maxIndex)
            {
                current := theBuffer[index];
                
                if (current == firstCh)
                {
                    int j := firstChLen;
                    int chLen := 0;
                    while (j < strLen)
                    {
                        current := theBuffer[index + j];
                        s.read(j, ref nextCh);
                        
                        if (nextCh == current)
                        { 
                            chLen := nextCh.Length
                        }
                        else
                        { 
                            j := strLen; 
                            chLen := 1 
                        };
                        
                        j := j + chLen
                    };
                    
                    if (j == strLen)
                    { 
                        ^ index; 
                    }
                };
                    
                index += 1
            };
            
            ^ -1
        }

        string Substring(int index, int length)
        {
            if (length == 0)
                { ^ emptyString };
                
            ^ String.copy(index, length, theBuffer)
        }
    
        wide WideSubstring(int index, int length)
        {
            if (length == 0)
                { ^ emptyWideString };
                
            ^ WideString.copy(index, length, theBuffer)
        }

        delete(int index, int length)
        {
            int n := 0 - length;
            int l := theLength;
            l := l - index;
            l := l - length;
            
            theBuffer.move(index + length, l, n);
            
            theLength.reduce(length)
        }
    
        trim(int n)
        {
            theLength.reduce(n)
        }
    }

}