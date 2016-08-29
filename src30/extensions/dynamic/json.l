#import system.
#import system'collections.
#import system'text.
#import system'dynamic.
#import system'routines.
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
    
    #method(w2) eval:output &array:array
    [
        output::jsonSerializer jsonSerialize &enumerable:array.
    ]
    
    #method(w2) eval:output &enumerable:list
    [
        output::jsonSerializer jsonSerialize &enumerable:list.
    ]
    
    #method(w2) eval:output &dto:object
    [
        output::jsonSerializer jsonSerialize &dto:object.
    ]
    
    #method(w2) eval:output &struct:object
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
        self write:"{".

        0 till:(object fieldCount) &by:2 &doEach:i
        [
            (0 == i) 
                ! [ self write:"," ].
            
            #var prop := object getFieldAt:(i + 1).
            self write:"""".
            self write:(prop literal).
            self write:""":".
            
            #var member := object getFieldAt:(i + 1).
            
            member cast:%eval &to:jsonDispatcher &with:self.
        ].
                                
        self write:"}".
        
        ^ self.
    ]
    
    #method jsonSerialize &dto:object
    [
        self write:"{".
                
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
                
                self writeLiteral:"""":(message subject literal):""":".
                
                self::jsonSerializer jsonSerialize:(object::message eval).
                
                self write:",".
                
                i := i + 1.
            ].
            
            offs += len.
            
            object readMatchedEntries &mssg:%"get[0]" &int:offs &mssgarray:properties &vint:len.
        ].
        
        #var(int)outputLen := self length.
        (outputLen > 1)
            ? [ self setLength &int:(outputLen - 1). ].
        
        self write:"}".
        
        ^ self.
    ]    
    
    #method jsonSerialize &enumerable:object
    [
        self write:"[".

        object run:self &eachPair:(:output:object)
        [
            self::jsonSerializer jsonSerialize:object.
            
            self write:",".
        ].
        
        #var(int)outputLen := self length.
        (outputLen > 1)
            ? [ self setLength &int:(outputLen - 1). ].
        
        self write:"]".
        
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
        #var(literal)token := $helper $readNext &enumerator:e literal.
        #var(bool)eof := false.
        #loop (eof)!
        [
            #var(literal)field := $helper $getIdentifier &literal:token.
            
            ($helper $readNext &enumerator:e == ":")
                ! [ #throw JsonException new. ].
            
            #var object := $helper deserializeObject &enumerator:e.
            
            target::(Signature new &literal:field) set:object.
            
            token := $helper $readNext &enumerator:e literal.
            token =>
                "," ? [ token := $helper $readNext &enumerator:e literal. ]
                "}" ? [ eof := true. ]
                ! [ #throw JsonException new. ].
        ].
        
        ^ target.
    ]
    
    #method deserializeArray:target &enumerator:e
    [
        #var(literal)token := $helper $readNext &enumerator:e literal.
        #var(bool)eof := false.
        #loop (eof)!
        [
            target += $helper deserializeObject &literal:token &enumerator:e.
            
            token := $helper $readNext &enumerator:e literal.
            token =>
                "," ? [ token := $helper $readNext &enumerator:e literal. ]
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
        
        (ch == #34) // "
            ? [ ^ $helper $getQuote &literal:token. ]
            ! [
                (ch == #91) // [
                    ? [ ^ $helper deserializeArray:(ArrayList new) &enumerator:e array. ]
                    ! [
                        (ch == #123) //{
                            ? [ ^ $helper deserializeStruct:(DynamicStruct new) &enumerator:e. ]
                            ! [
                                (ch is &digit) 
                                    ? [ ^ token toInt. ].
                                    
                                #throw JsonException new.
                            ].
                    ]
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
    
    #method(literal) $getIdentifier &literal:field
    [
        #var(int)len := field length.
        #var(char)fch.
        #var(char)lch.
        field read &int:0 &vchar:fch.
        field read &int:(len - 1) &vchar:lch.
        
        ((lch == #34)&&(fch == #34))
            ! [ #throw JsonException new. ].
            
        ^ field Substring &int:1 &int:(len - 2).            
    ]
    
    #method(literal) $getQuote &literal:field
    [
        #var(int)len := field length.
        #var(char)fch.
        #var(char)lch.
        field read &int:0 &vchar:fch.
        field read &int:(len - 1) &vchar:lch.
        
        ((lch == #34)&&(fch == #34))
            ! [ #throw JsonException new. ].
            
        ^ field Substring &int:1 &int:(len - 2).
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
