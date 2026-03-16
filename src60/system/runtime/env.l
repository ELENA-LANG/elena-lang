public singleton Environment
{
   int ProcessorCount
   {
      get()
         = systemInfo.ProcessorCount;
   }

   ProcessorType ProcessorType
   {
      get()
         = systemInfo.ProcessorType;
   }
}

#if (__project["_Win"]) // Windows

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

internal class SystemInfo
{
   int           ProcessorCount : rprop;
   ProcessorType ProcessorType : rprop;   

   private ProcessorType toProcessorType(int cpuType)
   {
      if (cpuType == PROCESSOR_ARCHITECTURE_AMD64)
         { ^ class ProcessorType.AMD64 }
      else if (cpuType == PROCESSOR_ARCHITECTURE_ARM)
         { ^ class ProcessorType.ARM }
      else if (cpuType == PROCESSOR_ARCHITECTURE_ARM64)
         { ^ class ProcessorType.ARM64 }
      else if (cpuType == PROCESSOR_ARCHITECTURE_IA64)
         { ^ class ProcessorType.IA64 }
      else if (cpuType == PROCESSOR_ARCHITECTURE_INTEL)
         { ^ class ProcessorType.x86 };

      ^ class ProcessorType.Unknown
   }

   constructor()
   {
      SYSTEM_INFO si := default;

      excluded {
         extern KERNEL32.GetSystemInfo(si);
      };

      ProcessorType := toProcessorType(si.dwOemId);
      ProcessorCount := si.dwNumberOfProcessors;      
   }
}

#elif (__project["_Linux"] || __project["_FreeBSD"])

const int _SC_NPROCESSORS_ONLN = 84;

internal class SystemInfo
{
   int           ProcessorCount : rprop;
   ProcessorType ProcessorType : rprop;   

   constructor()
   {
      int numCPU := extern libc.sysconf(_SC_NPROCESSORS_ONLN);
      if (numCPU < 0)
         numCPU := 1;

#if (__project["_Linux32"])

      ProcessorType := class ProcessorType.x86;

#elif (__project["_Linux64"] || __project["_FreeBSD64"])

      ProcessorType := class ProcessorType.AMD64;

#elif (__project["_LinuxPPC64le"])

      ProcessorType := class ProcessorType.PPC64le;

#elif (__project["_LinuxARM64"])

      ProcessorType := class ProcessorType.ARM64;

#endif

      ProcessorCount := numCPU;
   }
}

#endif

internal static SystemInfo systemInfo
   = new SystemInfo();
