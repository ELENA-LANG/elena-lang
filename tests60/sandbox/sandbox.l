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

import system'winforms;

public Program()
{
      MSG          msg := new MSG();
//      int          proceeded := 0;
      int          result := 0;
      bool         running := true;    
      
//      WindowHandle handleCopy := /*_mainWindow;*/default;

      while (running) {
         result := extern USER32.PeekMessageW(msg, 0, 0, 0);
         if (result == 0)
         {
            int cmd := msg.Code;
            if (cmd == WM_QUIT) {
               running := false;
            };
            
/*            excluded {
               extern USER32.TranslateMessage(msg);
               extern USER32.DispatchMessageW(msg)               
            }*/
         };
//         _onUpdate();
      };

}
