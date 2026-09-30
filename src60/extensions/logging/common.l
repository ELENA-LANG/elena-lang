import extensions;

public const struct LogLevel : enum<int>(Info = 1,Warning = 2,Error= 3);

public interface ILoggerFactory
{
   abstract register(Func<ILogger> loggerGetter);
   abstract register(ILogger logger);
   
   abstract ILogger new();
}

public interface ILogger
{
   abstract writeEntry(LogLevel level, string message);
   writeFormattedEntry(LogLevel level, string message, params object[] args)
      <= writeEntry(level, message.interpolate(params args));
}

public singleton IdleLogger : ILogger
{
   writeEntry(LogLevel level, string message) {}
}
