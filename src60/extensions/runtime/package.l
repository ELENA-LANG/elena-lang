import extensions;
import system'text;
import system'runtime;

public extension packageOp
{
   string getFullPackageInfo()
   {
      auto package := self.Package();
      auto output := new StringBuilder();

      if ((package.Namespace ?? string.MinValue).Length != 0)
         output.printLine("Namespace: ", self.Namespace);

      if ((package.Name ?? string.MinValue).Length != 0)
         output.printLine("Name: ", self.Name);

      if ((package.Version ?? string.MinValue).Length != 0)
         output.printLine("Name: ", self.Version);

      if ((package.Author ?? string.MinValue).Length != 0)
         output.printLine("Author: ", self.Author);   

      ^ output.Value
   }
}