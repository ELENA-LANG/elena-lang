const int TextPageSize = 128;

// --- TextBuffer ---

public sealed class TextBuilder
{
    char[]    theBuffer;
    ref<int>  theLength;

    constructor()
    {
        theBuffer := new char[](TextPageSize);
        theLength := 0;
    }

//    constructor new : value
//        <= new;
//    [
//        self write(value).
//    ] 
//
    vprop int Length
    {
        read(ref int retVal)
        {
            theLength.readValueTo(ref retVal)
        }
            
        setLength(int len)
        {
            theLength.Value := len
        }
    }

    clear()
    {
        theLength.Value := 0
    }

    char[] cast()
        = theBuffer;

    string cast()
        = String.new(0, cast int(theLength), theBuffer);
        
    string Value
        = String.new(0, cast int(theLength), theBuffer);

    string Printable
        = self.Value;

//    cast T<wide>
//        = WideLiteralValue new(0, T<int>(theLength), theBuffer).
//
//    writeLine
//    [
//        self write:forwards'newLine.
//    ]

    append(line)
    {
        self.write:line
    }
                                    
    private reserve(int len)
    {
        int newLength := len;
        
        newLength := newLength + (cast int(theLength));
        
        int bufferLength := theBuffer.Length;
        
        if (newLength >= bufferLength)
        {
            //aNewLength := (aNewLength / $TextPageSize + 1) * $TextPageSize.
            newLength := newLength / TextPageSize;
            newLength := newLength + 1;
            newLength := newLength * TextPageSize;
            
            char[] newBuffer := new char[](newLength);
            
            newBuffer.write(0, bufferLength, theBuffer);
            
            theBuffer := newBuffer
        }
    }

    write(string s)
    {
        int length := s.Length;
        
        ifnot (0 == length)
        {
            self.reserve(length);
            
            s.save(cast int(theLength), theBuffer, ref length);
            
            theLength.append(length)
        }
    }

//    write(wide aLiteral)
//    [
//        int aLength := aLiteral length.
//        
//        ifnot (0 == aLength)
//        [
//            self reserve(aLength).
//            
//            aLiteral save(T<int>(theLength), theBuffer, &aLength).
//            
//            theLength append(aLength).
//        ].
//    ]

    write(char ch)
    {
        self.reserve(1);
        
        theBuffer[cast int(theLength)] := ch;
        
        theLength.append(1)
    }

    write(int n)
    {
        byte tmp[16];
        char ch_tmp[16];
        int  len := 16;
        
        stringConvertor.convert(n, tmp, ref len);
        
        UTF8Encoder.convert(0, ref len, tmp, ch_tmp, ref len);
        
        self.write(ch_tmp, len)
    }

//    write : anObject
//    [
//        self write(anObject literal).
//    ]

    write(char[] array, int length)
    {
        ifnot (0 == length)
        {
            self.reserve(length);
            
            theBuffer.write(cast int(theLength), length, array);
            
            theLength.append(length)
        }
    }

    private readIndex(string s, ref int i, ref int retVal)
    {
        char ch := $0;
        int  n := 0;
        int  v := 0;
        
        int i_val := i;
        
        s.read(i_val, ref ch);
        while (ch != $125)
        {
            intConvertor.convert(ch, ref n);
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
            i_val := i;
            s.read(i_val, ref ch)
        };
        
        retVal := v
    }

    writeFormatted(string s, params object[] args)
    {
        int  len := s.Length;
        int  i := 0;
        char ch := $0;
        int  chlen := 0;
        int  index := 0;
        
        while (i < len)
        {
            s.read(i, ref ch);
            
            if (ch == $123)
            { 
                i := i + 1;
                
                self.readIndex(s, ref i, ref index);
                self.write(args[index]);
                
                chlen := 1
            }
            else
            {
                self.write(ch);
                chlen := ch.Length
            };
            
            i := i + chlen
        }
    }

    fill(string s, int counter)
    {
        for (int i := 0, i < counter, i += 1)
        {
            self.write(s)
        }
    }

    fill(wide s, int counter)
    {
        for (int i := 0, i < counter, i += 1)
        {
            self.write(s)
        }
    }

    fill(char ch, int counter)
    {
        int i := 0;
        while (i < counter)
        {
            self.write(ch);
            
            i += 1
        }
    }

//    insert(int anIndex, int aLength, CharArray anArray)
//    [
//        self reserve(aLength).
//        
//        theBuffer move(anIndex, T<int>(theLength), aLength).
//        
//        theBuffer write(anIndex, aLength, anArray).
//        
//        theLength append(aLength).
//    ]

