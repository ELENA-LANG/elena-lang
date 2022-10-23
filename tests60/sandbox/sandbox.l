A;

public program()
{
   try {
      var a := new A();
      a.fail()  
   }
   catch(Exception err) {
     console.writeLine(err)
//      console.writeLine(ex)
   }
}
