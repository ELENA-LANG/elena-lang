public const string newLine
    = $13$10;

// --- characterController ---

singleton characterController
{
//    // !! should be corrected
//    isUpperCase(char aValue)
//    [
//        int aRetVal := system'external'USER32 IsCharUpperW(aValue).
//        
//        ^ (aRetVal != 0)
//    ]
//    
//    isLowerCase(char aValue)
//    [
//        int aRetVal := system'external'USER32 IsCharLowerW(aValue).
//        
//        ^ (aRetVal != 0)
//    ]
    
    toUpperCase(char char, ref char retVal)
    {
        int   length := 0;
        short buffer[3];

        charConvertor.save(char, 0, buffer, ref length);

        buffer[length] := 0;
        
        extern USER32.CharUpperW(buffer);
        
        charConvertor.convert(0, buffer, ref retVal)
    }
    
//    toLowerCase(char aChar, ref<char> aRetVal)
//    [
//        int        aLength := 0.
//        T<short,3> aBuffer.
//
//        charConvertor save(aChar, 0, T<ShortArray>(aBuffer), &aLength).
//        
//        aBuffer[aLength] := 0.
//        
//        system'external'USER32 CharLowerW(T<ShortArray>(aBuffer)).
//        
//        charConvertor convert(0, T<ShortArray>(aBuffer), aRetVal)
//    ]
}

// --- caseLitrealOp ---

public extension caseStringOp : String
{
    upperCase()
    {
        string retVal := emptyString;
        int    index := 0;
        char   current := $0;
        char   converted := $0;
        int    currentLen := 0;
        
        int     length := self.Length;
        
        while (index < length)
        {
            self.read(index, ref current);
            currentLen := current.Length;
            
            characterController.toUpperCase(current, ref converted);
            
            retVal := retVal.add(converted);
            
            index := index + currentLen
        };
            
        ^ retVal
    }

//    lowerCase
//    [
//        literal aRetVal := emptyLiteral.        
//        int     anIndex := 0.
//        char    aCurrent := $0.
//        char    aConverted := $0.
//        int     aCurrentLen := 0.
//        
//        int     aLength := self length.
//        
//        while (anIndex < aLength)
//        [
//            self read(anIndex, &aCurrent).
//            aCurrentLen := aCurrent length.
//            
//            characterController toLowerCase(aCurrent, &aConverted).
//            
//            aRetVal := aRetVal add(aConverted).
//            
//            anIndex := anIndex + aCurrentLen
//        ].
//            
//        ^ aRetVal
//    ]    
}

//// --- caseLitrealOp ---
//
//public extension<wide> caseWideOp
//{
//    upperCase
//    [
//        wide aRetVal := emptyWideLiteral.        
//        int  anIndex := 0.
//        char aCurrent := $0.
//        char aConverted := $0.
//        int  aCurrentLen := 0.
//
//        int  aLength := self length.
//                
//        while (anIndex < aLength)
//        [
//            self read(anIndex, &aCurrent).
//            aCurrentLen := aCurrent length.
//            
//            characterController toUpperCase(aCurrent, &aConverted).
//            
//            aRetVal := aRetVal add(aConverted).
//            
//            anIndex := anIndex + aCurrentLen
//        ].
//            
//        ^ aRetVal
//    ]
//
//    lowerCase
//    [
//        wide aRetVal := emptyWideLiteral.        
//        int  anIndex := 0.
//        char aCurrent := $0.
//        char aConverted := $0.
//        int  aCurrentLen := 0.
//        
//        int  aLength := self length.
//        
//        while (anIndex < aLength)
//        [
//            self read(anIndex, &aCurrent).
//            aCurrentLen := aCurrent length.
//            
//            characterController toLowerCase(aCurrent, &aConverted).
//            
//            aRetVal := aRetVal add(aConverted).
//            
//            anIndex := anIndex + aCurrentLen
//        ].
//            
//        ^ aRetVal
//    ]
//}