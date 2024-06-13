import extensions;
import extensions'dynamic;
import system'dynamic;

public class CommandDTO
{
    int Command  : prop;
    
    string Value : prop;
    
    string User  : prop;
}

public program()
{
   var s := "{""Command"":1,""Value"":""a""}";

   var dto := class Serializer<CommandDTO>.fromJson(s);

   var commmand := dto.Command;
   var value := dto.Value;
}
