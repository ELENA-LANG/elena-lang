// --- ProgramLoop ---

class ProgramLoop
{
   bool   _active;
   object _control;
    
   constructor assign(control)
   {
      _active := true;
      _control := control;
        
      // if initializing is not defined - do nothing
      _control\.initializing();
   }
    
   stop() { _active := false }
    
   function()
   {
      // if grettings is not defined - do nothing
      console.printLine(_control.greeting() ?? emptyString);
        
      var prompt := _control.Prompt ?? emptyString;
        
      while (_active)
      {
         console.write:prompt;

         console.printLine(_control.proceed(console.readLine()))
      }
   }
}

public static programLoop = ProgramLoop.assign(forward program'control);
