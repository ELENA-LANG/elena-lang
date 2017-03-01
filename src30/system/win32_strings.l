// --- characterController ---

class $characterController =
{
    // !! should be corrected
    isUpperCase &char:aValue
    [
        int aRetVal := system'external'USER32 IsCharUpperW &char:aValue.
        
        ^ (aRetVal != 0).
    ]
    
    isLowerCase &char:aValue
    [
        int aRetVal := system'external'USER32 IsCharLowerW &char:aValue.
        
        ^ (aRetVal != 0).
    ]
    
    stacksafe toUpperCase &char:aChar &vchar:aRetVal
    [
        int      aLength := 0.
        short(3) aBuffer := 0.

        charConvertor $save &char:aChar &int:0 &shortarray:aBuffer &vint:aLength.

        aBuffer@aLength := 0.
        
        system'external'USER32 CharUpperW &shortarray:aBuffer.
        
        charConvertor $convert &int:0 &shortarray:aBuffer &vchar:aRetVal.
    ]
    
    stacksafe toLowerCase &char:aChar &vchar:aRetVal
    [
        int      aLength := 0.
        short(3) aBuffer := 0.

        charConvertor $save &char:aChar &int:0 &shortarray:aBuffer &vint:aLength.
        
        aBuffer@aLength := 0.
        
        system'external'USER32 CharLowerW &shortarray:aBuffer.
        
        charConvertor $convert &int:0 &shortarray:aBuffer &vchar:aRetVal.
    ]
}.

//// --- caseLitrealOp ---
//
//#class(extension:literal)caseLiteralOp
//{
//    #method upperCase
//    [
//        #var(literal)aRetVal := emptyLiteralValue.        
//        #var(int)anIndex := 0.
//        #var(char)aCurrent.
//        #var(char)aConverted.
//        #var(int)aCurrentLen.
//        
//        #var(int)aLength := self length.
//        
//        #loop (anIndex < aLength)?
//        [
//            self read &int:anIndex &vchar:aCurrent.
//            aCurrentLen := aCurrent length.
//            
//            $characterController toUpperCase &char:aCurrent &vchar:aConverted.
//            
//            aRetVal := aRetVal add &char:aConverted.
//            
//            anIndex := anIndex + aCurrentLen.
//        ].
//            
//        ^ aRetVal.
//    ]
//
//    #method lowerCase
//    [
//        #var(literal)aRetVal := emptyLiteralValue.        
//        #var(int)anIndex := 0.
//        #var(char)aCurrent.
//        #var(char)aConverted.
//        #var(int)aCurrentLen.
//        
//        #var(int)aLength := self length.
//        
//        #loop (anIndex < aLength)?
//        [
//            self read &int:anIndex &vchar:aCurrent.
//            aCurrentLen := aCurrent length.
//            
//            $characterController toLowerCase &char:aCurrent &vchar:aConverted.
//            
//            aRetVal := aRetVal add &char:aConverted.
//            
//            anIndex := anIndex + aCurrentLen.
//        ].
//            
//        ^ aRetVal.
//    ]    
//}
//
//// --- caseLitrealOp ---
//
//#class(extension:wide)caseWideOp
//{
//    #method upperCase
//    [
//        #var(wide)aRetVal := emptyWideLiteralValue.        
//        #var(int)anIndex := 0.
//        #var(char)aCurrent.
//        #var(char)aConverted.
//        #var(int)aCurrentLen.
//
//        #var(int)aLength := self length.
//                
//        #loop (anIndex < aLength)?
//        [
//            self read &int:anIndex &vchar:aCurrent.
//            aCurrentLen := aCurrent length.
//            
//            $characterController toUpperCase &char:aCurrent &vchar:aConverted.
//            
//            aRetVal := aRetVal add &char:aConverted.
//            
//            anIndex := anIndex + aCurrentLen.
//        ].
//            
//        ^ aRetVal.
//    ]
//
//    #method lowerCase
//    [
//        #var(wide)aRetVal := emptyWideLiteralValue.        
//        #var(int)anIndex := 0.
//        #var(char)aCurrent.
//        #var(char)aConverted.
//        #var(int)aCurrentLen.
//        
//        #var(int)aLength := self length.
//        
//        #loop (anIndex < aLength)?
//        [
//            self read &int:anIndex &vchar:aCurrent.
//            aCurrentLen := aCurrent length.
//            
//            $characterController toLowerCase &char:aCurrent &vchar:aConverted.
//            
//            aRetVal := aRetVal add &char:aConverted.
//            
//            anIndex := anIndex + aCurrentLen.
//        ].
//            
//        ^ aRetVal.
//    ]
//}