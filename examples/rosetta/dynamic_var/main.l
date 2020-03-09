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
    
    function()
    {
        auto prop := new MessageName(console.write:"Enter the variable name:".readLine());
        (prop.setPropertyMessage())(theVariables,42);
        
        console.printLine(prop.toPrintable(),"=",(prop.getPropertyMessage())(theVariables)).readChar()     
    }
}

public program = new TestClass();
