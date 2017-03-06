// Create a variable with a user-defined name. The variable name should not be written in the program text, 
// but should be taken from the user dynamically. 

#import system'dynamic.
#import extensions.

class TestClass
{
    object theVariables.

    constructor new
    [
        theVariables := DynamicStruct new.
    ]
    
    eval
    [
        subject varRef := Signature new &literal:(console write:"Enter the variable name:" readLine).
        theVariables::varRef set:42.
        
        var v := theVariables::varRef get.
        
        console writeLine:(varRef literal):"=":(theVariables::varRef get) readChar.
    ]
}

program = TestClass new.
