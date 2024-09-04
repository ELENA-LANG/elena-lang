import system;

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
