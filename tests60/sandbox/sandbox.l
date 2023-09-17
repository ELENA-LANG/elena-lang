import system'routines;
import extensions;

public program()
{
   var src := "Hello";
   var dst := src;          //  copying the reference
   var copy := src.clone(); //  copying the content
}
