namespace text
{
   // --- StringWriter ---
   public sealed class StringWriter : BaseVariable
   {
      string _value;
      string _newLine;
        
      constructor()
      {
         _value := emptyString;
         _newLine := newLineConstant;
      }
        
      constructor load(string s)
      {
         _value := s
      }
    
      constructor(string s, int count)
      {
         _value := emptyString;
         _newLine := newLineConstant;
            
         for (int i := 0, i < count, i += 1)
         {
            self.append(s);
         }
      }
        
      write(string s)
      {
         _value := _value.add:s
      }
        
      write(char ch)
      {
         _value := _value.add:ch
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
         _value := emptyString
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