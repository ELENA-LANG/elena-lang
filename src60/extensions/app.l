import system'routines;

#if (__project["_Win"]) // Windows

// --- program_arguments ---

public static Program_arguments = 
    new extensions'text'TokenEnumerator(CommandLine.toString(), new extensions'text'TokenStatemachine()).selectBy::(s => s.trim($34)).toArray();

// obosolete
public program_arguments = Program_arguments;
  
#elif (__project["_Linux"] || __project["_FreeBSD"])

// --- Arguments ---

public static Program_arguments = 
    CommandLineArguments;

// obosolete
public program_arguments = Program_arguments;

#endif

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
      Console.printLine(_control.greeting() ?? EmptyString);
        
      var prompt := _control.Prompt ?? EmptyString;
        
      while (_active)
      {
         Console.write(prompt);

         Console.printLine(_control.proceed(Console.readLine()))
      }
   }
}

public static ProgramLoop = ProgramLoopImpl.assign(forward program'control);
