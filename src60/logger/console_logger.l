import extensions;
import extensions'logging;

public class ConsoleLogger : LoggerBase
{
   constructor new(LogLevel baseLevel)
      <= super new(baseLevel)
   {
   }
   
   indexed internal write(string message)
   {
      Console.writeLine(message)
   }
}