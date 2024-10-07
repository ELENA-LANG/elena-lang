import extensions;

// --- Prompt ---

public const IntSamplePrompt = "Enter the two integer numbers:";

public const RealSamplePrompt = "Enter the two real numbers:";

// --- EReader ---

extension EReader
{
   loadFrom(output)
   {
      try
      {
         ^ output.readLine().convertTo(self)
      }
      catch(Exception e)
      {
         Console.writeLine(e.Message).readChar();
                    
         AbortException.raise()
      }
   }   
}

EnteredNumber = forward number.new().loadFrom(Console);

// --- Program ---

public program()
{
   Console
      .printLine(forward prompt)
      .printLine("The sum is ",EnteredNumber + EnteredNumber)
      .readChar()
}
