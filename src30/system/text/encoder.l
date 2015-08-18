#import system. 

// === basic interfaces ===
#subject(class:system'text'Encoder)encoder.

// --- Encoder ---

#class(limited)Encoder
{
    #method encoder = $self.
    
    #method(stacksafe) convert &int:anIndex &vint:aLength &bytearray:aByteArray &chararray:anOutput &vint:anOutputLength [] 

    #method(stacksafe) convert &int:anIndex &vint:aLength &chararray:aCharArray &bytearray:anOutput &vint:anOutputLength []
    
    #method(stacksafe) convert &int:anIndex &vint:aLength &literal:aLiteral &bytearray:anOutput &vint:anOutputLength
    [
        #var(type:chararray,size:64)temp.
        #var(type:int)temp_len := 64.
        (temp_len > aLength)
            ? [ temp_len := aLength. ]
            ! [ aLength << temp_len. ].
        
        aLiteral Substring &int:anIndex &int:aLength save &int:0 &chararray:temp &vint:temp_len.
        
        $self convert &int:0 &vint:temp_len &chararray:temp &bytearray:anOutput &vint:anOutputLength.
    ]
    
    #method(stacksafe) convert &int:anIndex &vint:aLength &wide:aLiteral &bytearray:anOutput &vint:anOutputLength
    [
        #var(type:chararray,size:64)temp.
        #var(type:int)temp_len := 64.
        (temp_len > aLength)
            ? [ temp_len := aLength. ]
            ! [ aLength << temp_len. ].
        
        aLiteral Substring &int:anIndex &int:aLength save &int:0 &chararray:temp &vint:temp_len.
        
        $self convert &int:0 &vint:temp_len &chararray:temp &bytearray:anOutput &vint:anOutputLength.
    ]
    
    #method(stacksafe) toByteArray &int:anIndex &int:aLength &literal:aLiteral
    [
        #var(type:int)i := anIndex.
        #var(type:bytearray)aRetVal := $nil.
        #var(type:int)remaining := aLength.
        
        #loop (i < aLength)?
        [
            #var(type:bytearray,size:64)tmp.
            #var(type:int)tmp_len := 64.
            #var(type:int)len := remaining.
            (len > tmp_len)
                ? [ len := tmp_len. ].
             
            $self convert &int:i &vint:len &literal:aLiteral &bytearray:tmp &vint:tmp_len.
            
            ($nil == aRetVal)
                ? [ aRetVal := ByteArray new &int:0 &int:tmp_len &bytearray:tmp. ]
                ! [ aRetVal := aRetVal add &int:tmp_len &bytearray:tmp ].
            
            i := i + len.
            remaining := remaining - len.
        ].
        
        ^ aRetVal.
    ]
    
    #method(stacksafe) toByteArray &int:anIndex &int:aLength &wide:aLiteral
    [
        #var(type:int)i := anIndex.
        #var(type:bytearray)aRetVal := $nil.
        #var(type:int)remaining := aLength.
        
        #loop (i < aLength)?
        [
            #var(type:bytearray,size:64)tmp.
            #var(type:int)tmp_len := 64.
            #var(type:int)len := remaining.
            (len > tmp_len)
                ? [ len := tmp_len. ].
             
            $self convert &int:i &vint:len &wide:aLiteral &bytearray:tmp &vint:tmp_len.
            
            ($nil == aRetVal)
                ? [ aRetVal := ByteArray new &int:0 &int:tmp_len &bytearray:tmp. ]
                ! [ aRetVal := aRetVal add &int:tmp_len &bytearray:tmp ].
            
            i := i + len.
            remaining := remaining - len.
        ].
        
        ^ aRetVal.
    ]
    
    #method(stacksafe) toLiteral &int:anIndex &int:aLength &bytearray:anArray
    [
        #var(type:int)i := anIndex.
        #var(type:literal)aRetVal := $nil.
        #var(type:int)remaining := aLength.
        
        #loop (i < aLength)?
        [
            #var(type:chararray,size:64)tmp.
            #var(type:int)tmp_len := 64.
            #var(type:int)len := remaining.
            (len > tmp_len)
                ? [ len := tmp_len. ].
             
            $self convert &int:i &vint:len &bytearray:anArray &chararray:tmp &vint:tmp_len.
            
            ($nil == aRetVal)
               ? [ aRetVal := LiteralValue new &int:0 &int:tmp_len &chararray:tmp. ]
               ! [ aRetVal := aRetVal add &literal:(LiteralValue new &int:0 &int:tmp_len &chararray:tmp). ].
            
            i := i + len.
            remaining := remaining - len.
        ].
        
        ^ aRetVal.
    ]
    
    #method(stacksafe) toWide &int:anIndex &int:aLength &bytearray:anArray
    [
        #var(type:int)i := anIndex.
        #var(type:wide)aRetVal := $nil.
        #var(type:int)remaining := aLength.
        
        #loop (i < aLength)?
        [
            #var(type:chararray,size:64)tmp.
            #var(type:int)tmp_len := 64.
            #var(type:int)len := remaining.
            (len > tmp_len)
                ? [ len := tmp_len. ].
             
            $self convert &int:i &vint:len &bytearray:anArray &chararray:tmp &vint:tmp_len.
            
            ($nil == aRetVal)
               ? [ aRetVal := WideLiteralValue new &int:0 &int:tmp_len &chararray:tmp. ]
               ! [ aRetVal := aRetVal add &wide:(WideLiteralValue new &int:0 &int:tmp_len &chararray:tmp). ].
            
            i := i + len.
            remaining := remaining - len.
        ].
        
        ^ aRetVal.
    ]
}

