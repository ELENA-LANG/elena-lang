import system'text;

public const struct ExtensionDispatcher
{
   embeddable __string pointer[] _array;

   protected constructor allocate(int length)
      = new pointer[](length);
        
   internal constructor copy(pointer[] array, int length)
      <= allocate(length)
   {
      for(int i := 0; i < length; i += 1)
      {
         _array[i] := array[i]
      }
   }

   static ExtensionDispatcher? load(Message message, string[] namespaces)
   {
      pointer output[64];
        
      byte tmp[1024];
      int nsLen := namespaces.Length;
      int tmpIndex := 0;
      for(int i := 0; i < nsLen; i+=1) {
         auto s := namespaces[i];
         int sLen := s.Length;

         int tmpLen := UTF8Encoder.toByteArray(s, 0, ref sLen, tmp, tmpIndex, 1024);
         tmpIndex += tmpLen;
         tmp[tmpIndex] := 0;
         tmpIndex += 1;

         if (tmpIndex >= 1024){
            InvalidArgumentException.raise()
         }
      };
      tmp[tmpIndex] := 0;

      int len := extern LoadExtensionDispatcherLA(tmp, message, output);
      if (len == 1)
      {
         ^ nil
      };
      
      output[len] := 0;
      len += 1; 
      
      ^ ExtensionDispatcher.copy(output, len)
   }

   dispatch() : external(system'core_routines'__extdispatcher);
}

public const struct VariadicExtensionDispatcher
{
   embeddable __string pointer[] _array;
    
   protected constructor allocate(int length)
      = new pointer[](length);
        
   internal constructor copy(pointer[] array, int length)
      <= allocate(length)
   {
      for(int i := 0; i < length; i += 1)
      {
         _array[i] := array[i]
      }
   }
    
   static VariadicExtensionDispatcher? load(Message message, string[] namespaces)
   {
      pointer output[64];
        
      byte tmp[1024];
      int nsLen := namespaces.Length;
      int tmpIndex := 0;
      for(int i := 0; i < nsLen; i+=1) {
         auto s := namespaces[i];
         int sLen := s.Length;
            
         int  tmpLen := UTF8Encoder.toByteArray(s, 0, ref sLen, tmp, tmpIndex, 1024);
         tmpIndex += tmpLen;
         tmp[tmpIndex] := 0;
         tmpIndex += 1;
            
         if (tmpIndex >= 1024){
            InvalidArgumentException.raise()
         }
      };
      tmp[tmpIndex] := 0;
        
      int len := extern LoadExtensionDispatcherLA(tmp, message, output);
      if (len == 1)
      {
         ^ nil
      };
      
      output[len] := 0;
      len += 1; 
      
      ^ VariadicExtensionDispatcher.copy(output, len)
   }
    
   dispatch() : external(system'core_routines'__vextdispatcher);
}

public extension dynamicOp
{
   mixInto(role)
      = new Extension(self, role);
}