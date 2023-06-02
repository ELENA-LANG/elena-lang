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

dynamicLoading()
{
    var loadedClass := new Symbol("sandbox'MyDynamicallyLoadedClass");
    var loadedMessage := new Message("doSomething[1]");
    
    loadedMessage(loadedClass)
}
    
//dynamicEvaluating()
//{
//    var loadedClass := new Symbol("exprtree1'MyDynamicallyLoadedClass");
//    var loadedMessage := new Message("doSomething[1]");
    
//    var t := DynamicTape.load(
//        ConstantFunction.load(loadedClass),
//        MessageFunction.load(loadedMessage));

//    t()
//}    
    
//treeEvaluating()
//{
//    var loadedMessage := new Message("doSomething[1]");
    
//    var c := Expression.MessageCall(
//                            loadedMessage, 
//                            Expression.Symbol("exprtree1'MyDynamicallyLoadedClass"));
                            
//    c.eval()                                
//}    
    
public program()
{
    dynamicLoading();
    
//    dynamicEvaluating();
    
//    treeEvaluating()
}
