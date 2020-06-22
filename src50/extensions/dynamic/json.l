import system'collections;
import system'text;
import system'dynamic;
import system'routines;
import extensions;
import extensions'text;

// --- IOException ---

class JsonException : Exception
{
    constructor new()
        <= new("Json Deserializer error");
}

extension jsonSerializer : TextBuilder
{
    serialize(Enumerator en)
    {
        self.write:"[";

        en.forEachPair(self,(output,object) {
            output.jsonSerialize(object);
            
            output.write:","
        });
        
        int outputLen := self.Length;
        if(outputLen > 1)
            { self.setLength(outputLen - 1) };
        
        self.write:"]";
    }    
    
    serialize(Indexer it)
    {
        self.write:"[";
        
        while (it.Available)
        {
            self.jsonSerialize(it.get());
            self.write:",";
            
            it.appendIndex:1
        };
        
        int outputLen := self.Length;
        if(outputLen > 1)
            { self.setLength(outputLen - 1) };
        
        self.write:"]"
    }    
    
    serialize(Struct struct)
    {
        self.write:"{";

        int i := 0;
        int len := struct.__FieldCount;
        while (i < len)
        {
            ifnot(0 == i) 
                { self.write:"," };
            
            var prop := struct.getField(i);
            self.write:"""";
            self.write:(prop.toPrintable());
            self.write:""":";
            
            var member := struct.getField(i + 1);
            
            self.serialize(member);
            
            i := i + 2
        };
                                
        self.write:"}"
    }

    serialize(int n)
    {
        self.write(n.toPrintable())
    }

    serialize(byte n)
    {
        self.write(n.toPrintable())
    }

    serialize(short n)
    {
        self.write(n.toPrintable())
    }

    serialize(long n)
    {
        self.write(n.toPrintable())
    }

    serialize(real n)
    {
        self.write(n.toPrintable())
    }
        
    serialize(String s)
    {
        self.write:"""";
        self.write(s);
        self.write:""""
    }

    serialize(bool b)
    {
        self.write(b.toPrintable())
    }

    serialize(char b)
    {
        self.write:"'".write(b.toPrintable()).write:"'"
    }

    serialize(wide s)
    {
        self.write:"""".write(cast string(s)).write:""""
    }
        
    serialize(DTO object)
    {
        self.write:"{";

        auto props := object.__getClass().__dto_properties();
//        if (nil == props)
//        [
//            props := object __properties.
//        ].
        
        int len := props.Length;
        for (int i := 0, i < len, i += 1)
        {
            auto prop := props[i];
            var val := prop.getPropertyValue(object);
            
            if (nil != val)
            {
                self.print("""",prop.toPrintable(),""":");
                
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

    serialize(Array array)
    {
        self.serialize(array.enumerator())
    }
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
}        
