//import extensions;
// 
//public program()
//{
//    var n := new Integer();
//    var m := new Integer();
// 
//    console.write:"Enter two space delimited integers:";
//    console.loadLine(n,m);
// 
//    var myArray := class Matrix<int>.allocate(n,m);
// 
//    myArray.setAt(0,0,2);
// 
//    console.printLine(myArray.at(0, 0))
//}

// --- matrixof ---

public sealed Matrix
{
   object[] _array;
   int _rows;
   int _columns;

   constructor allocate(rows, columns)
   {
   }
   
//      <= allocate(cast int(rows), cast int(columns));
    
   constructor allocate(int rows, int columns)
   {
      _rows := rows;
      _columns := columns;
        
      int len := _rows * _columns;
        
      _array := new object[](len)
   }
   
}

public program()
{
}