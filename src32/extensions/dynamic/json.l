//import system'collections.
import system'text.
import system'dynamic.
import system'routines.
//import extensions.
//import extensions'text.

// --- IOException ---

class JsonException :: Exception
{
    constructor new
        <= new : "Json Deserializer error".
}

//class jsonDispatcher =
//{
//    eval:output &array:array
//    [
//        output::jsonSerializer jsonSerialize &enumerable:array.
//    ]
//    
//    eval:output &enumerable:list
//    [
//        output::jsonSerializer jsonSerialize &enumerable:list.
//    ]
//    
//    eval:output &dto:object
//    [
//        output::jsonSerializer jsonSerialize &dto:object.
//    ]
//    
//    eval:output &struct:object
//    [
//        output::jsonSerializer jsonSerialize &struct:object.
//    ]
//    
//    ! output : object
//        = output writeLiteral:"""":(object literal):"""".
//}.

textbuffer extension jsonSerializer
{
    $serialize byte:n
    [
        self write(n literal).
    ]

    $serialize short:n
    [
        self write(n literal).
    ]

    $serialize int:n
    [
        self write(n literal).
    ]

    $serialize long:n
    [
        self write(n literal).
    ]

    $serialize real:n
    [
        self write(n literal).
    ]

    $serialize bool:b
    [
        self write(b literal).
    ]

    $serialize char:b
    [
        self write:"'"; write(b literal); write:"'".
    ]

    $serialize literal:s
    [
        self write:""""; write:s; write:"""".
    ]

    $serialize wide:s
    [
        self write:""""; write(s literal); write:"""".
    ]

    $serialize enumerator:en
    [
        self write:"[".

        en runFor:self eachPair(:output:object)
        [
            output~jsonSerializer jsonSerialize:object.
            
            output write:",".
        ].
        
        int outputLen := self length.
        if(outputLen > 1)
            [ self setLength int(outputLen - 1) ].
        
        self write:"]".
    ]    
    
    $serialize indexer:it
    [
        self write:"[".
        
        while (it available)
        [
            self~jsonSerializer jsonSerialize(it get).
            self write:",".
            
            it append index:1
        ].
        
        int outputLen := self length.
        if(outputLen > 1)
            [ self setLength int(outputLen - 1) ].
        
        self write:"]".
    ]    
    
    $serialize tape_structure:struct
    [
        self write:"{".

        int i := 0.
        int len := struct fieldCount.
        while (i < len)
        [
            (0 == i) 
                ! [ self write:"," ].
            
            var prop := struct getField int:i.
            self write:"""".
            self write:(prop literal).
            self write:""":".
            
            var member := struct getField(i + 1).
            
            self~jsonSerializer jsonSerialize:member.
            
            i := i + 2.
        ].
                                
        self write:"}"
    ]

    $serialize dto:object
    [
        self write:"{".
                
        mssg(10) properties := 0.
        int      len := 10.
        int      offs := 0.

//        object readMatchedEntries &mssg:%"get[0]" &int:0 &mssgarray:properties &vint:len.
//        while (len > 0)
//        [
//            int i:=0.
//            while (i < len)
//            [
//                mssg message := properties@i.
//                
//                self writeLiteral:"""":(message subject literal):""":".
//                
//                self::jsonSerializer jsonSerialize:(object::message eval).
//                
//                self write:",".
//                
//                i := i + 1.
//            ].
//            
//            offs += len.
//            
//            object readMatchedEntries &mssg:%"get[0]" &int:offs &mssgarray:properties &vint:len.
//        ].
        
        int outputLen := self length.
        if(outputLen > 1)
            [ self setLength int(outputLen - 1) ].
        
        self write:"}"
    ]    
        
    $serialize array:array
    [
        self~jsonSerializer $serialize enumerator:array.
    ]

    $serialize(BaseEnumerator o)
    [
        self~jsonSerializer $serialize enumerator:o.
    ]

    $serialize(BaseIndexer o)
    [
        self~jsonSerializer $serialize indexer:o.
    ]
        
//    eval:output &dto:object
//    [
//        output::jsonSerializer jsonSerialize &dto:object.
//    ]

    jsonSerialize : object
    [
        if($nil != object)
        [
            self~jsonSerializer $serialize:object.
        ];
        [ self write:"null" ].

        ^ self
    ]
}

