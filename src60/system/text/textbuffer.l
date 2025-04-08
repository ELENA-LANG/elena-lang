namespace text
{
   const IntNumber TextPageSize = 128;
    
   // --- TextBuffer ---
    
   public sealed class TextBuilder
   {
      char[]         _buffer;
      Reference<int> _length;
    
      constructor()
      {
         _buffer := new char[](TextPageSize);
         _length := 0;
      }
    
      constructor load(value)
      {
         self.write(value)
      } 
    
      int Length
      {
         get()
         {
            int retVal := *_length;
                
            ^ retVal
         }
                
         set(len)
         {
            _length.Value := len
         }
      }
    
      clear()
      {
         _length.Value := 0
      }
    
      char[] cast()
         = _buffer;
    
      string cast()
         = String.copy(0, *_length, _buffer);
            
      string Value
         = String.copy(0, *_length, _buffer);
    
      get retoverload wide Value()
      {
         ^ WideString.copy(0, _length.Value, _buffer);
      }
    
      wide cast()
         = WideString.copy(0, *_length, _buffer);
            
      string toPrintable()
         = *self;
    
      writeLine()
      {
         self.write(NewLineConstant)
      }
    
      append(line)
      {
         self.write(line)
      }
                                        
      private reserve(int len)
      {
         int newLength := len;
            
         newLength := newLength + (*_length);
            
         int bufferLength := _buffer.Length;
            
         if (newLength >= bufferLength)
         {
            //aNewLength := (aNewLength / $TextPageSize + 1) * $TextPageSize.
            newLength := newLength / TextPageSize;
            newLength := newLength + 1;
            newLength := newLength * TextPageSize;
                
            var char[] newBuffer := new char[](newLength);
                
            Array<char>.copy(newBuffer, _buffer, 0, bufferLength);
                
            _buffer := newBuffer
         }
      }
    
      write(string s)
      {
         int length := s.Length;
            
         if:not (0 == length)
         {
            self.reserve(length);

            length := s.save(0, length, _buffer, *_length);
                
            _length.append(length)
         }
      }
    
      write(wide s)
      {
         int length := s.Length;
            
         if:not (0 == length)
         {
            self.reserve(length);
                
            length := s.save(0, length, _buffer, *_length);
                
            _length.append(length)
         }
      }
    
      write(char ch)
      {
         self.reserve(1);
            
         _buffer[*_length] := ch;
            
         _length.append(1)
      }
    
      write(object)
      {
         self.write(object.toPrintable())
      }

      write(char[] array, int length)
      {
         if:not (0 == length)
         {
            self.reserve(length);
    
            int bufIndex := *_length;
            Array<char>.copyTo(_buffer, array, bufIndex, length);
    
            _length.append(length)
         }
      }

      private int readIndex(string s, ref int i)
      {
         char ch := $0;
         int  n := 0;
         int  v := 0;
            
         int i_val := i;
            
         ch := s[i_val];
         while (ch != $125)
         {
            n := IntConvertor.convert(ch);
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
            ch := s[i_val]
         };
            
         ^ v
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
            ch := s[i];
                
            if (ch == $123)
            { 
               i := i + 1;
                    
               index := self.readIndex(s, ref i);
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
         for (int i := 0; i < counter; i += 1)
         {
            self.write(s)
         }
      }
    
      fill(wide s, int counter)
      {
         for (int i := 0; i < counter; i += 1)
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
         
         int bufLen := *_length;   
         Array<char>.move(_buffer, index, bufLen, -length);
         Array<char>.copyTo(_buffer, array, index, length);
            
         _length.append(length)
      }

      insert(int index, string s)
      {
         int length := UTF8Encoding.getCharCount(s);
    
         self.reserve(length);
         
         int bufLen := *_length;   
         Array<char>.move(_buffer, index, bufLen, -length);
            
         s.save(0, s.Length, _buffer, index);
            
         _length.append(length)
      }

      insert(int index, wide s)
      {
         int length := UTF16Encoding.getCharCount(s);
    
         self.reserve(length);
            
         int bufLen := *_length;   
         Array<char>.move(_buffer, index, bufLen, -length);
            
         s.save(0, s.Length, _buffer, index);
            
         _length.append(length)
      }
        
      insert(int index, char ch)
      {
         self.reserve(1);
            
         Array<char>.move(_buffer, index, *_length, 1);
            
         _buffer[index] := ch;
            
         _length.append(1)
      }

      int indexOf(int position, char value)
      {
         int  index := 0;
         char current := $0;
            
         int len := *_length;
         while (index < len)
         {
            current := _buffer[index];
                
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
            
         int maxIndex := *_length;
         maxIndex := maxIndex - strLen;
            
         while (index <= maxIndex)
         {
            current := _buffer[index];
                
            if (current == firstCh)
            {
               int j := firstChLen;
               int chLen := 0;
               while (j < strLen)
               {
                  current := _buffer[index + j];
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
           
         firstCh := s[0];
         firstChLen := firstCh.Length;
            
         int maxIndex := *_length;
         maxIndex := maxIndex - strLen;
            
         while (index <= maxIndex)
         {
            current := _buffer[index];
                
            if (current == firstCh)
            {
               int j := firstChLen;
               int chLen := 0;
               while (j < strLen)
               {
                  current := _buffer[index + j];
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
            { ^ EmptyString };
                
         ^ String.copy(index, length, _buffer)
      }
    
      wide WideSubstring(int index, int length)
      {
         if (length == 0)
            { ^ emptyWideString };
                
         ^ WideString.copy(index, length, _buffer)
      }

      delete(int index, int length)
      {
         int l := *_length;
         l := l - index;
         l := l - length;
            
         Array<char>.move(_buffer, index, l, length);
            
         _length.reduce(length)
      }
    
      trim(int n)
      {
         _length.Value := n
      }
   }

   // --- StringBuilder ---    
   public sealed class StringBuilder
   {
      byte[]         _buffer;
      Reference<int> _length;
    
      constructor()
      {
         _buffer := new byte[](TextPageSize);
         _length := 0;
      }
                                        
      private reserve(int len)
      {
         int newLength := len;
            
         newLength := newLength + (*_length);
            
         int bufferLength := _buffer.Length;
            
         if (newLength >= bufferLength)
         {
            //aNewLength := (aNewLength / $TextPageSize + 1) * $TextPageSize.
            newLength := newLength / TextPageSize;
            newLength := newLength + 1;
            newLength := newLength * TextPageSize;
                
            var byte[] newBuffer := new byte[](newLength);
                
            Array<byte>.copy(newBuffer, _buffer, 0, bufferLength);
                
            _buffer := newBuffer
         }
      }
    
      write(char ch)
      {
         reserve(4);

         int len := UTF8Encoder.convertChar(ch, _buffer, *_length);
            
         _length.append(len)
      }
        
      write(string s)
      {
         int s_len := s.Length;
         int counter := UTF8Encoding.getCharCount(s);

         self.reserve(counter);

         UTF8Encoder.toByteArray(s, 0, ref s_len, _buffer, *_length, ref counter);
         
         _length.append(counter)
      }

      write(o)
         <= write(o.toPrintable());

      insert(int index, char ch)
      {
         int current := *_length;
         if (current < index)
            InvalidArgumentException.raise();

         self.reserve(4);
            
         byte tmp[4];
         int len := UTF8Encoder.convertChar(ch, tmp, 0);

         Array<byte>.move(_buffer, index, current, len);            
         Array<byte>.copyTo(_buffer, tmp, index, len);
            
         _length.append(len)
      }

      insert(int index, string s)
      {
         int current := *_length;
         if (current < index)
            InvalidArgumentException.raise();

         int s_len := s.Length;
         int counter := UTF8Encoding.getCharCount(s);

         self.reserve(counter);

         Array<byte>.move(_buffer, index, current, s_len);
         UTF8Encoder.toByteArray(s, 0, ref s_len, _buffer, index, ref counter);
         
         _length.append(counter)         
      }

      insert(index, o)
         <= insert(index as:IntNumber, o.toPrintable());

      delete(int index, int length)
      {
         int l := *_length;
         l := l - index;
         l := l - length;
            
         Array<byte>.move(_buffer, index, l, length);
            
         _length.reduce(length)
      }

      int Length
         = *_length;

      string Value
         = String.fromByteArray(0, *_length, _buffer);
   }

   // --- WideStringBuilder ---    
   public sealed class WideStringBuilder
   {
      short[]        _buffer;
      Reference<int> _length;
    
      constructor()
      {
         _buffer := new short[](TextPageSize);
         _length := 0;
      }
                                        
      private reserve(int len)
      {
         int newLength := len;
            
         newLength := newLength + (*_length);
            
         int bufferLength := _buffer.Length;
            
         if (newLength >= bufferLength)
         {
            //aNewLength := (aNewLength / $TextPageSize + 1) * $TextPageSize.
            newLength := newLength / TextPageSize;
            newLength := newLength + 1;
            newLength := newLength * TextPageSize;
                
            var short[] newBuffer := new short[](newLength);
                
            Array<short>.copy(newBuffer, _buffer, 0, bufferLength);
                
            _buffer := newBuffer
         }
      }
    
      write(char ch)
      {
         reserve(2);

         UTF16Encoding.toShortArray(ch, _buffer, *_length, out int len);
            
         _length.append(len)
      }
        
      write(wide s)
      {
         int s_len := s.Length;
         int counter := UTF16Encoding.getCharCount(s);

         self.reserve(counter);

         UTF16Encoding.toShortArray(s, 0, s_len, _buffer, *_length);
         
         _length.append(counter)
      }

      wide(o)
         <= write(cast wide(o.toPrintable()));

      insert(int index, char ch)
      {
         int current := *_length;
         if (current < index)
            InvalidArgumentException.raise();

         self.reserve(2);

         int len := ch.WideLength;

         Array<short>.move(_buffer, index, current, len);            
         UTF16Encoding.toShortArray(ch, _buffer, *_length, ref len);
            
         _length.append(len)
      }

      insert(int index, wide s)
      {
         int current := *_length;
         if (current < index)
            InvalidArgumentException.raise();

         int s_len := s.Length;
         int counter := UTF16Encoding.getCharCount(s);

         self.reserve(counter);

         Array<short>.move(_buffer, index, current, s_len);
         UTF16Encoding.toShortArray(s, 0, s_len, _buffer, index);
         
         _length.append(counter)         
      }

      insert(index, o)
         <= insert(index as::IntNumber, o.toPrintable() as::WideString);

      delete(int index, int length)
      {
         int l := *_length;
         l := l - index;
         l := l - length;
            
         Array<short>.move(_buffer, index, l, length);
            
         _length.reduce(length)
      }

      int Length
         = *_length;

      string Value
         = WideString.fromShortArray(0, *_length, _buffer);
   }
}
