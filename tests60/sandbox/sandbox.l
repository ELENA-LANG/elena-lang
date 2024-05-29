import extensions;
import extensions'runtime;

public program() 
{
   console.printLine("before:");
   SystemMonitor.printMemoryStatistics();

   var n := 0;
   while (n < 100000) {
      n := n + 1;
   };
   
   console.printLine("after:");
   SystemMonitor.printMemoryStatistics();
}
