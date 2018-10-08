import system'routines.
import system'text.

// --- outputOp ---

public extension outputOp
{
    generic print(dynamic<object> list)
    [
        int len := list length.
        int i := 0.
        while (i < len)
        [
            object obj := list[i].
            
            self write(obj toLiteral).
            
            i := i + 1
        ].
    ]
    
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

    printLine
    [
        self writeLine.
    ]
            
    generic printLine(dynamic<object> list)
    [
        int len := list length.
        int i := 0.
        while (i < len)
        [
            object obj := list[i].
            self write(obj~convertorOp toLiteral).            
            
            i := i + 1
        ].
        
        ^   self writeLine
    ]    
}

public extension outputFormatterOp
{
    generic printPaddingRight(int width, V<object> list)
    [
        auto aBuffer := TextBuilder new.
        
        int len := list length.
        int i := 0.
        while (i < len)
        [
            object obj := list[i].
            
            aBuffer write(obj~convertorOp toLiteral).
            
            i := i + 1
        ].
        
        self write:(aBuffer literal) paddingRight:width with:$32.
            
        ^ self
    ]

    generic printPaddingLeft(int width, V<object> list)
    [
        auto aBuffer := TextBuilder new.
        
        int len := list length.
        int i := 0.
        while (i < len)
        [
            object obj := list[i].
            
            aBuffer write(obj~convertorOp toLiteral).
            
            i := i + 1
        ].
        
        self write:(aBuffer literal) paddingLeft:width with:$32.
            
        ^ self
    ]
}

public extension outputCharFormatterOp
{
    generic printPaddingRightChar(int width, char ch, V<object> list)
    [
        auto aBuffer := TextBuilder new.
        
        int len := list length.

        int i := 0.
        while (i < len)
        [
            object obj := list[i].
            
            aBuffer write(obj~convertorOp toLiteral).
            
            i := i + 1
        ].
        
        self write:(aBuffer literal) paddingRight:width with:ch.
            
        ^ self
    ]

    generic printPaddingLeftChar(int width, char ch, V<object> list)
    [
        auto aBuffer := TextBuilder new.
        
        int len := list length.
        int i := 0.
        while (i < len)
        [
            object obj := list[i].
            
            aBuffer write(obj~convertorOp toLiteral).
            
            i := i + 1
        ].
        
        self write:(aBuffer literal) paddingLeft:width with:ch.
            
        ^ self
    ]    
}

// --- formatterOp ---

singleton helper
{
    readIndex(literal s, ref<int> i, ref<int> retVal)
    [
        char ch := $0.
        int  n := 0.
        int  v := 0.
        
        s read(T<int>(i), &ch).
        while (ch != $125)
        [
            intConvertor convert(ch, &n).
            if((n >= 30h)&&(n <= 39h))
            [                
                n := n - 30h.
                
                v := v * 10.
                v := v + n.
            ];
            [ InvalidArgumentException new; raise ].
                
            i append(1).
            s read(T<int>(i), &ch)
        ].
        
        retVal value := v
    ]
}

public extension formatterOp
{
    generic printFormatted(literal s, V<object> args)
    [
        auto buffer := TextBuilder new.
        
        int  len := s length.
        int  i := 0.
        char ch := $0.
        int  chlen := 0.
        int  index := 0.
        
        while (i < len)
        [
            s read(i, &ch).
            
            if (ch == $123)
                [ 
                    i := i + 1.
                    
                    helper readIndex(s, &i, &index).
                    buffer write((args[index])~convertorOp toLiteral).
                    
                    chlen := 1.
                ];
                [ 
                    buffer write(ch). 
                    ch readLengthTo(&chlen)
                ].
            
            i := i + chlen
        ].
        
        self write:buffer.
        
        ^ self
    ]
    
    generic printLineFormatted(literal s, V<object> args)
    [
        self~formatterOp printFormatted(s, args).
        self writeLine.
    ]
}

// --- inputOp ---

public extension inputOp
{
    readLineTo:var
    [
        var aLine := self readLine.
        
        aLine~convertorOp convertTo:var.
        
        ^var.
    ]
    
    generic readLine(V<object> args)
    [
        literal aLine := self readLine.
        
        T<Array> aParams := aLine split($32).
        
        int aLength := args length.
        
        int anIndex := 0.
        while (anIndex < aLength)
        [
            (aParams getAt(anIndex))~convertorOp convertTo(args[anIndex]).
            
            anIndex := anIndex + 1
        ].
            
        ^ self
    ]
}
