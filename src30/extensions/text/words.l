#define system.
#define system'text.

#class WordEnumerator :: Enumerator
{
    #field(type:literal)    theLiteral.
    #field(type:vint)       theIndex.
    #field(type:textbuffer) theBuffer.
    
    #constructor new &literal:aLiteral
    [
        theLiteral := aLiteral.
        theIndex := Integer new.
        theBuffer := TextBuffer new.
    ]    
    
    #method reset 
    [
        theIndex write &int:0.
    ]
    
    #method next
    [
        theBuffer clear.

        #var(type:int)aLength.
        #var(type:int)aCurrent := theIndex.
        #var(type:char)aChar.
        #var(type:bool)aSpaceMode := true.
        #var(type:int)aCharLen.
        
        theLiteral readLength &vint:aLength.
        
        #loop (aCurrent < aLength) ?
        [
            theLiteral read &int:aCurrent &vchar:aChar.
            (aChar == 32)
                ? [
                    aSpaceMode ! [ theIndex write &int:aCurrent. ^ true. ].                    
                ]
                ! [
                    aSpaceMode ? [ aSpaceMode := false. ].
                        
                    theBuffer write &char:aChar.
                ].
                
            aChar readLength &vint:aCharLen.
            aCurrent := aCurrent + aCharLen.
        ].

        theIndex write &int:aCurrent.

        ^ aSpaceMode not.
    ]
    
    #method get = theBuffer literal.
}

