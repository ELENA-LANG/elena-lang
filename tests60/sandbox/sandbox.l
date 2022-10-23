A; ss

public program()
{
   try {
      var a := new A();
      a.fail()  
   }
   catch(Exception ex) {
      console.writeLine(ex)
   }
}
