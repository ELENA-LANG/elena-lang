import system;

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