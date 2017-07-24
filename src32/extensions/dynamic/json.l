#import system.
#import system'collections.
#import system'text.
#import system'dynamic.
#import system'routines.
#import extensions.
#import extensions'text.

// --- IOException ---

exceptionof(JSONError) JsonException
{
    constructor new
        <= new : "Json Deserializer error".
        
    constructor new : aMessage
    [
        theMessage := aMessage literal.
        theCallStack := CallStack new.
    ]
}

class jsonDispatcher =
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
    
    ! output : object
        = output writeLiteral:"""":(object literal):"""".
}.

extension(textbuffer) jsonSerializer
{
    jsonSerialize &struct:object
    [
        self write:"{".

        0 till:(object fieldCount) &by:2 &doEach:i
        [
            (0 == i) 
                ! [ self write:"," ].
            
            var prop := object getFieldAt:(i + 1).
            self write:"""".
            self write:(prop literal).
            self write:""":".
            
            var member := object getFieldAt:(i + 1).
            
            member cast:%eval &to:jsonDispatcher &with:self.
        ].
                                
        self write:"}".
        
        ^ self.
    ]
    
    jsonSerialize &dto:object
    [
        self write:"{".
                
        mssg(10) properties := 0.
        int      len := 10.
        int      offs := 0.

        object readMatchedEntries &mssg:%"get[0]" &int:0 &mssgarray:properties &vint:len.
        while (len > 0)
        [
            int i:=0.
            while (i < len)
            [
                mssg message := properties@i.
                
                self writeLiteral:"""":(message subject literal):""":".
                
                self::jsonSerializer jsonSerialize:(object::message eval).
                
                self write:",".
                
                i := i + 1.
            ].
            
            offs += len.
            
            object readMatchedEntries &mssg:%"get[0]" &int:offs &mssgarray:properties &vint:len.
        ].
        
        int outputLen := self length.
        (outputLen > 1)
            ? [ self setLength &int:(outputLen - 1). ].
        
        self write:"}".
        
        ^ self.
    ]    
    
    jsonSerialize &enumerable:object
    [
        self write:"[".

        object run:self &eachPair:(:output:object)
        [
            self::jsonSerializer jsonSerialize:object.
            
            self write:",".
        ].
        
        int outputLen := self length.
        (outputLen > 1)
            ? [ self setLength &int:(outputLen - 1). ].
        
        self write:"]".
        
        ^ self.
    ]    
    
    jsonSerialize : object
    [
        ($nil != object)
            ? [
                object cast:%eval &to:jsonDispatcher &with:self.
            ]
            ! [ self write:"null". ].
            
        ^ self.            
    ]
}
class $helper =
{
    deserializeStruct:target &enumerator:e
    [
        literal token := $helper $readNext &enumerator:e literal.
        bool    eof := false.
        until (eof)
        [
            literal field := $helper $getIdentifier &literal:token.
            
            ($helper $readNext &enumerator:e == ":")
                ! [ JsonException new raise. ].
            
            var object := $helper deserializeObject &enumerator:e.
            
            target::(Signature new &literal:field) set:object.
            
            token := $helper $readNext &enumerator:e literal.
            token =>
                "," [ token := $helper $readNext &enumerator:e literal. ];
                "}" [ eof := true. ];
                ! [ JsonException new raise. ].
        ].
        
        ^ target.
    ]
    
    deserializeArray:target &enumerator:e
    [
        literal token := $helper $readNext &enumerator:e literal.
        bool    eof := false.
        until (eof)
        [
            target += $helper deserializeObject &literal:token &enumerator:e.
            
            token := $helper $readNext &enumerator:e literal.
            token =>
                "," [ token := $helper $readNext &enumerator:e literal. ];
                "]" [ eof := true. ];
                ! [ JsonException new raise. ].
        ].
        
        ^ target.
    ]
    
    deserializeObject &enumerator:e
        <= deserializeObject &literal:($helper $readNext &enumerator:e literal) &enumerator:e.
        
    deserializeObject &literal:token &enumerator:e
    [
        char ch := #0.
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
                                    
                                JsonException new raise.
                            ].
                    ]
            ].
    ]
    
    deserializeStruct &object:object &enumerator:e
        <= deserializeStruct &object:object &literal:($helper $readNext &enumerator:e literal) &enumerator:e.
        
    deserializeStruct &object:object &literal:token &enumerator:e
    [
        char ch := #0.
        token read &int:0 &vchar:ch.

        (ch == #123)
            ? [ ^ $helper deserializeStruct:object &enumerator:e. ]
            ! [ JsonException new raise. ].
    ]
    
    $readNext &enumerator:e
    [
        (e next)
            ! [ JsonException new raise. ].

        ^ e get.        
    ]
    
    literal $getIdentifier &literal:field
    [
        int  len := field length.
        char fch := #0.
        char lch := #0.
        field read &int:0 &vchar:fch.
        field read &int:(len - 1) &vchar:lch.
        
        ((lch == #34)&&(fch == #34))
            ! [ JsonException new raise. ].
            
        ^ field Substring &int:1 &int:(len - 2).            
    ]
    
    literal $getQuote &literal:field
    [
        int  len := field length.
        char fch := #0.
        char lch := #0.
        field read &int:0 &vchar:fch.
        field read &int:(len - 1) &vchar:lch.
        
        ((lch == #34)&&(fch == #34))
            ! [ JsonException new raise. ].
            
        ^ field Substring &int:1 &int:(len - 2).
    ]
}.

extension jsonOp
{
    toJson
        = TextBuilder new jsonSerialize:self literal.
        
    fromJson
        = $helper deserializeObject &enumerator:(TokenEnumerator new &literal:(self literal) &token_dfa:(ExpressionStatemachine new)).
        
    readJson : object
        = $helper deserializeStruct &object:object &enumerator:(TokenEnumerator new &literal:(self literal) &token_dfa:(ExpressionStatemachine new)).
}        
