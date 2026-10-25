//import system'text'parsing;
//
//
//import system'dynamic;
//import extensions'logging;
//import logger;
//
//public class MyClassToLog
//{
//   readonly ILogger _logger;
//   
//   constructor new(ILogger logger) : injectable_constructor()
//   {
//      _logger := logger;
//      _logger.writeEntry(LogLevel.Info, "Creating...");
//   }
//   
//   start()
//   {
//      _logger.writeEntry(LogLevel.Info, "Starting...");
//   }
//   
//   failMe()
//   {
//      Exception.raise("It fails me");
//   }   
//   
//   do()
//   {
//      _logger.writeEntry(LogLevel.Info, "Doing...");
//      
//      try
//      {
//         failMe()
//      }
//      catch(Exception ex)
//      {
//         _logger.writeFormattedEntry(LogLevel.Error, "Failed: {0}", ex);
//      }
//   }
//   
//   stop()
//   {
//      _logger.writeEntry(LogLevel.Info, "Stopping...");
//   }
//}
//
//public Program()
//{
//   //LogSetup.registerFromString("console;file=output;").injectLogger();
//   LogSetup.registerFromString("file=output;").injectLogger();
//   
//   //LogSetup.registerConsoleLogger(LogLevel.Info).injectLogger();
//   
//   MyClassToLog.new().start().do().stop();
//   
//   2.fail()
//}

namespace registration
{
   public symbol registerMe
      = Console.writeLine("registration was called");
}

import sandbox'registration : preloaded;

public Program()
{
   Console.writeLine("Program starting");
}
