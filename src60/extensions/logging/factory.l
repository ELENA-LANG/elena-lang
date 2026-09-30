internal sealed LoggerFactoryInstance : ILoggerFactory
{
   ILogger       _logger;
   Func<ILogger> _getter;
   
   private ILogger getLogger()
   {
      if:nil(_logger) {
         _logger := _getter();
      };
      
      ^ _logger;
   }
      
   constructor() 
   {
      _getter := ([] => ILogger);
   }
   
   register(Func<ILogger> loggerGetter)
   {
      _getter := loggerGetter
   }
   
   register(ILogger logger)
   {
      if:not:nil(_logger) {
         InvalidOperationException.raise("Only one logger can be registered");
      };
      
      _logger := logger
   }
   
   ILogger new()
      = getLogger();   
}

public static ILoggerFactory LoggerFactory
   = new LoggerFactoryInstance();