// --- UTF8Encoder ---

#symbol UTF8Encoder = Encoder
{
    #method convert &int:anIndex &vint:aLength &bytearray:aByteArray &chararray:anOutput &vint:anOutputLength
        => system'internal's_encode.
    
    #method convert &int:anIndex &vint:aLength &chararray:aByteArray &bytearray:anOutput &vint:anOutputLength
        => system'internal's_decode.
    
    #method(stacksafe) convert &int:anIndex &vint:aLength &literal:aLiteral &bytearray:anOutput &vint:anOutputLength
    [
        aLiteral $save &int:anIndex &bytearray:anOutput &int:aLength.

        anOutputLength << aLength.
    ]
    
    #method(stacksafe) toLiteral &int:anIndex &int:aLength &bytearray:anArray
        = UTF8Encoding toLiteral &int:anIndex &int:aLength &bytearray:anArray.
}.

// --- UTF8Encoding ---

#symbol UTF8Encoding =
{
    encoder = UTF8Encoder.
    
    #method(stacksafe)charCount &literal:aLiteral &vint:aRetVal
        => system'internal's_charcount.
        
    #method(stacksafe)convert &int:anIndex &int:aLength &literal:aLiteral &bytearray:anArray
    [
        aLiteral $save &int:anIndex &bytearray:anArray &int:aLength.
    ]
        
    #method(stacksafe) toLiteral &int:anIndex &int:aLength &bytearray:anArray
        = LiteralValue $new &int:anIndex &int:aLength &bytearray:anArray.
        
    #method(stacksafe) convert &int:anIndex &vint:aLength &bytearray:aByteArray &chararray:anOutput &vint:anOutputLength
        => system'internal's_encode.   
        
    #method(stacksafe) convert &int:anIndex &vint:aLength &literal:aByteArray &chararray:anOutput &vint:anOutputLength
        => system'internal's_encode.   
}.

// --- UTF16Encoder ---

#symbol UTF16Encoder = Encoder
{
    #method convert &int:anIndex &vint:aLength &bytearray:aByteArray &chararray:anOutput &vint:anOutputLength
        => system'internal'ws_encode.
    
    #method convert &int:anIndex &vint:aLength &chararray:aByteArray &bytearray:anOutput &vint:anOutputLength
        => system'internal'ws_decode.
}.

// --- UTF16Encoding ---

#symbol UTF16Encoding =
{
    encoder = UTF16Encoder.
    
    #method(stacksafe)charCount &wide:aLiteral &vint:aRetVal
        => system'internal'ws_charcount.
        
    #method(stacksafe)convert &int:anIndex &int:aLength &wide:aLiteral &shortarray:anArray
    [
        aLiteral $save &int:anIndex &shortarray:anArray &int:aLength.
    ]

    #method(stacksafe)convert &int:anIndex &int:aSize &shortarray:aByteArray &chararray:anOutput &vint:anOutputLength
        => system'internal'sa_encode.
        
    #method(stacksafe)convert &int:anIndex &int:aSize &chararray:aByteArray &shortarray:anOutput &vint:anOutputLength
        => system'internal'sa_decode.
        
    #method(stacksafe)convert &int:anIndex &vint:aSize &literal:aLiteral &shortarray:anOutput &vint:anOutputLength
    [
        #var(type:chararray,size:64)temp.
        #var(type:int)temp_len := 64.
        (temp_len < aSize)
            ? [ aSize << temp_len. ].
        
        UTF8Encoding convert &int:anIndex &vint:aSize &literal:aLiteral &chararray:temp &vint:temp_len.
        
        $self convert &int:0 &int:temp_len &chararray:temp &shortarray:anOutput &vint:anOutputLength.            
    ]
        
    #method(stacksafe,type:literal)toLiteral &int:anIndex &int:aSize &shortarray:anInput
    [
        #var(type:chararray,size:64)temp.
        #var(type:int)temp_len := 64.
        
        $self convert &int:anIndex &int:aSize &shortarray:anInput &chararray:temp &vint:temp_len.
        
        ^ LiteralValue new &int:0 &int:temp_len &chararray:temp.
    ]
}.
