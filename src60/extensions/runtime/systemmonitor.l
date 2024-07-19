import extensions;
import system'runtime;

public singleton SystemMonitor
{
   printMemoryStatistics()
   {
      GCStatistics statistics := GCManager.calcStatistics();
   
      console.printLine("=== Memory usage statistics ===");
      console.printLine("yg allocated:      ", statistics.ygInfo.allocated.toString(10));
      console.printLine("yg free:           ", statistics.ygInfo.free.toString(10));
      console.printLine("mg allocated:      ", statistics.mgInfo.allocated.toString(10));
      console.printLine("mg free:           ", statistics.mgInfo.free.toString(10));
      console.printLine("perm allocated:    ", statistics.permInfo.allocated.toString(10));
      console.printLine("perm free:         ", statistics.permInfo.free.toString(10));
      console.printLine("minor collections: ", statistics.minorCollections.toString(10));
      console.printLine("major collections: ", statistics.majorCollections.toString(10));
      console.printLine("===============================");
   }
}

