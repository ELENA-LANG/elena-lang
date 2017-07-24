// === basic interfaces ===
type encoder :: system'text'Encoder.

// --- Encoder ---

limited class Encoder
{
    stacksafe convert int:anIndex vint:aLength bytearray:aByteArray chararray:anOutput vint:anOutputLength [] 

    stacksafe convert int:anIndex vint:aLength chararray:aCharArray bytearray:anOutput vint:anOutputLength []
    
    stacksafe convert int:anIndex vint:aLength literal:aLiteral bytearray:anOutput vint:anOutputLength
    [
        char temp(64).
        int  temp_len := 64.
        if (temp_len > aLength)
            [ temp_len := aLength ];
            [ aLength int := temp_len ].
        
        aLiteral Substring int:anIndex int:aLength; save int:0 chararray:temp vint:temp_len.
        
        $self convert int:0 vint:temp_len chararray:temp bytearray:anOutput vint:anOutputLength.
    ]
    
    stacksafe convert int:anIndex vint:aLength wide:aLiteral bytearray:anOutput vint:anOutputLength
    [
        char temp(64).
        int  temp_len := 64.
        if (temp_len > aLength)
            [ temp_len := aLength ];
            [ aLength int := temp_len ].
        
        aLiteral Substring int:anIndex int:aLength; save int:0 chararray:temp vint:temp_len.
        
        $self convert int:0 vint:temp_len chararray:temp bytearray:anOutput vint:anOutputLength.
    ]
    
    stacksafe toByteArray int:anIndex int:aLength literal:aLiteral
    [
        int       i := anIndex.
        bytearray aRetVal := $nil.
        int       remaining := aLength.
        
        while (i < aLength)
        [
            byte tmp(64).
            int  tmp_len := 64.
            int  len := remaining.
            if (len > tmp_len)
                [ len := tmp_len ].
             
            $self convert int:i vint:len literal:aLiteral bytearray:tmp vint:tmp_len.
            
            if ($nil == aRetVal)
                [ aRetVal := ByteArray new int:0 int:tmp_len bytearray:tmp ];
                [ aRetVal := aRetVal add int:tmp_len bytearray:tmp ].
            
            i := i + len.
            remaining := remaining - len
        ].
        
        ^ aRetVal
    ]
    
    stacksafe toByteArray int:anIndex int:aLength wide:aLiteral
    [
        int       i := anIndex.
        bytearray aRetVal := $nil.
        int       remaining := aLength.
        
        while (i < aLength)
        [
            byte tmp(64).
            int  tmp_len := 64.
            int  len := remaining.
            if (len > tmp_len)
                [ len := tmp_len ].
             
            $self convert int:i vint:len wide:aLiteral bytearray:tmp vint:tmp_len.
            
            if ($nil == aRetVal)
                [ aRetVal := ByteArray new int:0 int:tmp_len bytearray:tmp ];
                [ aRetVal := aRetVal add int:tmp_len bytearray:tmp ].
            
            i := i + len.
            remaining := remaining - len
        ].
        
        ^ aRetVal
    ]
    
    stacksafe toLiteral int:anIndex int:aLength bytearray:anArray
    [
        int     i := anIndex.
        literal aRetVal := $nil.
        int     remaining := aLength.
        
        while (i < aLength)
        [
            char tmp(64).
            int  tmp_len := 64.
            int  len := remaining.
            if (len > tmp_len)
                [ len := tmp_len ].
             
            $self convert int:i vint:len bytearray:anArray chararray:tmp vint:tmp_len.
            
            if ($nil == aRetVal)
               [ aRetVal := LiteralValue new int:0 int:tmp_len chararray:tmp ];
               [ aRetVal := aRetVal add literal:(LiteralValue new int:0 int:tmp_len chararray:tmp) ].
            
            i := i + len.
            remaining := remaining - len.
        ].
        
        ^ aRetVal.
    ]
    
    stacksafe toWide int:anIndex int:aLength bytearray:anArray
    [
        int  i := anIndex.
        wide aRetVal := $nil.
        int  remaining := aLength.
        
        while (i < aLength)
        [
            char tmp(64).
            int  tmp_len := 64.
            int  len := remaining.
            if (len > tmp_len)
                [ len := tmp_len ].
             
            $self convert int:i vint:len bytearray:anArray chararray:tmp vint:tmp_len.
            
            if ($nil == aRetVal)
               [ aRetVal := WideLiteralValue new int:0 int:tmp_len chararray:tmp ];
               [ aRetVal := aRetVal add wide:(WideLiteralValue new int:0 int:tmp_len chararray:tmp) ].
            
            i := i + len.
            remaining := remaining - len
        ].
        
        ^ aRetVal
    ]
}

