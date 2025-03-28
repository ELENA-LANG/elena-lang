import extensions;
import system'runtime;

public singleton SystemMonitor
{
   printMemoryStatistics()
   {
      GCStatistics statistics := GCManager.calcStatistics();
   
      Console.printLine("=== Memory usage statistics ===");
      Console.printLine("yg allocated:      ", statistics.ygInfo.allocated.toString(10));
      Console.printLine("yg free:           ", statistics.ygInfo.free.toString(10));
      Console.printLine("mg allocated:      ", statistics.mgInfo.allocated.toString(10));
      Console.printLine("mg free:           ", statistics.mgInfo.free.toString(10));
      Console.printLine("perm allocated:    ", statistics.permInfo.allocated.toString(10));
      Console.printLine("perm free:         ", statistics.permInfo.free.toString(10));
      Console.printLine("minor collections: ", statistics.minorCollections.toString(10));
      Console.printLine("major collections: ", statistics.majorCollections.toString(10));
      Console.printLine("===============================");
   }
}

