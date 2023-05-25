import extensions;
import extensions'text;
import system'routines;

public program()
{
   var variants := new string[]{"abc","dfge","ab"}; 

   var o := variants.sort:(a,b => a.Length > b.Length );
}