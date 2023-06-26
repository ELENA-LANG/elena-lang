//
//public const struct ExtensionDispatcher
//{
//   embeddable __string pointer[] array;
//
//   static ExtensionDispatcher load(Message message, string[] namespaces)
//   {
//      pointer output[64];
//        
//      byte tmp[1024];
//      int nsLen := namespaces.Length;
//      int tmpIndex := 0;
//      for(int i := 0, i < nsLen, i+=1) {
//         auto s := namespaces[i];
//         int sLen := s.Length;
//
//         UTF8Encoding.convert(tmpIndex, sLen, s, tmp);
//         tmpIndex += sLen;
//         tmp[tmpIndex] := 0;
//         tmpIndex += 1;
//
//         if (tmpIndex >= 1024){
//            InvalidArgumentException.raise()
//         }
//      };
//      tmp[tmpIndex] := 0;
//
//      int len := extern LoadExtensionDispatcherLA(tmp, message, output);
//
////        intern system'core_routines'__extdispatcher.load(output, message, tmp, ref int len);
////        if (len == 1)
////        {
////            ^ nil
////        };
////        
////        output[len] := 0;
////        len += 1; 
////        
////        ^ ExtensionDispatcher.copy(output, len)
//
//   }
//}