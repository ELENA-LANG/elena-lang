import system'text;

singleton helper
{
   readIndex(string s, ref int i, ref int retVal)
   {
      char ch := $0;
      int  n := 0;
      int  v := 0;
        
      ch := s[i];

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
         ch := s[i];
      };
        
      retVal := v
   }
}

public extension stringFormatterOp : String
{
   interpolate(params object[] args)
   {
      auto buffer := new TextBuilder();
        
      int  len := self.Length;
      int  i := 0;
      char ch := $0;
      int  chlen := 0;
      int  index := 0;
        
      while (i < len)
      {
         ch := self[i];
            
         if (ch == $123)
         {
            i := i + 1;
                
            helper.readIndex(self, ref i, ref index);
                
            var arg := args[index];
            buffer.write(arg.toString());
                
            chlen := 1
         }
         else
         { 
            buffer.write(ch); 
            chlen := ch.Length;
         };
            
         i := i + chlen
      };
        
      ^ *buffer;
   }
} 