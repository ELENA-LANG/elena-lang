// Loop over multiple arrays (or lists or tuples or whatever they're called in your language) and display the <i>th element of each. 

import system'routines;
import extensions;

public program()
{  
    var a1 := new string[]{"a","b","c"};
    var a2 := new string[]{"A","B","C"};
    var a3 := new int[]{1,2,3};
    
    for(int i := 0, i < a1.Length, i += 1)
    {
        console.printLine(a1[i], a2[i], a3[i])
    };
    
    console.readChar()
}

// To build the program to use 'programUsingZip' symbol, just change
// the forward to use programUsingZip as entry point
// --
// program=loopma'programUsingZip
// --
public programUsingZip()
{
    var a1 := new string[]{"a","b","c"};
    var a2 := new string[]{"A","B","C"};
    var a3 := new int[]{1,2,3};
    var zipped := a1.zipBy(a2,(first,second => first + second.toString() ))
                       .zipBy(a3, (first,second => first + second.toString() ));
    
    zipped.forEach:(e)
        { console.writeLine:e };
}