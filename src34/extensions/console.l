//import system'routines.
//import system'text.
//
//// --- outputOp ---
//
//extension outputOp
//{
//    generic print(object<> list)
//    [
//        int len := list length.
//        int i := 0.
//        while (i < len)
//        [
//            object obj := list[i].
//            
//            self write(obj~convertorOp toLiteral).
//            
//            i := i + 1
//        ].
//        
//        ^ self
//    ]
//    
//    generic printWide(object<> list)
//    [
//        int len := list length.
//        int i := 0.
//        while (i < len)
//        [
//            object obj := list[i].
//            
//            self write(obj~convertorOp toWide).
//            
//            i := i + 1
//        ].
//        
//        ^ self
//    ]
//
//    printLine
//    [
//        self writeLine.
//    ]
//            
//    generic printLine(object<> list)
//    [
//        int len := list length.
//        int i := 0.
//        while (i < len)
//        [
//            object obj := list[i].
//            self write(obj~convertorOp toLiteral).            
//            
//            i := i + 1
//        ].
//        
//        ^ self writeLine
//    ]    
//
//    generic printPaddingRight(object<> list)
//    [
//        textbuffer aBuffer := TextBuilder new.
//        
//        int len := list length.
//        int i := 1.
//        while (i < len)
//        [
//            object obj := list[i].
//            
//            aBuffer write(obj~convertorOp toLiteral).
//            
//            i := i + 1
//        ].
//        
//        var aWidth := list[0].
//        
//        self write(aBuffer literal) paddingRight:aWidth with:$32.
//            
//        ^ self
//    ]
//
//    generic printPaddingLeft(object<> list)
//    [
//        textbuffer aBuffer := TextBuilder new.
//        
//        int len := list length.
//        int i := 1.
//        while (i < len)
//        [
//            object obj := list[i].
//            
//            aBuffer write(obj~convertorOp toLiteral).
//            
//            i := i + 1
//        ].
//        
//        var aWidth := list[0].
//        
//        self write(aBuffer literal) paddingLeft:aWidth with:$32.
//            
//        ^ self
//    ]
//
//    generic printPaddingRightChar(object<> list)
//    [
//        textbuffer aBuffer := TextBuilder new.
//        
//        int len := list length.
//        if (len < 2)
//        [
//            InvalidArgumentException new; raise
//        ].
//        
//        int i := 2.
//        while (i < len)
//        [
//            object obj := list[i].
//            
//            aBuffer write(obj~convertorOp toLiteral).
//            
//            i := i + 1
//        ].
//        
//        var aWidth := list[0].
//        var ch := list[1].
//        
//        self write(aBuffer literal) paddingRight:aWidth with:ch.
//            
//        ^ self
//    ]
//
//    generic printPaddingLeftChar(object<> list)
//    [
//        textbuffer aBuffer := TextBuilder new.
//        
//        int len := list length.
//        if (len < 2)
//        [
//            InvalidArgumentException new; raise
//        ].
//        
//        int i := 2.
//        while (i < len)
//        [
//            object obj := list[i].
//            
//            aBuffer write(obj~convertorOp toLiteral).
//            
//            i := i + 1
//        ].
//        
//        var aWidth := list[0].        
//        var ch := list[1].
//        
//        self write(aBuffer literal) paddingLeft:aWidth with:ch.
//            
//        ^ self
//    ]    
//}
//
//// --- formatterOp ---
//
//class $helper =
//{
//    stacksafe readIndex literal:s vint:i vint:retVal
//    [
//        char ch := $0.
//        int  n := 0.
//        int  v := 0.
//        
//        s read int:i vchar:ch.
//        while (ch != $125)
//        [
//            intConvertor convert char:ch vint:n.
//            if((n >= 30h)and:(n <= 39h))
//            [                
//                n := n - 30h.
//                
//                v := v * 10.
//                v := v + n.
//            ];
//            [ InvalidArgumentException new; raise ].
//                
//            i append int:1.
//            s read int:i vchar:ch
//        ].
//        
//        retVal int := v
//    ]
//}.
//
//extension formatterOp
//{
//    generic printFormatted(object<> args)
//    [
//        literal    s := args[0].
//        textbuffer buffer := TextBuilder new.
//        
//        int  len := s length.
//        int  i := 0.
//        char ch := $0.
//        int  chlen := 0.
//        int  index := 0.
//        
//        while (i < len)
//        [
//            s read int:i vchar:ch.
//            
//            if (ch == $123)
//                [ 
//                    i := i + 1.
//                    
//                    $helper readIndex literal:s vint:i vint:index.
//                    buffer write((args[index + 1])~convertorOp toLiteral).
//                    
//                    chlen := 1.
//                ];
//                [ 
//                    buffer write char:ch. 
//                    ch readLengthTo vint:chlen
//                ].
//            
//            i := i + chlen
//        ].
//        
//        self write:buffer.
//        
//        ^ self
//    ]
//    
//    generic printLineFormatted(object<> args)
//    [
//        self~formatterOp printFormatted object<>:args.
//        self writeLine.
//    ]
//}
//
//// --- inputOp ---
//
//extension inputOp
//{
//    readLineTo:var
//    [
//        var aLine := self readLiteral.
//        
//        aLine~convertorOp convert to:var.
//        
//        ^var.
//    ]
//    
//    generic readLine(object<> Arguments)
//    [
//        literal aLine := self readLiteral.
//        
//        array aParams := aLine split char:$32.
//        
//        int aLength := Arguments length.
//        
//        int anIndex := 0.
//        while (anIndex < aLength)
//        [
//            (aParams getAt int:anIndex)~convertorOp convert to(Arguments[anIndex]).
//            
//            anIndex := anIndex + 1
//        ].
//            
//        ^ self
//    ]
//}
