import extensions;
//
//class MyClass
//{
//    doInit()
//    {
//        console.writeLine:"onInit fired!"
//    }
//}
//
//class MainClass
//{
//    event Func onInit; // a field with the same name is declared as well
//
//    init()
//    {
//        // call the function if it is not nil
//        onInit.?()
//    }    
//}
//
//public program()
//{
//    var class := new MyClass();
//
//    var mainClass := new MainClass();
//
//    // adding an event handler 
//    mainClass.onInit := { class.doInit() };
//    // adding the second event handler 
//    mainClass.onInit := { console.writeLine:"I'm fired too!!" }; 
//
//    mainClass.init()
//}

public program()
{
   var f := /*{ console.writeLine("Hello") }*/nil;

   f();
}
