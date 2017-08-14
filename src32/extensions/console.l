import system'routines.
import system'text.

// --- outputOp ---

extension outputOp
{
    generic print args:list
    [
        int len := list length.
        int i := 0.
        while (i < len)
        [
            object obj := list[i].
            
            self write(obj~convertorOp toLiteral).
            
            i := i + 1
        ].
        
        ^ self
    ]
    
    generic printWide args:list
    [
        int len := list length.
        int i := 0.
        while (i < len)
        [
            object obj := list[i].
            
            self write(obj~convertorOp toWide).
            
            i := i + 1
        ].
        
        ^ self
    ]
    
    generic printLine args:list
    [
        int len := list length.
        int i := 0.
        while (i < len)
        [
            object obj := list[i].
            
            self write(obj~convertorOp toLiteral).
            
            i := i + 1
        ].
        
        ^ self writeLine
    ]    

    generic printPaddingRight args:list
    [
        textbuffer aBuffer := TextBuilder new.
        
        int len := list length.
        int i := 1.
        while (i < len)
        [
            object obj := list[i].
            
            aBuffer write(obj~convertorOp toLiteral).
            
            i := i + 1
        ].
        
        var aWidth := list[0].
        
        self write(aBuffer literal) paddingRight:aWidth with:$32.
            
        ^ self
    ]
    
//    print : anObject1 paddingLeft:aWidth
//    [
//        textbuffer aBuffer := TextBuilder new.
//        
//        aBuffer
//            write:(anObject1~convertorOp toLiteral).
//        
//        self write:(aBuffer literal) paddingLeft:aWidth with:$32.
//            
//        ^ self
//    ]
//    
//    print : anObject1: anObject2 paddingLeft:aWidth
//    [
//        textbuffer aBuffer := TextBuilder new.
//        
//        aBuffer
//            write:(anObject1~convertorOp toLiteral);
//            write:(anObject2~convertorOp toLiteral).
//        
//        self write:(aBuffer literal) paddingLeft:aWidth with:$32.
//            
//        ^ self
//    ]
//    
//    print : anObject1 : anObject2 : anObject3 paddingLeft:aWidth
//    [
//        textbuffer aBuffer := TextBuilder new.
//        
//        aBuffer
//            write:(anObject1~convertorOp toLiteral);
//            write:(anObject2~convertorOp toLiteral);
//            write:(anObject3~convertorOp toLiteral).
//        
//        self write:(aBuffer literal) paddingLeft:aWidth with:$32.
//            
//        ^ self
//    ]
//    
//    print : anObject1 : anObject2 : anObject3 : anObject4 paddingLeft:aWidth
//    [
//        textbuffer aBuffer := TextBuilder new.
//        
//        aBuffer
//            write:(anObject1~convertorOp toLiteral);
//            write:(anObject2~convertorOp toLiteral);
//            write:(anObject3~convertorOp toLiteral);
//            write:(anObject4~convertorOp toLiteral).
//        
//        self write:(aBuffer literal) paddingLeft:aWidth with:$32.
//            
//        ^ self
//    ]
}

////// --- formatterOp ---
////
////class $helper =
////{
////    stacksafe readIndex literal:s vint:i vint:retVal
////    [
////        char ch := $0.
////        int  n := 0.
////        int  v := 0.
////        
////        s read int:i vchar:ch.
////        while (ch != $125)
////        [
////            intConvertor convert char:ch vint:n.
////            if((n >= 30h)and:(n <= 39h))
////            [                
////                n := n - 30h.
////                
////                v := v * 10.
////                v := v + n.
////            ];
////            [ InvalidArgumentException new; raise ].
////                
////            i append int:1.
////            s read int:i vchar:ch
////        ].
////        
////        retVal int := v
////    ]
////}.
////
////extension formatterOp
////{
////    generic writeFormatted args:args
////    [
////        literal    s := args@0.
////        textbuffer buffer := TextBuilder new.
////        
////        int  len := s length.
////        int  i := 0.
////        char ch := $0.
////        int  chlen := 0.
////        int  index := 0.
////        
////        while (i < len)
////        [
////            s read int:i vchar:ch.
////            
////            if (ch == $123)
////                [ 
////                    i := i + 1.
////                    
////                    $helper readIndex literal:s vint:i vint:index.
////                    buffer write:((args@(index + 1))~convertorOp toLiteral).
////                    
////                    chlen := 1.
////                ];
////                [ 
////                    buffer write char:ch. 
////                    ch readLengthTo vint:chlen
////                ].            
////            
////            i := i + chlen
////        ].
////        
////        self write:buffer.
////        
////        ^ self
////    ]
////    
////    generic writeLineFormatted args:args
////    [
////        self~formatterOp writeFormatted args:args.
////        self writeLine.
////    ]
////}

// --- inputOp ---

extension inputOp
{
    readLineTo:var
    [
        var aLine := self readLiteral.
        
        aLine~convertorOp convert to:var.
        
        ^var.
    ]
    
    generic readLine args:Arguments
    [
        literal aLine := self readLiteral.
        
        array aParams := aLine split char:$32.
        
        int aLength := Arguments length.
        
        int anIndex := 0.
        while (anIndex < aLength)
        [
            (aParams getAt int:anIndex)~convertorOp convert to(Arguments[anIndex]).
            
            anIndex := anIndex + 1
        ].
            
        ^ self
    ]
}
