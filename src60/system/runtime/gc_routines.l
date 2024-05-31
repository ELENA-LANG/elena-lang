import system;

// --- GCHeapInfo ---
public struct GCHeapInfo
{
   uint allocated : rprop;
   uint free      : rprop;
}

// --- GCStatistics ---
public struct GCStatistics
{
   GCHeapInfo ygInfo : rprop;
   GCHeapInfo mgInfo : rprop;
   GCHeapInfo permInfo : rprop;
   uint minorCollections : rprop;
   uint majorCollections : rprop;
}

// --- GCManager ---

public singleton GCManager
{
   collectMinor() : external(system'core_routines'__gc_minor);

   collectFull() : external(system'core_routines'__gc_full);

   GCStatistics calcStatistics()
   {
      GCStatistics statistics := default;

      extern GetGCStatisticsLA(statistics);

      ^ statistics
   }

   resetStatistics()
   {
      extern ResetGCStatisticsLA();
   }
}