import extensions;
import system'runtime;

public singleton SystemMonitor
{
   printMemoryStatistics()
   {
      GCStatistics statistics := GCManager.calcStatistics();
   
      console.printLine("=== Memory usage statistics ===");
      console.printLine("yg allocated:      ", statistics.ygInfo.allocated);
      console.printLine("yg free:           ", statistics.ygInfo.free);
      console.printLine("mg allocated:      ", statistics.mgInfo.allocated);
      console.printLine("mg free:           ", statistics.mgInfo.free);
      console.printLine("perm allocated:    ", statistics.permInfo.allocated);
      console.printLine("perm free:         ", statistics.permInfo.free);
      console.printLine("minor collections: ", statistics.minorCollections);
      console.printLine("major collections: ", statistics.majorCollections);
      console.printLine("===============================");
   }
}

