import extensions;

public program()
{
   var tb := new system'text'TextBuilder();
   tb.write("a");
   tb.write("b");
   tb.write("c");

   console.printLine(tb,"== abc ->", "abc" == tb);
}
