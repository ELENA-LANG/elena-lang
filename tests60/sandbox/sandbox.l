static sync = new object();

returnVal()
{
   lock(sync)  
   {
      ^ "Hello"
   }
}


public program()
{
   console.writeLine(returnVal());
}


