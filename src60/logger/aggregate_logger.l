import extensions'logging;

public sealed class AggregateLogger : ILogger
{
   readonly ILogger[] _loggers;
   
   constructor(ILogger[] loggers)
   {
      _loggers := loggers
   }
   
   constructor new(params ILogger[] loggers)
   {
      _loggers := loggers
   }
   
   writeEntry(LogLevel level, string message)
   {
      foreach(auto logger; in _loggers) {
         logger.writeEntry(level, message)
      }
   }
}