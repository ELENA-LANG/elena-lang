namespace text
{
   // --- StringBookmark ---
   public class StringBookmark : BaseValue
   {
      string _value;
      int    _index;
      int    _start;
        
      constructor()
      {
         _value := EmptyString;
         _index := 0;
         _start := 0
      }
    
      constructor(text)
      {
         _value := text;
         _index := 0;
         _start := 0
      }
    
      bool equal(int value)
         => _index;
    
      bool equal(string value)
         => _value;
    
      bool equal(wide value)
         => _value;
    
      bool less(int value)
         => _index;
    
      bool less(string value)
         => _value;
    
      bool less(wide value)
         => _value;
    
      find(object)
         <= find(cast int(object));
    
      append(object)
         <= append(cast int(object));
    
      insert(object)
         <= insert(object.toPrintable());
    
      delete(object)
         <= delete(cast int(object));
    
      find(int n)
      {
         _index := n
      }
    
      find(string s)
      {
         _index := _value.indexOf(_index, s);
            
         ^ (-1 != _index)
      }
        
      find(wide s)
      {
         _index := _value.indexOf(_index, s);
            
         ^ (-1 != _index)
      }
        
      mark()
      {
         if(-1 == _index)
         {
            self.clear()
         }
         else
         {
            _index := 1 + _index;
            _start := _index
         }        
      }
        
      append(string s)
      {
         _value := _value + s
      }
        
      append(wide s)
      {
         _value := _value + s
      }
        
      append(int n)
      {
         _value := _value + n
      }
        
      insert(string s)
      {
         _value := _value.insert(_index, s);
            
         _index := _index + s.Length
      }
        
      insert(wide s) 
      {
         _value := _value.insert(_index, s);
            
         _index := _index + s.Length
      }
        
      delete(int len)
      {
         _value := _value.delete(_index, len)
      }
    
      delete(string s)
         <= delete(s.Length);
    
      delete(wide s)
         <= delete(s.Length);
    
      clear()
      {
         _value := EmptyString;
         _index := 0;
         _start := 0
      }
    
      get Marked()
      {
         if(-1 == _index)
         {
            ^ _value.Substring(_start)
         }
         else
         {
            ^ _value.Substring(_start, _index - _start)           
         }
      }
        
      string toPrintable() => _value;
        
      string cast() = cast string(_value);
        
      wide cast() = cast wide(_value);
        
      int cast() = cast int(_index);
        
      get Length() => _value;
        
      get Index() = _index;
   }

   // --- StringWriter ---
   public sealed class StringWriter : BaseVariable
   {
      string _value;
      string _newLine;
        
      constructor()
      {
         _value := EmptyString;
         _newLine := NewLineConstant;
      }
        
      constructor load(string s)
      {
         _value := s
      }
    
      constructor(string s, int count)
      {
         _value := EmptyString;
         _newLine := NewLineConstant;
            
         for (int i := 0; i < count; i += 1)
         {
            self.append(s);
         }
      }
        
      write(string s)
      {
         _value := _value.add(s)
      }
        
      write(char ch)
      {
         _value := _value.add(ch)
      }
        
      writeLine(string s)
      {
         self.write(s).write(_newLine)
      }
       
      writeLine()
      {
         self.write(_newLine)
      }    
    
      append(string s)
         <= write(s); 
        
      append(char ch)
         <= write(ch); 
        
      insert(index, s)
      {
         _value := _value.insert(index, s)
      }
    
      replace(string s, string s2)
      {
         _value := _value.replace(s, s2)
      }
        
      replace(object s, object s2)
      {
         _value := _value.replace(cast string(s), cast string(s2))
      }
        
      clear()
      {
         _value := EmptyString
      }
    
      string toPrintable() = _value;
    
      string cast() = _value;
    
      string Value
      {
         get() = _value;
            
         set(val)
         {
            _value := val
         }
      }
    
      wide cast() = _value;
    
      dispatch() => _value;
   }
}