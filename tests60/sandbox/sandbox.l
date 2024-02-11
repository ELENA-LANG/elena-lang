import extensions;

singleton Helper
{
   printLen(params object[] args)
   {
      console.printLine(args.Length);
   }
}

public program()
{
   Helper.printLen(1,2,3);
   Helper.printLen(1,2);
   Helper.printLen(1);
   Helper.printLen();
}