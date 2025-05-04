import extensions;
 
public program()
{
    var s := "abcd";
 
    Console.printLine(s," starts with ab: ",s.startingWith("ab"));
    Console.printLine(s," starts with cd: ",s.startingWith("cd"));
 
    Console.printLine(s," ends with ab: ",s.endingWith("ab"));
    Console.printLine(s," ends with cd: ",s.endingWith("cd"));
 
    Console.printLine(s," contains ab: ",s.containing("ab"));
    Console.printLine(s," contains bc: ",s.containing("bc"));
    Console.printLine(s," contains cd: ",s.containing("cd"));
    Console.printLine(s," contains az: ",s.containing("az"));
 
    Console.printLine(s," index of az: ",s.indexOf(0, "az"));
    Console.printLine(s," index of cd: ",s.indexOf(0, "cd"));
    Console.printLine(s," index of bc: ",s.indexOf(0, "bc"));
    Console.printLine(s," index of ab: ",s.indexOf(0, "ab"));
 
    Console.readChar()
}