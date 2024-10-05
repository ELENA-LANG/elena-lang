// --- ProgramLoop ---

class ProgramLoopImpl
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
      console.printLine(_control.greeting() ?? EmptyString);
        
      var prompt := _control.Prompt ?? EmptyString;
        
      while (_active)
      {
         console.write(prompt);

         console.printLine(_control.proceed(console.readLine()))
      }
   }
}

public static ProgramLoop = ProgramLoopImpl.assign(forward program'control);
