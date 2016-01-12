#import system.
#import system'collections.
#import system'text.
#import system'dynamic.
#import extensions.
#import extensions'text.

// --- IOException ---

#class JsonException :: Exception
{
    #constructor new
        <= %new : "Json Deserializer error".
        
    #constructor new : aMessage
        <= %new &literal:(aMessage literal) &subject: %ScriptError.
}

#symbol jsonDispatcher =
{
    eval:output &int:n
    [
        output write:(n literal).
    ]
    
    eval:output &bool:b
    [
        output write:(b literal).
    ]
    
    eval:output &array:array
    [
        output::jsonSerializer jsonSerialize &enumerable:array.
    ]
    
    eval:output &enumerable:list
    [
        output::jsonSerializer jsonSerialize &enumerable:list.
    ]
    
    eval:output &dto:object
    [
        output::jsonSerializer jsonSerialize &dto:object.
    ]
    
    eval:output &literal:s
    [
        output writeLiteral:"""":s:"""".
    ]
    
    ! : output : object
        = output writeLiteral:"""":(object literal):"""".
}.

#class(extension:textbuffer)jsonSerializer
{
    #method jsonSerialize &dto:object
    [
        #var(type:textbuffer)output := self textbuffer.
        
        output write:"{".
                
        #var(type:mssgarray,size:10) properties.
        #var(type:int)len := 10.
        #var(type:int)offs := 0.

        object read &mssg_mask:%get &index:0 &mssgarray:properties &vint:len.
        #loop (len > 0) ?
        [
            #var(type:int)i:=0.
            #loop (i < len)?
            [
                #var(type:mssg) message := properties@i.
                
                output writeLiteral:"""":(message subject name):""":".
                
                output::jsonSerializer jsonSerialize:(object::message eval).
                
                output write:",".
                
                i := i + 1.
            ].
            
            offs := offs + len.
            
            object read &mssg_mask:%get &index:offs &mssgarray:properties &vint:len.
        ].
        
        #var(type:int)outputLen := output length.
        (outputLen > 1)
            ? [ output setLength &int:(outputLen - 1). ].
        
        output write:"}".
        
        ^ self.
    ]    
    
    #method jsonSerialize &enumerable:object
    [
        #var(type:textbuffer)output := self textbuffer.
        
        output write:"[".

        object run:output &eachPair:(:output:object)
        [
            output::jsonSerializer jsonSerialize:object.
            
            output write:",".
        ].
        
        #var(type:int)outputLen := output length.
        (outputLen > 1)
            ? [ output setLength &int:(outputLen - 1). ].
        
        output write:"]".
        
        ^ self.
    ]    
    
    #method jsonSerialize : object
    [
        ($nil != object)
            ? [
                object cast:%eval &to:jsonDispatcher &with:self.
            ]
            ! [ self write:"null". ].
            
        ^ self.            
    ]
}
#symbol $helper =
{
    #method deserializeStruct:target &enumerator:e
    [
        #var(type:literal)token := $helper $readNext &enumerator:e.
        #var(type:bool)eof := false.
        #loop (eof)!
        [
            #var(type:literal)field := $helper $getIdentifier:token.
            
            ($helper $readNext &enumerator:e == ":")
                ! [ #throw JsonException new. ].
            
            #var object := $helper deserializeObject &enumerator:e.
            
            target::(Signature new &literal:field) set:object.
            
            token := $helper $readNext &enumerator:e.
            token =>
                "," ? [ token := $helper $readNext &enumerator:e. ]
                "}" ? [ eof := true. ]
                ! [ #throw JsonException new. ].
        ].
        
        ^ target.
    ]
    
    #method deserializeArray:target &enumerator:e
    [
        #var(type:literal)token := $helper $readNext &enumerator:e.
        #var(type:bool)eof := false.
        #loop (eof)!
        [
            target += $helper deserializeObject &literal:token &enumerator:e.
            
            token := $helper $readNext &enumerator:e.
            token =>
                "," ? [ token := $helper $readNext &enumerator:e. ]
                "]" ? [ eof := true. ]
                ! [ #throw JsonException new. ].
        ].
        
        ^ target.
    ]
    
    #method deserializeObject &enumerator:e
        <= deserializeObject &literal:($helper $readNext &enumerator:e literal) &enumerator:e.
        
    #method deserializeObject &literal:token &enumerator:e
    [
        #var(type:char)ch.
        token read &int:0 &vchar:ch.
        
        ch =>
            #34 ? [  // "
                ^ $helper $getQuote:token.
            ] 
            #91 ? [ // [
                ^ $helper deserializeArray:(ArrayList new) &enumerator:e array.
            ]
            #123 ? [ //{
                ^ $helper deserializeStruct:(DynamicStruct new) &enumerator:e.
            ]
            ! [
                (ch is &digit) 
                    ? [ ^ token toInt. ].
                    
                #throw JsonException new.                    
            ].        
    ]
    
    $readNext &enumerator:e
    [
        (e next)
            ! [ #throw JsonException new. ].

        ^ e get.        
    ]
    
    $getIdentifier:field
    [
        #var(type:int)len := field length.
        #var(type:char)fch.
        #var(type:char)lch.
        field read &int:0 &vchar:fch.
        field read &int:(len - 1) &vchar:lch.
        
        ((lch == #34)&&(fch == #34))
            ? [ ^ field Substring &int:1 &int:(len - 2). ]
            ! [ #throw JsonException new. ].
    ]
    
    $getQuote:field
    [
        #var(type:int)len := field length.
        #var(type:char)fch.
        #var(type:char)lch.
        field read &int:0 &vchar:fch.
        field read &int:(len - 1) &vchar:lch.
        
        ((lch == #34)&&(fch == #34))
            ? [ ^ field Substring &int:1 &int:(len - 2). ]
            ! [ #throw JsonException new. ].
    ]
}.

#class(extension)jsonOp
{
    #method toJson
        = TextBuffer new jsonSerialize:self literal.
        
    #method fromJson
        = $helper deserializeObject &enumerator:(TokenEnumerator new &literal:(self literal) &token_dfa:(ExpressionStatemachine new)).
}        
