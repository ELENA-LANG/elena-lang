import extensions'text;

public Program()
{
    var s := StringWriter.load("Hello");
    s.append(" World");
    
    Console.writeLine(s).readChar()
}