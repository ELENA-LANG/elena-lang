
A;

public program()
{
   var target := self;
   var mssg := mssg fail[1];

   var s:= target.__getClassName() + " : Method " + mssg.toPrintable() + " not found";

   console.writeLine(s);

//   var a := new A();
//   a.fail()  
}
