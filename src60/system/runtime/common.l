import system;

// --- Package ---

public class Package
{
   string Namespace : rprop;
   string Name : rprop;
   string Version : rprop;
   string Author : rprop;

   private string loadField(pointer packagePtr, int index) : external(system'core_routines'__loadPackageInfo);

   constructor(pointer packagePtr)
   {
      Namespace := self.loadField(packagePtr, 0);
      Name := self.loadField(packagePtr, 1);
      Version := self.loadField(packagePtr, 2);
      Author := self.loadField(packagePtr, 3);
   }
}

// --- basic extensions ---

public extension commonOp
{
   Package Package()
   {
      pointer ptr := __packageInfo;

      ^ new Package(ptr);
   }
}

// --- ProcessorType ---

public const struct ProcessorType : enum<int>(x86 = 0, AMD64 = 1, ARM = 2, ARM64 = 2, IA64 = 3, Unknown = 255);
