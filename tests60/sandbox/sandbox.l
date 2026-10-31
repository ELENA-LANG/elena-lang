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

import system'drawing;


/*struct Function
{
   __winapi_fn_ptr func;
   
   constructor()
   {
      handle libGL := extern KERNEL32.LoadLibraryW("opengl32.dll"w);
      func := extern KERNEL32.GetProcAddress(libGL, "wglGetProcAddress");
   }
   
   function(HDC dc)
   {
      int retVal := func(dc);
   }
}*/


public struct wglGetProcAddressPtr : extern_winapi_func(HDC dc);

public Program()
{
   handle libGL := extern KERNEL32.LoadLibraryW("opengl32.dll"w);

   wglGetProcAddressPtr wglGetProcAddress := wglGetProcAddressPtr.load(libGL, "wglGetProcAddress");
   
   HDC dc := default;
   
   int retVal := wglGetProcAddress(dc);
}
