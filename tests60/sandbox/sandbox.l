import system'routines;
import system'collections;
import extensions;
//import system'text;

singleton Tester
{
   load(params object[] args)
   {
//      string line := "1 2";

//      Array params := line.splitBy($32);

      int len := args.Length;
      for(int i := 0, i < len, i := i + 1)
      {
         console.writeLine(args[i]);

//         params[i].convertTo(args[i]);
      };

//      ^ self
   }
}

//public program()
//{
//    console.printLine(console.readLine()
//                              .split()
//                              .selectBy(mssgconst toInt<convertorOp>[1])
//                              .summarize())
//}

public program()
{
   var A := Integer.new();
   var B := Integer.new();
 
   Tester.load(A,B);

   //console.loadLine(A,B).printLine(A + B)
}