// --- UTF8Encoder ---

class UTF8Encoder = Encoder::
{
    convert int:anIndex vint:aLength bytearray:aByteArray chararray:anOutput vint:anOutputLength
        => system'core_routines'__utf8encoder.
    
    convert int:anIndex vint:aLength chararray:aByteArray bytearray:anOutput vint:anOutputLength
        => system'core_routines'__utf8encoder.
    
    stacksafe convert int:anIndex vint:aLength literal:aLiteral bytearray:anOutput vint:anOutputLength
    [
        aLiteral $save int:anIndex bytearray:anOutput int:aLength.

        anOutputLength int := aLength
    ]
    
    stacksafe toLiteral int:anIndex int:aLength bytearray:anArray
        = UTF8Encoding toLiteral int:anIndex int:aLength bytearray:anArray.
}.

// --- UTF8Encoding ---

class UTF8Encoding =
{
    encoder = UTF8Encoder.
    
    stacksafe charCount literal:aLiteral vint:aRetVal
        => system'core_routines'__utf8encoder.
        
    stacksafe convert int:anIndex int:aLength literal:aLiteral bytearray:anArray
    [
        aLiteral $save int:anIndex bytearray:anArray int:aLength.
    ]
        
    stacksafe toLiteral int:anIndex int:aLength bytearray:anArray
        = LiteralValue $new int:anIndex int:aLength bytearray:anArray.
        
    stacksafe convert int:anIndex vint:aLength bytearray:aByteArray chararray:anOutput vint:anOutputLength
        => system'core_routines'__utf8encoder.
        
    stacksafe convert int:anIndex vint:aLength literal:aByteArray chararray:anOutput vint:anOutputLength
        => system'core_routines'__utf8encoder.
}.

// --- UTF16Encoder ---

class UTF16Encoder = Encoder::
{
    convert int:anIndex vint:aLength bytearray:aByteArray chararray:anOutput vint:anOutputLength
        => system'core_routines'__utf16encoder.
    
    convert int:anIndex vint:aLength chararray:aByteArray bytearray:anOutput vint:anOutputLength
        => system'core_routines'__utf16encoder.
}.

// --- UTF16Encoding ---

class UTF16Encoding =
{
    encoder = UTF16Encoder.
    
    stacksafe charCount wide:aLiteral vint:aRetVal
        => system'core_routines'__utf16encoder.
        
    stacksafe convert int:anIndex int:aLength wide:aLiteral shortarray:anArray
    [
        aLiteral $save int:anIndex shortarray:anArray int:aLength.
    ]

    stacksafe convert int:anIndex int:aSize shortarray:aByteArray chararray:anOutput vint:anOutputLength
        => system'core_routines'__utf16encoder.
        
    stacksafe convert int:anIndex int:aSize chararray:aByteArray shortarray:anOutput vint:anOutputLength
        => system'core_routines'__utf16encoder.
        
    stacksafe convert int:anIndex vint:aSize literal:aLiteral shortarray:anOutput vint:anOutputLength
    [
        char temp(64).
        int  temp_len := 64.
        if (temp_len < aSize)
            [ aSize int := temp_len ].
        
        UTF8Encoding convert int:anIndex vint:aSize literal:aLiteral chararray:temp vint:temp_len.
        
        $self convert int:0 int:temp_len chararray:temp shortarray:anOutput vint:anOutputLength
    ]
        
    stacksafe literal toLiteral int:anIndex int:aSize shortarray:anInput
    [
        char temp(64).
        int  temp_len := 64.
        
        $self convert int:anIndex int:aSize shortarray:anInput chararray:temp vint:temp_len.
        
        ^ LiteralValue new int:0 int:temp_len chararray:temp
    ]
}.
