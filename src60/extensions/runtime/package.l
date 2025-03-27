import extensions;
import system'text;
import system'runtime;

public extension packageOp
{
   string getFullPackageInfo()
   {
      auto package := self.Package();
      auto output := new StringBuilder();

      if ((self.Namespace ?? string.MinValue).length != 0)
         output.printLine("Namespace: ", self.Namespace);

      if ((self.Name ?? string.MinValue).length != 0)
         output.printLine("Name: ", self.Name);

      if ((self.Version ?? string.MinValue).length != 0)
         output.printLine("Name: ", self.Version);

      if ((self.Author ?? string.MinValue).length != 0)
         output.printLine("Author: ", self.Author);   

      ^ output.Value
   }
}