import system'routines;
import system'collections;
import extensions;
//import system'text;

//public program()
//{
//    console.printLine(console.readLine()
//                              .split()
//                              .selectBy(mssgconst toInt<convertorOp>[1])
//                              .summarize())
//}

singleton tester
{
   load(params object[] args)
   {
      string line := "1 2";

      Array params := line.splitBy($32);

      int len := args.Length;
      for(int i := 0, i < len, i := i + 1)
      {
         params[i].convertTo(args[i]);
      };

      ^ self
   }
}

public program()
{
   var A := Integer.new();
   var B := Integer.new();

   //tester.load(A,B);

   var r := A + B;

   //console.writeLine(r); 
   //console.printLine(A + B)

   //console.loadLine(A,B).printLine(A + B)
}