//class $helper =
//{
//    deserializeStruct:target &enumerator:e
//    [
//        literal token := $helper $readNext &enumerator:e literal.
//        bool    eof := false.
//        until (eof)
//        [
//            literal field := $helper $getIdentifier &literal:token.
//            
//            ($helper $readNext &enumerator:e == ":")
//                ! [ JsonException new raise. ].
//            
//            var object := $helper deserializeObject &enumerator:e.
//            
//            target::(Signature new &literal:field) set:object.
//            
//            token := $helper $readNext &enumerator:e literal.
//            token =>
//                "," [ token := $helper $readNext &enumerator:e literal. ];
//                "}" [ eof := true. ];
//                ! [ JsonException new raise. ].
//        ].
//        
//        ^ target.
//    ]
//    
//    deserializeArray:target &enumerator:e
//    [
//        literal token := $helper $readNext &enumerator:e literal.
//        bool    eof := false.
//        until (eof)
//        [
//            target += $helper deserializeObject &literal:token &enumerator:e.
//            
//            token := $helper $readNext &enumerator:e literal.
//            token =>
//                "," [ token := $helper $readNext &enumerator:e literal. ];
//                "]" [ eof := true. ];
//                ! [ JsonException new raise. ].
//        ].
//        
//        ^ target.
//    ]
//    
//    deserializeObject &enumerator:e
//        <= deserializeObject &literal:($helper $readNext &enumerator:e literal) &enumerator:e.
//        
//    deserializeObject &literal:token &enumerator:e
//    [
//        char ch := #0.
//        token read &int:0 &vchar:ch.
//        
//        (ch == #34) // "
//            ? [ ^ $helper $getQuote &literal:token. ]
//            ! [
//                (ch == #91) // [
//                    ? [ ^ $helper deserializeArray:(ArrayList new) &enumerator:e array. ]
//                    ! [
//                        (ch == #123) //{
//                            ? [ ^ $helper deserializeStruct:(DynamicStruct new) &enumerator:e. ]
//                            ! [
//                                (ch is &digit) 
//                                    ? [ ^ token toInt. ].
//                                    
//                                JsonException new raise.
//                            ].
//                    ]
//            ].
//    ]
//    
//    deserializeStruct &object:object &enumerator:e
//        <= deserializeStruct &object:object &literal:($helper $readNext &enumerator:e literal) &enumerator:e.
//        
//    deserializeStruct &object:object &literal:token &enumerator:e
//    [
//        char ch := #0.
//        token read &int:0 &vchar:ch.
//
//        (ch == #123)
//            ? [ ^ $helper deserializeStruct:object &enumerator:e. ]
//            ! [ JsonException new raise. ].
//    ]
//    
//    $readNext &enumerator:e
//    [
//        (e next)
//            ! [ JsonException new raise. ].
//
//        ^ e get.        
//    ]
//    
//    literal $getIdentifier &literal:field
//    [
//        int  len := field length.
//        char fch := #0.
//        char lch := #0.
//        field read &int:0 &vchar:fch.
//        field read &int:(len - 1) &vchar:lch.
//        
//        ((lch == #34)&&(fch == #34))
//            ! [ JsonException new raise. ].
//            
//        ^ field Substring &int:1 &int:(len - 2).            
//    ]
//    
//    literal $getQuote &literal:field
//    [
//        int  len := field length.
//        char fch := #0.
//        char lch := #0.
//        field read &int:0 &vchar:fch.
//        field read &int:(len - 1) &vchar:lch.
//        
//        ((lch == #34)&&(fch == #34))
//            ! [ JsonException new raise. ].
//            
//        ^ field Substring &int:1 &int:(len - 2).
//    ]
//}.

extension jsonOp
{
    toJson
        = TextBuilder new; jsonSerialize:self; literal.
        
//    fromJson
//        = $helper deserializeObject &enumerator:(TokenEnumerator new &literal:(self literal) &token_dfa:(ExpressionStatemachine new)).
//        
//    readJson : object
//        = $helper deserializeStruct &object:object &enumerator:(TokenEnumerator new &literal:(self literal) &token_dfa:(ExpressionStatemachine new)).
}        
