import extensions;
import extensions'text;

extension EReader
{
   loadFrom()
   {
      try
      {
         ^ "2".convertTo(self)
      }
      catch(Exception e)
      {
         console.writeLine(e.Message).readChar();
                    
         AbortException.raise()
      }
   }   
}

enteredNumber = Integer.new().loadFrom();

public program()
{
   console
      .printLine(enteredNumber)
      .readChar()

//   var o := new Object();
//   o.fail()
}