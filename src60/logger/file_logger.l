import system'io;
import extensions;
import extensions'logging;

public class FileLogger : LoggerBase
{
   readonly TextWriter _writer;
   
   constructor new(LogLevel baseLevel, string path, bool appendMode)
      <= super new(baseLevel)
   {
      _writer := appendMode ? (File.textlogger(path)) : (File.textwriter(path));
   }
   
   indexed internal write(string message)
   {
      
   }
}