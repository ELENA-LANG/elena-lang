import extensions;
import extensions'text;
 
public program()
{
    var s := "World";
    s := "Hello " + s;
    Console.writeLine(s);
 
    // Alternative way
    var s2 := StringWriter.load("World");
    s2.insert(0, "Hello ");
    Console.writeLine(s2);
    Console.readChar()
}