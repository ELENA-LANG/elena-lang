import system;

public const int PROCESSOR_ARCHITECTURE_AMD64 = 9;
public const int PROCESSOR_ARCHITECTURE_ARM = 5;
public const int PROCESSOR_ARCHITECTURE_ARM64 = 12;
public const int PROCESSOR_ARCHITECTURE_IA64 = 6;
public const int PROCESSOR_ARCHITECTURE_INTEL = 0;
public const int PROCESSOR_ARCHITECTURE_UNKNOWN = 65535;

struct SYSTEM_INFO
{
   int     dwOemId : prop;
   int     dwPageSize : prop;
   pointer lpMinimumApplicationAddress : prop;
   pointer lpMaximumApplicationAddress : prop;
   pointer dwActiveProcessorMask : prop;
   int     dwNumberOfProcessors : prop;
   int     dwProcessorType : prop;
   int     dwAllocationGranularity : prop;
   short   wProcessorLevel : prop;
   short   wProcessorRevision : prop;
}

textblock EnvironmentImlp
{
   private static ProcessorType toProcessorType(int cpuType)
   {
      if (cpuType == PROCESSOR_ARCHITECTURE_AMD64)
         { ^ ProcessorType.AMD64 }
      else if (cpuType == PROCESSOR_ARCHITECTURE_ARM)
         { ^ ProcessorType.ARM }
      else if (cpuType == PROCESSOR_ARCHITECTURE_ARM64)
         { ^ ProcessorType.ARM64 }
      else if (cpuType == PROCESSOR_ARCHITECTURE_IA64)
         { ^ ProcessorType.IA64 }
      else if (cpuType == PROCESSOR_ARCHITECTURE_INTEL)
         { ^ ProcessorType.x86 };

      ^ ProcessorType.Unknown
   }

   static constructor()
   {
      SYSTEM_INFO si := default;

      extern {
         extern KERNEL32.GetSystemInfo(si);
      };

      _processorType := toProcessorType(si.dwOemId);
      _processorCount := si.dwNumberOfProcessors;
   }
}