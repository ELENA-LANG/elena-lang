extension op
{
   printLine(params object[] list)
   {
      int len := list.Length;
      for(int i := 0, i < len, i := i + 1)
      {
         object obj := list[i];
            
         self.write(obj.toPrintable())
      }
   }
}

public program()
{
   console.printLine(1,"+",2,"=",1+2);
}
