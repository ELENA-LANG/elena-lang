import system'collections;
import system'text;
import system'dynamic;
import system'routines;
import extensions;
import extensions'text;

// --- IOException ---

class JsonException : Exception
{
    static raise()
        = JsonException.new().raise();
    
    constructor new()
        <= new("Json Deserializer error");
}

////class jsonDispatcher =
////{
////    eval:output &array:array
////    [
////        output::jsonSerializer jsonSerialize &enumerable:array.
////    ]
////    
////    eval:output &enumerable:list
////    [
////        output::jsonSerializer jsonSerialize &enumerable:list.
////    ]
////    
////    eval:output &dto:object
////    [
////        output::jsonSerializer jsonSerialize &dto:object.
////    ]
////    
////    eval:output &struct:object
////    [
////        output::jsonSerializer jsonSerialize &struct:object.
////    ]
////    
////    ! output : object
////        = output writeLiteral:"""":(object literal):"""".
////}.

extension jsonSerializer : TextBuilder
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

//    serialize(Enumerator en)
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
//            [ self setLength(outputLen - 1) ].
//        
//        self write:"]".
//    ]    
    
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

    serialize(int n)
    {
        self.write(n.Printable)
    }
    
    serialize(String s)
    {
        self.write:"""";
        self.write(s);
        self.write:""""
    }
    
    serialize(DTO object)
    {
        self.write:"{";

        auto props := object.__dto_properties();
//        if (nil == props)
//        [
//            props := object __properties.
//        ].
        
        int len := props.Length;
        for (int i := 0, i < len, i += 1)
        {
            auto prop := props[i];
            var val := mixin prop(object).get;
            
            if (nil != val)
            {
                self.print("""",prop.Printable,""":");
                
                self.serialize(val);
                
                self.write:","
            }
        };
                                                
        int outputLen := self.Length;
        if(outputLen > 1)
        {
            outputLen -= 1;
            self.Length := outputLen
        };
        
        self.write:"}"
    }    

//    serialize(Array array)
//    [
//        self~jsonSerializer serialize(T<Enumerator>(array)).
//    ]
//
////    $serialize(BaseEnumerator o)
////    [
////        self~jsonSerializer $serialize enumerator:o.
////    ]
////
////    $serialize(BaseIndexer o)
////    [
////        self~jsonSerializer $serialize indexer:o.
////    ]
//        
////    eval:output &dto:object
////    [
////        output::jsonSerializer jsonSerialize &dto:object.
////    ]
//
//    jsonSerialize : object
//    [
//        if(nil != object)
//        [
//            self~jsonSerializer serialize(object).
//        ];
//        [ self write:"null" ].
//
//        ^ self
//    ]
}

singleton helper
{
    deserialize(DynamicStruct target, Enumerator<string> e)
    {
        string token := helper.readNext(e);
        bool    eof := false;
        until (eof)
        {
            string field := helper.getIdentifier(token);
            
            ifnot(helper.readNext(e) == ":")
                { JsonException.new().raise() };
            
            var object := helper.deserializeObject(e);
            
            MessageName prop := new MessageName(field);
            Message setProp := prop.setPropertyMessage();
            
            setProp(target, object);
            
            token := helper.readNext(e);
            token =>
                "," { token := helper.readNext(e) }
                "}" { eof := true }
                : { JsonException.new().raise() }
        };
        
        ^ target
    }
    
    deserialize(ArrayList target, Enumerator<string> e)
    {
        string token := helper.readNext(e);
        bool    eof := false;
        until (eof)
        {
            target.append(helper.deserializeObject(token, e));
            
            token := helper.readNext(e);
            token =>
                "," { token := helper.readNext(e) }
                "]" { eof := true }
                : { JsonException.new().raise() }
        };
        
        ^ target
    }
    
    deserializeObject(Enumerator<string> e)
        <= deserializeObject(helper.readNext(e), e);
        
    deserializeObject(string token, Enumerator<string> e)
    {
        char ch := $0;
        token.read(0, ref ch);
        
        if(ch == $34) // "
        { 
            ^ helper.getQuote(token) 
        }
        else if(ch == $91) // [
        { 
            ^ helper.deserialize(new ArrayList(), e).Value 
        }
        else if (ch == $123) //{
        {
            ^ helper.deserialize(new DynamicStruct(), e)
        }
        else if (ch.isDigit())
        {
            ^ token.toInt()
        }
        else
        {
            JsonException.raise()
        }
    }
    
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
    
    string readNext(Enumerator<string> e)
    {
        ifnot(e.next())
            { JsonException.raise() };

        ^ e.get()        
    }
    
    string getIdentifier(string field)
    {
        int  len := field.Length;
        char fch := $0;
        char lch := $0;
        field.read(0, ref fch);
        field.read(len - 1, ref lch);
                
        ifnot(lch == $34 && fch == $34)
            { JsonException.raise() };
            
        ^ field.Substring(1, len - 2)
    }
    
    string getQuote(string field)
    {
        int  len := field.Length;
        char fch := $0;
        char lch := $0;
        field.read(0, ref fch);
        field.read(len - 1, ref lch);
        
        ifnot(lch == $34 && fch == $34)
            { JsonException.raise() };
            
        ^ field.Substring(1, len - 2)
    }
}

public extension jsonOp
{
    string toJson()
        = new TextBuilder().serialize(self);
        
    fromJson()
        = helper.deserializeObject(new TokenEnumerator(self, new ExpressionStatemachine()));

////    readJson : object
////        = $helper deserializeStruct &object:object &enumerator:(TokenEnumerator new &literal:(self literal) &token_dfa:(ExpressionStatemachine new)).
}        
