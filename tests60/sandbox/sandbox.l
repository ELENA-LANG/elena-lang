import extensions;

singleton Ops
{
    test(params object[] args)
    {
       self.boxVArg(params args);
    }

    boxVArg(params object[] args)
    {
//        // an variadic argument is boxed into dynamic array
        var o := args;
        
        // print a boxed variadic argument as a collection 
        console.printLine(o.asEnumerable())
    }
}

public program()
{
    //Ops.boxVArg(1,2.0r,"abc");
   Ops.test(1,2.0r,"abc");
}