import extensions;
import system'dynamic;
import system'dynamic'expressions;

public singleton MyDynamicallyLoadedClass
{
    doSomething()
    {
        console.writeLine("did something")
    }
}

symbol used : preloaded = new object[]{MyDynamicallyLoadedClass, mssg doSomething};
    
treeEvaluating()
{
    var loadedMessage := new Message("doSomething[1]");
    
    var c := Expression.MessageCall(
                            loadedMessage, 
                            Expression.Symbol("sandbox'MyDynamicallyLoadedClass"));
                            
    c.eval()                                
}    

public program()
{
    treeEvaluating()
}
