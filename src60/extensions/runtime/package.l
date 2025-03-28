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
         output.print("Namespace: ", package.Namespace).print(NewLineConstant);

      if ((package.Name ?? string.MinValue).Length != 0)
         output.print("Name: ", package.Name).print(NewLineConstant);

      if ((package.Version ?? string.MinValue).Length != 0)
         output.print("Name: ", package.Version).print(NewLineConstant);

      if ((package.Author ?? string.MinValue).Length != 0)
         output.print("Author: ", package.Author).print(NewLineConstant);   

      ^ output.Value
   }
}