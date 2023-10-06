import extensions;
import extensions'text;
import system'dynamic;
import system'collections;

// --- IOException ---

class JsonException : Exception
{
   constructor new()
      <= super new("Json Deserializer error");
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
      char ch := token[0];
        
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

      ^ *e        
   }
    
   string getIdentifier(string field)
   {
      int  len := field.Length;
      char fch := field[0];
      char lch := field[len - 1];
                
      ifnot(lch == $34 && fch == $34)
         { JsonException.raise() };
            
      ^ field.Substring(1, len - 2)
   }
    
   string getQuote(string field)
   {
      int  len := field.Length;
      char fch := field[0];
      char lch := field[len - 1];
        
      ifnot(lch == $34 && fch == $34)
         { JsonException.raise() };
            
      ^ field.Substring(1, len - 2)
   }
}

public extension jsonOp
{
   fromJson()
      = helper.deserializeObject(new TokenEnumerator(self, new ExpressionStatemachine()));
   
}