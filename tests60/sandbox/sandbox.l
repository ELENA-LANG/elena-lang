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

public Program()
{
   UnsafePointer ptr := UnsafePointer.Default;
   if (ptr == UnsafePointer.Default) {
      Console.writeLine("It's empty");
   }
   
   //X ptr := X.Default
   
   //X ptr := new X();
}

//struct WGLGetExtensionsStringARBPTR    : extern_winapi_func<pointer>(HDC dc);
//
//public Program()
//{
//   handle libGL := extern KERNEL32.LoadLibraryW("opengl32.dll"w);
//
//   WGLGetExtensionsStringARBPTR wglGetExtensionsStringARB := WGLGetExtensionsStringARBPTR.load(libGL, "wglGetExtensionsStringARB");
//   bool f := wglGetExtensionsStringARB.isUnassigned();
//
//   //bool f := wglGetProcAddress.isUnassigned();
//         
//   HDC dc := default;
//   
//   pointer retVal := wglGetExtensionsStringARB(dc);
//}
