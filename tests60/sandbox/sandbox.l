//import extensions;

/*public extension invokeOp : MessageName
{
    getPropertyValue(target)
    {
        Message mssg := self.getPropertyMessage();
        
        ^ mssg(target)
    
    }
}*/


/*public extension outputFormatterOp
{
   printFormatted(string s, params object[] args)
   {
   }
}*/

/*
import system'dynamic;
import system'dynamic'expressions;

symbol used : preloaded = new object[]{MyDynamicallyLoadedClass, mssg doSomething};

public singleton MyDynamicallyLoadedClass
{
    doSomething()
    {
        console.writeLine("did something")
    }
}

public program()
{
    var loadedClass := new Symbol("sandbox'MyDynamicallyLoadedClass");
    var loadedMessage := new Message("doSomething[1]");
    
    var t := DynamicTape.load(
        ConstantFunction.load(loadedClass),
        MessageFunction.load(loadedMessage));

    t()
}
*/

B;

C : B;
D : B;

singleton E
{
   load(params B[] args) {}
}

public program()
{
   E.load(new C(), new D());
}
