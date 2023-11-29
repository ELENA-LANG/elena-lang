import extensions;

singleton MultiValGetter
{
   object, object getMultiVal()
   {
      ^ ("Alex", 13)
//      ^ ("Alex".add("."), 13)
   }
}

public program()
{
   auto t := MultiValGetter.getMultiVal();
   console.printLine("Item1=",t.Item1,", Item2=", t.Item2);

//    var n := 2;

//    var t := (n + 1,"abc");
//    var t := (n, "abc");
       
//    console.printLine("Item1=",t.Item1,", Item2=", t.Item2);
    
 //   var a;
//    var b;
    
//    a,b := t;
//    console.printLine("a=",a,", b=", t.Item2);
}