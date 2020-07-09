public program()
{
   console.writeLine("Hello").fail();

   var o := 2;
   var s := o.__getClassName();
   console.writeLine(s);

   var m := mssgconst toPrintable[1];
   var s2 := m.toPrintable();
   console.writeLine(s2);
}