#import system.
#import system'collections.
#import system'text.
#import system'dynamic.
//#import system'routines.
#import extensions.
#import extensions'text.

// --- IOException ---

#class(exceptionof:JSONError) JsonException
{
    #constructor new
        <= %new : "Json Deserializer error".
        
    #constructor new : aMessage
    [
        theMessage := aMessage literal.
        theCallStack := CallStack new.
    ]
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
    
    eval:output &struct:object
    [
        output::jsonSerializer jsonSerialize &struct:object.
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
    #method jsonSerialize &struct:object
    [
        #var(textbuffer)output := self textbuffer.
        
        output write:"{".

        0 till:(object fieldCount) &by:2 &doEach:i
        [
            (0 == i) 
                ! [ output write:"," ].
            
            #var prop := object getFieldAt:(i + 1).
            output write:"""".
            output write:(prop literal).
            output write:""":".
            
            #var member := object getFieldAt:(i + 1).
            
            member cast:%eval &to:jsonDispatcher &with:output.
        ].
                                
        output write:"}".
        
        ^ self.
    ]
    
    #method jsonSerialize &dto:object
    [
        #var(textbuffer)output := self textbuffer.
        
        output write:"{".
                
        #var(mssg:10) properties.
        #var(int)len := 10.
        #var(int)offs := 0.

        object readMatchedEntries &mssg:%"get[0]" &int:0 &mssgarray:properties &vint:len.
        #loop (len > 0) ?
        [
            #var(int)i:=0.
            #loop (i < len)?
            [
                #var(mssg) message := properties@i.
                
                output writeLiteral:"""":(message subject name):""":".
                
                output::jsonSerializer jsonSerialize:(object::message eval).
                
                output write:",".
                
                i := i + 1.
            ].
            
            offs := offs + len.
            
            object readMatchedEntries &mssg:%"get[0]" &int:offs &mssgarray:properties &vint:len.
        ].
        
        #var(int)outputLen := output length.
        (outputLen > 1)
            ? [ output setLength &int:(outputLen - 1). ].
        
        output write:"}".
        
        ^ self.
    ]    
    
    #method jsonSerialize &enumerable:object
    [
        #var(textbuffer)output := self textbuffer.
        
        output write:"[".

        object run:output &eachPair:(:output:object)
        [
            output::jsonSerializer jsonSerialize:object.
            
            output write:",".
        ].
        
        #var(int)outputLen := output length.
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
        #var(literal)token := $helper $readNext &enumerator:e.
        #var(bool)eof := false.
        #loop (eof)!
        [
            #var(literal)field := $helper $getIdentifier:token.
            
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
        #var(literal)token := $helper $readNext &enumerator:e.
        #var(bool)eof := false.
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
        #var(char)ch.
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
    
    #method deserializeStruct &object:object &enumerator:e
        <= deserializeStruct &object:object &literal:($helper $readNext &enumerator:e literal) &enumerator:e.
        
    #method deserializeStruct &object:object &literal:token &enumerator:e
    [
        #var(char)ch.
        token read &int:0 &vchar:ch.

        (ch == #123)
            ? [ ^ $helper deserializeStruct:object &enumerator:e. ]
            ! [ #throw JsonException new. ].
    ]
    
    $readNext &enumerator:e
    [
        (e next)
            ! [ #throw JsonException new. ].

        ^ e get.        
    ]
    
    $getIdentifier:field
    [
        #var(int)len := field length.
        #var(char)fch.
        #var(char)lch.
        field read &int:0 &vchar:fch.
        field read &int:(len - 1) &vchar:lch.
        
        ((lch == #34)&&(fch == #34))
            ? [ ^ field Substring &int:1 &int:(len - 2). ]
            ! [ #throw JsonException new. ].
    ]
    
    $getQuote:field
    [
        #var(int)len := field length.
        #var(char)fch.
        #var(char)lch.
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
        = TextBuilder new jsonSerialize:self literal.
        
    #method fromJson
        = $helper deserializeObject &enumerator:(TokenEnumerator new &literal:(self literal) &token_dfa:(ExpressionStatemachine new)).
        
    #method readJson : object
        = $helper deserializeStruct &object:object &enumerator:(TokenEnumerator new &literal:(self literal) &token_dfa:(ExpressionStatemachine new)).
}        
