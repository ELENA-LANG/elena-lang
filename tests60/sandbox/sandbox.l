import extensions;

//singleton MySingleton
//{
//    static mySingletonField := "Some value";
//    
//    printMe()
//    {
//        var s := mySingletonField;
//
//        console.printLine(mySingletonField)
//    }
//}

onModuleUse : preloaded = console.printLine("Starting");

public program()
{
   console.printLine("Hello");

//    MySingleton.printMe()
}