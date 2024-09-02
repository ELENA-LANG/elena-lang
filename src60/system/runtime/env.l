import system;

public const struct ProcessorType : enum<int>(x86 ::= 0, AMD64 ::= 1, ARM ::= 2, ARM64 ::= 2, IA64 ::= 3, Unknown ::= 255);

public sealed class Environment : using(EnvironmentImlp)
{
   static int _processorCount;
   static ProcessorType _processorType;

   static int ProcessorCount
   {
      get()
         = _processorCount;
   }

   static ProcessorType ProcessorType
   {
      get()
         = _processorType;
   }

   private constructor() {}
}