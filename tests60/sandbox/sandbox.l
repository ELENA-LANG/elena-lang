import extensions;
//import extensions'text;
//
//// --- Program ---
//
//public program()
//{
//    var text := "aba";
//    var searchText := "b";  
//    var replaceText := "B";
//
//    var bm := new StringBookmark(text);
//
//    while (bm.find(searchText))
//    {
//        bm.delete(searchText).insert(replaceText)
//    };
//
//    console
//        .printLine("The resulting text:",bm)
//        .readChar() // wait for any key
//}

class Counter
{
   int value := 0;

   bool next()
   {
      if (value > 5) { ^ false; };

      value += 1;

      ^ true
   } 

   get Value()
      = value;
}

public program()
{
//   var b := false;
//   console.printLine("the value is ", b);
//   b 
//      ? { console.writeLine("Weak operator thinks, it is true") }
//      : { console.writeLine("Weak operator thinks, it is false") }

   var c := new Counter();
   $rep c.next() ?
//   while (c.next()) 
   {
       console.writeLine(*c);
   }
}
