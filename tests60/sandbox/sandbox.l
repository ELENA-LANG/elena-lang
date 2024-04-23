import extensions;
import extensions'dynamic;
import system'dynamic;
import algorithms;

public program()
{
   var arr := new object[] { 1, 5, -2, 56, -23 };

   var sortedArr := arr.quickSort();

   console.writeLine(sortedArr.toJson());

   
}
