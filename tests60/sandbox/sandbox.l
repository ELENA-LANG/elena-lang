import extensions;

singleton Helper
{
   testArg(arg1,arg2)
   {
      console.writeLine("testArg");
   }

   testVarg(params object[] args)
   {
      //var m := Expression.MessageCall(mssg testArg[2], )

      auto m := mssg testArg[2];

      m.invoke(Helper, params args)
   }
}

public program()
{
   Helper.testVarg(1,2);
}
