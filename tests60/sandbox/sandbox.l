//import system'io;
//import extensions;
//import extensions'routines;
//
//public program()
//{
////    if (program_arguments.Length == 1)
////        { console.writeLine:"Please provide the path to the file to view"; AbortException.raise() };
//
//    File.assign(/*program_arguments[1]*/"sandbox.l").forEachLine:printingLn;
//  
//    console.readChar() // wait for any key
//}

A
{
   Reference<int> ref := 2;
}

public program()
{
   var a := new A();
}