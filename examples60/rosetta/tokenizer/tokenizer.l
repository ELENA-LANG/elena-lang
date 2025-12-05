import system'routines;
import extensions;
 
public Program()
{
    auto string := "Hello,How,Are,You,Today";
 
    string.splitBy(",").forEach::(s)
    {
        Console.print(s,".")
    }
}