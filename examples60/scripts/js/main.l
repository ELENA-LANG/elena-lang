import extensions;
import extensions'scripting;

//public live_program()
//{
//   if (program_arguments.Length == 1)
//        { Console.printLine("Please provide the path to the file to interpret"); AbortException.raise() };
//
//    try
//    {
//        var o := jscript.evalPath(program_arguments[1]).program;
//        
//        o.main();
//    }
//    catch(Exception e)
//    {
//        Console.printLine:e
//    }
//}

public stat_program()
{
   forward jsprogram.main()
}