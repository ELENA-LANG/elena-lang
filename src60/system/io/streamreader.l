import system'text;

namespace io
{
   // --- TextReader ---
   public interface TextReader
   {
      TextBuilder _output;
      string      _newLineConstant;
      int         _newLineLength;
    
      abstract read(char[] array, ref int actualLength);
    
      get abstract Stream Stream();
    
      get abstract Encoder Encoder();
    
      abstract close();
        
      get abstract bool Available();
                
      string readLine()
      {
         string line := emptyString;
            
         int len := -1;
         while (len == -1)
         {
            len := _output.indexOf(0, _newLineConstant);
                
            if(len == -1)
            {
               char buffer[64];
               int  bufferLen := 64;
                    
               self.read(buffer, ref bufferLen);
               if(bufferLen == 0)
               { 
                  len := -2 // !! to indicate the eof 
               }
               else
               {
                  _output.write(buffer, bufferLen)
               }
            }
         };
            
         if(len < 0) 
         {
            line := cast string(_output);
            _output.clear();
         }
         else
         {
            line := _output.Substring(0, len);
            _output.delete(0, len + _newLineLength)
         };
    
         ^ line
      }
        
      wide readWideLine()
      {
         wide line := emptyWideString;
         wide nl := cast wide(_newLineConstant);
         int  nl_len := nl.Length;
            
         int len := -1;
         while (len == -1)
         {
            len := _output.indexOf(0, nl);
                
            if(len == -1)
            {
               char buffer[64];
               int  bufferLen := 64;

               self.read(buffer, ref bufferLen);
               if(bufferLen == 0)
               { 
                  len := -2 // !! to indicate the eof 
               }
               else
               {
                  _output.write(buffer, bufferLen)
               }                                          
            }
         };

         if(len < 0) 
         {
            line := cast wide(_output);
            _output.clear();
         }
         else
         {
            line := _output.WideSubstring(0, len);
            _output.delete(0, len + nl_len)
         };
    
         ^ line
      }
   }
}