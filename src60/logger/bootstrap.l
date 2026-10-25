import system'dynamic;
import extensions'logging;
import system'text'parsing;

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

   registerFromString(string config)
   {
      auto regEx := new RegEx("(?<console>console;)|(?<file>file=(?<name>\w+);)|level=(?<level>\w+)");
      MatchResult result := regEx.match(config);
      if:not (result.Matched) {
         InvalidArgumentException.raise("config");
      };
      
      string? fileName := nil;
      LogLevel baseLevel := LogLevel.Info;
      bool consoleMode := regEx.checkGroup(result, "console");
      bool fileMode := regEx.checkGroup(result, "file");
      bool appendMode := false;
      if (fileMode && regEx.checkGroup(result, "name")) {
         auto g := regEx.retrieveGroup(result, "name");
         if:not:nil (var MatchGroup? cg := *g) {
            fileName := config.Substring(cg.Start, cg.End - cg.Start) + ".log";
         }
         else InvalidArgumentException.raise("config");
      };      
      
      if (regEx.checkGroup(result, "level")) {
         auto g := regEx.retrieveGroup(result, "level");
         if:not:nil (var MatchGroup? cg := *g) {
            string levelName := config.Substring(cg.Start, cg.End - cg.Start);
            levelName ???
               "Error" : { baseLevel := LogLevel.Error; }
               "Warning" : { baseLevel := LogLevel.Warning; }
               "Info" : { baseLevel := LogLevel.Info; }
               : { InvalidArgumentException.raise("config") };
         }
         else InvalidArgumentException.raise("config");         
      };
      
      if (consoleMode && fileMode) {
         registerFileAndConsoleLogger(baseLevel, fileName ?? "logger.log", appendMode);
      }
      else if (fileMode) {
         registerFileLogger(baseLevel, fileName ?? "logger.log", appendMode);
      }
      else registerConsoleLogger(baseLevel);
   }

   injectLogger()
   {
      DependencyInjector::register<ILogger>(LoggerFactory);
   }
   
   injectErrorHandler()
   {
      ILogger logger := LoggerFactory.new();
      
      startUpEvents.OnError := (err) {
         logger.writeEntry(LogLevel.Error, err.toPrintable());
      };
   }
}
