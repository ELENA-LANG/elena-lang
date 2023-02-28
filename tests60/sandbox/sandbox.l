
A
{
      private int read(byte[] buffer, int len)
      {
         int retVal := 0;

         for (int i := 0, i < len, i += 1)
         {
            retVal += 1;

            int ch := extern libc.getchar();

            byte b := byteConvertor.convert(ch);
    
            buffer[i] := b;
    
            if(ch == 0Ah)
            {
               ^ retVal
            }
         };

         ^ retVal
      }

      string readLine()
      {
         byte buffer[127];
         int length := 0;
         var output := emptyString;
            
         bool continue := true;
         while(continue)
         {
            length := 127;
            length := self.read(buffer, length);
    
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
}

public program()
{
//   var o := new Object();
//   o.fail()
}