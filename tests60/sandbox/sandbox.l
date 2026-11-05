import system'collections;

import extensions;
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
//public symbol Initialization =
//   LogSetup
//      .registerFromString("file=output;console;")
//      .injectErrorHandler()
//      .injectLogger();
//
//public Program()
//{
//   MyClassToLog.new().start().do().stop();
//}

/*
struct Y
{
   int y;

   constructor(int y)
   {
      this y := y
   }
}

struct X
{
   int x;

   Y sum(int n)
      = n + 2;
}

public Program()
{
   X x := default;

   Y y := x.sum(2)
}
*/

const string[] Prefixes = $[ "OpenGL ES-CM ", "OpenGL ES-CL ", "OpenGL ES " ];

public Program()
{
   Console.printLine(Prefixes.asEnumerable());
   
//   string[] Prefixes := $[ "OpenGL ES-CM ", "OpenGL ES-CL ", "OpenGL ES " ];
   
   //X ptr := X.Default
   
   //X ptr := new X();
}
