import extensions'text;

public program()
{
    var s := StringWriter.load("Hello");
    s.append(" World");
    
    console.writeLine(s).readChar()
}