    insert(int index, string s)
    {
        int length := 0;
        UTF8Encoding.charCount(s, ref length);

        self.reserve(length);
        
        theBuffer.move(index, cast int(theLength), length);
        
        s.save(index, theBuffer, ref length);
        
        theLength.append(length)
    }

//    insert(int anIndex, wide aLiteral)
//    [
//        int aLength := 0.
//        UTF16Encoding charCount(aLiteral, &aLength).
//
//        self reserve(aLength).
//        
//        theBuffer move(anIndex, T<int>(theLength), aLength).
//        
//        aLiteral save(anIndex, theBuffer, &aLength).
//        
//        theLength append(aLength).
//    ]
//    
//    insert(int anIndex, char aChar)
//    [
//        self reserve(1).
//        
//        theBuffer move(anIndex, T<int>(theLength), 1).
//        
//        theBuffer write(anIndex, aChar).
//        
//        theLength append(1).
//    ]
//
//    insert : anObject at:anIndex
//        <= insert(T<int>(anIndex), anObject).
//
//    indexOf(int aPosition, char aValue, ref<int> aRetVal)
//    [
//        int  anIndex := 0.
//        char aCurrent := $0.
//        
//        while (anIndex < T<int>(theLength))
//        [
//            theBuffer read(anIndex, &aCurrent).
//            
//            if (aCurrent == aValue)
//                [ aRetVal value := anIndex. ^self ].
//                
//            anIndex += 1
//        ].
//        
//        aRetVal value := -1
//    ]

    indexOf(int pos, string s, ref int retVal)
    {
        int  strLen := 0;
        int  index := 0;
        int  firstChLen := 0;
        char current := $0;
        char firstCh := $0;
        char nextCh := $0;
        
        strLen := s.Length;
        
        s.read(0, ref firstCh);
        firstChLen := firstCh.Length;
        
        int maxIndex := theLength;
        maxIndex := maxIndex - strLen;
        
        while (index < maxIndex)
        {
            current := theBuffer[index];
            
            if (current == firstCh)
            {
                int j := firstChLen;
                int chLen := 0;
                while (j < strLen)
                {
                    current := theBuffer[index + j];
                    s.read(j, ref nextCh);
                    
                    if (nextCh == current)
                    { 
                        chLen := nextCh.Length
                    }
                    else
                    { 
                        j := strLen; 
                        chLen := 1 
                    };
                    
                    j := j + chLen
                };
                
                if (j == strLen)
                { 
                    retVal := index; 
                    ^ self 
                }
            };
                
            index += 1
        };
        
        retVal := -1
    }

//    indexOf(int aPosition, wide aLiteral, ref<int> aRetVal)
//    [
//        int  aLiteralLength := 0.
//        int  anIndex := 0.
//        int  aFirstCharLen := 0.
//        char aCurrent := $0.
//        char aFirstChar := $0.
//        char aNextChar := $0.
//        
//        aLiteral readLengthTo(&aLiteralLength).
//        
//        aLiteral read(0, &aFirstChar).
//        aFirstChar readWideLengthTo(&aFirstCharLen).
//        
//        while (anIndex < theLength)
//        [
//            theBuffer read(anIndex, &aCurrent).
//            
//            if (aCurrent == aFirstChar)
//            [ 
//                int j := aFirstCharLen.
//                int chLen := 0.
//                while (j < aLiteralLength)
//                [
//                    theBuffer read(anIndex + j, &aCurrent).
//                    aLiteral read(j, &aNextChar).
//                    
//                    if (aNextChar == aCurrent)
//                        [ aNextChar readWideLengthTo(&chLen) ];
//                        [ j := aLiteralLength. chLen := 1 ].
//                    
//                    j := j + chLen
//                ].
//                
//                if (j > aLiteralLength)
//                    [ aRetVal value := anIndex. ^ self ]
//            ].
//                
//            anIndex := anIndex + 1
//        ].
//        
//        aRetVal value := -1
//    ]
//
//    indexOf(int aPosition, char aValue)
//    [
//        int aRetVal := 0.
//        self indexOf(aPosition, aValue, &aRetVal).
//        
//        ^ IntNumber new(aRetVal)
//    ]
//    
//    indexOf(int aPosition, literal aValue)
//    [
//        int aRetVal := 0.
//        self indexOf(aPosition, aValue, &aRetVal).
//        
//        ^ IntNumber new(aRetVal)
//    ]
//    
//    indexOf(int aPosition, wide aValue)
//    [
//        int aRetVal := 0.
//        self indexOf(aPosition, aValue, &aRetVal).
//        
//        ^ IntNumber new(aRetVal)
//    ]

    string Substring(int index, int length)
    {
        if (length == 0)
            { ^ emptyString };
            
        ^ String.new(index, length, theBuffer)
    }

//    wide WideSubstring(int anIndex, int aLength)
//    [
//        if (aLength == 0)
//            [ ^ emptyWideLiteral ].
//            
//        ^ WideLiteralValue new(anIndex, aLength, theBuffer)
//    ]

    delete(int index, int length)
    {
        int n := 0 - length;
        int l := theLength;
        l := l - index;
        l := l - length;
        
        theBuffer.move(index + length, l, n);
        
        theLength.reduce(length)
    }
    
    trim(int n)
    {
        theLength.reduce(n)
    }
}
