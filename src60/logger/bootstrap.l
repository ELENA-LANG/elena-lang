import extensions'logging;

public singleton LogSetup
{
   registerFileLogger(LogLevel baseLevel, string path)
      <= registerFileLogger(baseLevel, path, true);

   registerFileAndConsoleLogger(LogLevel baseLevel, string path, bool appendMode)
   {
      Func<ILogger> lazy_factory := ([ILogger] => AggregateLogger.new(
         FileLogger.new(baseLevel, path, appendMode),
         ConsoleLogger.new(baseLevel)));
            
      LoggerFactory.register(lazy_factory);
   }
      
   registerFileLogger(LogLevel baseLevel, string path, bool appendMode)
   {
      Func<ILogger> lazy_factory := ([ILogger] => FileLogger.new(baseLevel, path, appendMode));
      
      LoggerFactory.register(lazy_factory);      
   }
   
   registerConsoleLogger(LogLevel baseLevel)
   {
      Func<ILogger> lazy_factory := ([ILogger] => ConsoleLogger.new(baseLevel));
      
      LoggerFactory.register(lazy_factory);      
   }
   
   register(params ILogger[] loggers)
   {
      Func<ILogger> lazy_factory := ([ILogger] => new AggregateLogger(loggers));
      
      LoggerFactory.register(lazy_factory);
   }
}