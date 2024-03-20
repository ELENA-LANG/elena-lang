public singleton convertor 
{ 
   generic(n)
   {
      MessageName mssg := __received.MessageName;

      ^ mssg(new ExtensionVariable(n, extensions'intConvertOp))
   }
}

public program()
{
   var n := convertor.toInt("2");

   console.writeLine(n)
}
