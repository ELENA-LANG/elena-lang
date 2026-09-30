import extensions'logging;

public abstract class LoggerBase : ILogger
{
   readonly LogLevel _baseLevel;
   
   indexed internal abstract write(string message);
   
   protected constructor new(LogLevel baseLevel)
   {
      _baseLevel := baseLevel
   }
   
   writeEntry(LogLevel level, string message)
   {
      if (*level <= *_baseLevel) {
         write($"[{level}]:{message}");
      }
   }
}