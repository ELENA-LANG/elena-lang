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
         console.writeLine(e.Message).readChar();
                    
         AbortException.raise()
      }
   }   
}

enteredNumber = forward number.new().loadFrom(console);

// --- Program ---

public program()
{
   console
      .printLine(forward prompt)
      .printLine("The sum is ",enteredNumber + enteredNumber)
      .readChar()
}
