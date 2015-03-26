//#define system.
//
//// --- $ConsoleHandleFactory --
//
//#symbol $ConsoleHandleFactory =
//{
//    #method(type:handle) new &int_attributes:anAttributes
//    [ 
//        #var(type:handle)aHandle := system'external'KERNEL32 GetStdHandle &int:anAttributes.
//        
//        ^ Handle new &handle:aHandle.
//    ]
//}.
//
//// --- ConsoleWriter ---
//
//#class(struct:embeddable) $ConsoleWriter
//{
//    #field(type:handle) theHandle.
//
//    #constructor new &handle:aHandle
//    [
//        theHandle := aHandle.
//    ]
//
//    #method write &literal : aLiteral
//    [
//        #var(type:int)aLength := aLiteral length.
//        
//        system'external'KERNEL32 WriteConsoleW
//            &handle  :  theHandle
//            &literal :  aLiteral
//            &int     :  aLength
//            &int     :  0                       // !! temporally we do not use it
//            &int     :  0.             
//
//        // !!check the result of the operation
//    ]
//    
//    #method close
//    [
//        theHandle close.
//    ]
//}
//
//// --- ConsoleReader ---
//
//#class(sealed,struct:embeddable) $ConsoleReader
//{
//    #field(type:handle) theHandle.
//    
//    #constructor new &handle:aHandle
//    [
//        theHandle := aHandle.
//    ]
//
//    #method read &out'char:aChar
//    [
//        #var(type:bytearray,size:20)aDump.
//        #var(type:int)aNumberRead.
//        #var(type:short)anEventType.
//        #var(type:int)aKeyDown.
//        
//        #var found := false.
//        #loop found !
//        [
//            system'external'KERNEL32 ReadConsoleInputW
//                &handle        : theHandle
//                &out'bytearray : aDump
//                &int           : 1
//                &out'int       : aNumberRead.
//
//            anEventType := aDump read &index:0.
//            (anEventType == 1)
//                ? [  
//                    aKeyDown := aDump read &index:4.
//                    
//                    found := aKeyDown == 1.
//                ]
//                ! [ found := false. ].
//        ].
//
//        aChar := aDump read &index:14.
//    ]
//
//    #method read &out'int:aKey
//    [
//        #var(type:bytearray,size:20)aDump.
//        #var(type:int)aNumberRead.
//        #var(type:short)anEventType.
//        #var(type:int)aKeyDown.
//        
//        #var found := false.
//        #loop found !
//        [
//            system'external'KERNEL32 ReadConsoleInputW
//                &handle        : theHandle
//                &out'bytearray : aDump
//                &int           : 1
//                &out'int       : aNumberRead.
//
//            anEventType := aDump read &index:0.
//            (anEventType == 1)
//                ? [  
//                    aKeyDown := aDump read &index:4.
//                    
//                    found := aKeyDown == 1.
//                ]
//                ! [ found := false. ].
//        ].
//                    
//        aKey := aDump read &index:8.
//    ]
//
//    #method read &chararray:aBuffer &length:aLength &out'length:aRetVal
//    [
//        system'external'KERNEL32 ReadConsoleW
//            &handle     : theHandle
//            &chararray : aBuffer
//            &int        : aLength
//            &out'length : aRetVal
//            &int        : 0.
//    ]
//    
//    #method read &line
//    [
//        #var(type:chararray,size:127)aBuffer.
//        #var(type:length)aLength.
//        #var anOutput := emptyLiteralValue.
//        
//        #var(type:bool) continue := true.
//        #loop continue ?
//        [
//            aLength := 127.
//            aLength := $self read &chararray:aBuffer &length:aLength.
//            
//            #var(type:char)aShort := aBuffer read &index:(aLength - 1).
//            (aShort equal &char:#0Ah)
//                ? [
//                    aLength := aLength - 2.
//                    
//                    continue := false.
//                ].
//            
//            #var aTemp := LiteralValue new &length:aLength &chararray:aBuffer.
//            
//            anOutput := anOutput + aTemp.
//        ].
//
//        ^ anOutput.
//    ]
//
//    #method int
//    [
//        #var(type:int)aRetVal := $self int.
//        
//        ^ IntNumber new &int:aRetVal.
//    ]
//            
//    #method char
//    [
//        #var(type:char)aRetVal := $self char.
//        
//        ^ CharValue new &char:aRetVal.
//    ]
//            
//    #method close
//    [
//        theHandle close.
//    ]
//}
//
//// --- Console Standard Symbols
//
//#symbol stdConsoleWriter = $ConsoleWriter new &handle:($ConsoleHandleFactory new &int_attributes:0FFFFFFF5h).
//
//#symbol stdConsoleReader = $ConsoleReader new &handle:($ConsoleHandleFactory new &int_attributes:0FFFFFFF6h).
