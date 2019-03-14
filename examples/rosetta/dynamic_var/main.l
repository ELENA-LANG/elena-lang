// Create a variable with a user-defined name. The variable name should not be written in the program text, 
// but should be taken from the user dynamically. 

import system'dynamic;
import extensions;

class TestClass
{
    object theVariables;

    constructor()
    {
        theVariables := new DynamicStruct()
    }
    
    closure()
    {
        auto varRef := new MessageName(console.write:"Enter the variable name:".readLine());
        mixin varRef(theVariables).prop := 42;
        
        console.printLine(varRef.Printable,"=",mixin varRef(theVariables).get).readChar()     
    }
}

public program = new TestClass();
