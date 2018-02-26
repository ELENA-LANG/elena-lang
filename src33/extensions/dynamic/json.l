import system'collections.
import system'text.
import system'dynamic.
//import system'routines.
import extensions.
import extensions'text.

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
//    $serialize byte:n
//    [
//        self write(n literal).
//    ]
//
//    $serialize short:n
//    [
//        self write(n literal).
//    ]
//
//    $serialize long:n
//    [
//        self write(n literal).
//    ]
//
//    $serialize real:n
//    [
//        self write(n literal).
//    ]
//
//    $serialize bool:b
//    [
//        self write(b literal).
//    ]
//
//    $serialize char:b
//    [
//        self write:"'"; write(b literal); write:"'".
//    ]
//
//    $serialize wide:s
//    [
//        self write:""""; write(s literal); write:"""".
//    ]
//
//    $serialize enumerator:en
//    [
//        self write:"[".
//
//        en runFor:self eachPair(:output:object)
//        [
//            output~jsonSerializer jsonSerialize:object.
//            
//            output write:",".
//        ].
//        
//        int outputLen := self length.
//        if(outputLen > 1)
//            [ self setLength int(outputLen - 1) ].
//        
//        self write:"]".
//    ]    
//    
//    $serialize indexer:it
//    [
//        self write:"[".
//        
//        while (it available)
//        [
//            self~jsonSerializer jsonSerialize(it get).
//            self write:",".
//            
//            it append index:1
//        ].
//        
//        int outputLen := self length.
//        if(outputLen > 1)
//            [ self setLength int(outputLen - 1) ].
//        
//        self write:"]".
//    ]    
//    
//    $serialize tape_structure:struct
//    [
//        self write:"{".
//
//        int i := 0.
//        int len := struct fieldCount.
//        while (i < len)
//        [
//            (0 == i) 
//                ! [ self write:"," ].
//            
//            var prop := struct getField int:i.
//            self write:"""".
//            self write:(prop literal).
//            self write:""":".
//            
//            var member := struct getField(i + 1).
//            
//            self~jsonSerializer jsonSerialize:member.
//            
//            i := i + 2.
//        ].
//                                
//        self write:"}"
//    ]

    $serialize(IntNumber n)
    [
        self write(n literal)
    ]
    
    $serialize(LiteralValue s)
    [
        self write:"""".
        self write(s).
        self write:"""".
    ]
    
    multi $serialize:object
    [
        self write:"{".

        array props := (object class_ref)~__prober __properties.
        if ($nil == props)
        [
            props := object __properties.
        ].
        
        int i := 0.
        int len := props length.
        while (i < len)
        [
            var prop := props[i].
            var val := object~prop get.
            
            if ($nil != val)
            [
                self print("""",prop literal,""":").
                
                self $serialize(val).
                
                self write:","
            ].            

            i += 1
        ].
                                                
        int outputLen := self length.
        if(outputLen > 1)
            [ self setLength int(outputLen - 1) ].
        
        self write:"}"
    ]    

//    $serialize array:array
//    [
//        self~jsonSerializer $serialize enumerator:array.
//    ]
//
//    $serialize(BaseEnumerator o)
//    [
//        self~jsonSerializer $serialize enumerator:o.
//    ]
//
//    $serialize(BaseIndexer o)
//    [
//        self~jsonSerializer $serialize indexer:o.
//    ]
//        
////    eval:output &dto:object
////    [
////        output::jsonSerializer jsonSerialize &dto:object.
////    ]
//
//    jsonSerialize : object
//    [
//        if($nil != object)
//        [
//            self~jsonSerializer $serialize:object.
//        ];
//        [ self write:"null" ].
//
//        ^ self
//    ]
}

class $helper =
{
    deserialize tape_dynamic_structure:target enumerator:e
    [
        literal token := $helper $readNext enumerator:e.
        bool    eof := false.
        until (eof)
        [
            literal field := $helper $getIdentifier literal:token.
            
            ifnot($helper $readNext enumerator:e == ":")
                [ JsonException new; raise ].
            
            var object := $helper deserializeObject enumerator:e.
            
            target~(Signature new literal:field) set:object.
            
            token := $helper $readNext enumerator:e; literal.
            token =>
                "," [ token := $helper $readNext enumerator:e; literal ];
                "}" [ eof := true. ];
                ! [ JsonException new; raise. ].
        ].
        
        ^ target.
    ]
    
    deserialize array_list:target enumerator:e
    [
        literal token := $helper $readNext enumerator:e.
        bool    eof := false.
        until (eof)
        [
            target append($helper deserializeObject literal:token enumerator:e).
            
            token := $helper $readNext enumerator:e; literal.
            token =>
                "," [ token := $helper $readNext enumerator:e; literal. ];
                "]" [ eof := true. ];
                ! [ JsonException new; raise. ].
        ].
        
        ^ target.
    ]
    
    deserializeObject enumerator:e
        <= deserializeObject literal($helper $readNext enumerator:e) enumerator:e.
        
    deserializeObject literal:token enumerator:e
    [
        char ch := $0.
        token read int:0 vchar:ch.
        
        if(ch == $34) // "
            [ ^ $helper $getQuote literal:token ];
            [
                if(ch == $91) // [
                    [ ^ $helper deserialize array_list(ArrayList new) enumerator:e; array ];
                    [
                        if (ch == $123) //{
                            [ ^ $helper deserialize tape_dynamic_structure(DynamicStruct new) enumerator:e ];
                            [
                                if(ch isDigit) 
                                    [ ^ token toInt ].
                                    
                                JsonException new; raise.
                            ]
                    ]
            ].
    ]
    
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
    
    $readNext enumerator:e
    [
        ifnot(e next)
            [ JsonException new; raise ].

        ^ e get.        
    ]
    
    literal $getIdentifier literal:field
    [
        int  len := field length.
        char fch := $0.
        char lch := $0.
        field read int:0 vchar:fch.
        field read int(len - 1) vchar:lch.
        
        ifnot((lch == $34)&&(fch == $34))
            [ JsonException new; raise ].
            
        ^ field Substring int:1 int(len - 2)
    ]
    
    literal $getQuote literal:field
    [
        int  len := field length.
        char fch := $0.
        char lch := $0.
        field read int:0 vchar:fch.
        field read int(len - 1) vchar:lch.
        
        ifnot((lch == $34)&&(fch == $34))
            [ JsonException new; raise ].
            
        ^ field Substring int:1 int(len - 2).
    ]
}.

extension jsonOp
{
    toJson
        = TextBuilder new; $serialize:self; literal.
        
    fromJson
        = $helper deserializeObject enumerator(TokenEnumerator new literal:self token_dfa(ExpressionStatemachine new)).

//    readJson : object
//        = $helper deserializeStruct &object:object &enumerator:(TokenEnumerator new &literal:(self literal) &token_dfa:(ExpressionStatemachine new)).
}        
