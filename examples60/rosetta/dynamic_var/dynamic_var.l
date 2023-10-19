// Create a variable with a user-defined name. 
// The variable name should not be written in the program text, 
// but should be taken from the user dynamically. 

import system'dynamic;
import extensions;

class TestClass
{
    object variables;

    constructor()
    {
        variables := new DynamicStruct()
    }
    
    function()
    {
        auto prop := new MessageName(console.write:"Enter the variable name:".readLine());
        (prop.setPropertyMessage())(variables,42);
        
        console.printLine(prop.toPrintable(),"=",(prop.getPropertyMessage())(variables)).readChar()     
    }
}

public program = new TestClass();
