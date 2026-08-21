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

   long TickCount64
   {
      get()
         = systemInfo.TickCount64;
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

   long TickCount64
   {
      get()
         = extern KERNEL32.GetTickCount64();
   }

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

#elif (__project["_Linux"] || __project["_FreeBSD"] || __project["_MacOS"])

#if (__project["_MacOS"])

const int _SC_NPROCESSORS_ONLN = 58;

#else

const int _SC_NPROCESSORS_ONLN = 84;

#endif

#if (__project["_Linux"])

const int CLOCK_MONOTONIC = 1;

#elif (__project["_FreeBSD"])

const int CLOCK_MONOTONIC = 4;

#elif (__project["_MacOS"])

const int CLOCK_MONOTONIC = 6;

#endif

internal class SystemInfo
{
   int           ProcessorCount : rprop;
   ProcessorType ProcessorType : rprop;   

   long TickCount64
   {
      get()
      {
#if (__project["_Linux32"])
         int stamp[2];
#else
         long stamp[2];
#endif
         extern libc.clock_gettime(CLOCK_MONOTONIC, stamp);

         ^ cast long(stamp[0]) * 1000l + cast long(stamp[1]) / 1000000l
      }
   }

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

#elif (__project["_LinuxARM64"] || __project["_MacOSARM64"])

      ProcessorType := class ProcessorType.ARM64;

#endif

      ProcessorCount := numCPU;
   }
}

#endif

internal static SystemInfo systemInfo
   = new SystemInfo();
