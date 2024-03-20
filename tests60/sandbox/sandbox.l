public singleton convertor 
{ 
   generic(n)
   {
      MessageName mssg := __received.MessageName;

      ^ mssg(new ExtensionVariable(extensions'convertorOp, n))
   }
}

public program()
{
   var n := convertor.toInt("2");
}
