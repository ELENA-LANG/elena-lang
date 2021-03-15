public const string newLine
    = $13$10;

//// --- characterController ---
//
//singleton characterController
//{
//    isUpperCase(char value)
//    {
//        int   length := 0;
//        short buffer[3];
//
//        charConvertor.save(value, 0, buffer, ref length);
//
//        buffer[length] := 0;
//                
//        short word := buffer[0];
//        int retVal := extern USER32.IsCharUpperW(/*buffer[0]*/word);
//        
//        ^ (retVal != 0)
//    }
//    
//    isLowerCase(char value)
//    {
//        int   length := 0;
//        short buffer[3];
//
//        charConvertor.save(value, 0, buffer, ref length);
//
//        buffer[length] := 0;
//                
//        short word := buffer[0];
//        int retVal := extern USER32.IsCharLowerW(/*buffer[0]*/word);
//        
//        ^ (retVal != 0)
//    }
//    
//    char toUpperCase(char char)
//    {
//        int   length := 0;
//        short buffer[3];
//
//        charConvertor.save(char, 0, buffer, ref length);
//
//        buffer[length] := 0;
//        
//        extern USER32.CharUpperW(buffer);
//        
//        char retVal := charConvertor.convert(0, buffer);
//        
//        ^ retVal
//    }
//    
//    char toLowerCase(char char)
//    {
//        int   length := 0;
//        short buffer[3];
//
//        charConvertor.save(char, 0, buffer, ref length);
//        
//        buffer[length] := 0;
//        
//        extern USER32.CharLowerW(buffer);
//        
//        char retVal := charConvertor.convert(0, buffer);
//        
//        ^ retVal
//    }
//}
//
//// --- caseLitrealOp ---
//
//public extension caseStringOp : String
//{
//    string upperCase()
//    {
//        string retVal := emptyString;
//        int    index := 0;
//        char   current := $0;
//        char   converted := $0;
//        int    currentLen := 0;
//        
//        int     length := self.Length;
//        
//        while (index < length)
//        {
//            self.read(index, ref current);
//            currentLen := current.Length;
//            
//            converted := characterController.toUpperCase(current);
//            
//            retVal := retVal.add(converted);
//            
//            index := index + currentLen
//        };
//            
//        ^ retVal
//    }
//    
//    string lowerCase()
//    {
//        string retVal := emptyString;
//        int    index := 0;
//        char   current := $0;
//        char   converted := $0;
//        int    currentLen := 0;
//        
//        int     length := self.Length;
//        
//        while (index < length)
//        {
//            self.read(index, ref current);
//            currentLen := current.Length;
//            
//            converted := characterController.toLowerCase(current);
//            
//            retVal := retVal.add(converted);
//            
//            index := index + currentLen
//        };
//            
//        ^ retVal
//    }
//}
//
//// --- caseLitrealOp ---
//
//public extension caseWideOp : WideString
//{
//    wide upperCase()
//    {
//        wide retVal := emptyWideString;        
//        int  index := 0;
//        char current := $0;
//        char converted := $0;
//        int  currentLen := 0;
//
//        int length := self.Length;
//                
//        while (index < length)
//        {
//            self.read(index, ref current);
//            currentLen := current.Length;
//            
//            converted := characterController.toUpperCase(current);
//            
//            retVal := retVal.add(converted);
//            
//            index := index + currentLen
//        };
//            
//        ^ retVal
//    }
//
//    wide lowerCase()
//    {
//        wide retVal := emptyWideString;        
//        int  index := 0;
//        char current := $0;
//        char converted := $0;
//        int  currentLen := 0;
//        
//        int length := self.Length;
//        
//        while (index < length)
//        {
//            self.read(index, ref current);
//            currentLen := current.Length;
//            
//            converted := characterController.toLowerCase(current);
//            
//            retVal := retVal.add(converted);
//            
//            index := index + currentLen
//        };
//            
//        ^ retVal
//    }
//}