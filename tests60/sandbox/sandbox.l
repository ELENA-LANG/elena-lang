import system'routines;
import system'collections;
import extensions;
//import system'text;

public program()
{
   var extension := mssgconst toInt<intConvertOp>[1];
   var selector := (x => extension(x));

   var r := selector("2");

//   var a := new ArrayList().append("1").append("2");

//   var r := a.selectBy(mssgconst toInt<intConvertOp>[1]).summarize();

//    console.printLine(/*console.readLine()*/ "1 2"
//                              .split()
//                              .selectBy(mssgconst toInt<intConvertOp>[1])
//                              .summarize())
}
