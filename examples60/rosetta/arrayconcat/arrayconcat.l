/// concatenating two arrays in your language. 
import extensions;

public program()
{
    var a := new int[]{1,2,3};
    var b := new int[]{4,5};

    Console.printLine(
        "(",a.asEnumerable(),") + (",b.asEnumerable(),
        ") = (",(a + b).asEnumerable(),")").readChar();
}