import system'collections;
import system'dynamic;
import extensions;
import extensions'scripting;

const string ScriptFile = __project_constants["script_file"];

public singleton ScriptInterpreter
{
   function()
      = XScript.interpretPath(ScriptFile);
}

class FactoryAdapter : IFactory
{
   IFunctionRange                   _range;
   Union<IFunction, IPolarFunction> _function;

   IFunctionRange Range
      = _range;

   Union<IFunction, IPolarFunction> Function
     = _function;

   constructor(object proxy)
   {
      _range := (proxy.Range)::mockInterface<IFunctionRange>();
      
      var function := proxy.Function;
      if (function.respondTo(mssgconst x[2])) {
         _function := function::mockInterface<IPolarFunction>();
      }
      else {
         _function := function::mockInterface<IFunction>();
      };
   }
} 

public bootstrap : preloaded =
   true.then(
   {
      try {
         XScript.import("Provider", Provider.mixInto(
         ::{
            registerProxy(string name, object factoryProxy)
               = __arg self.register(name, new FactoryAdapter(factoryProxy));
         }));
         
         forward ScriptFunction();
      }
      catch(Exception ex) {
         Console.printLine("Script bootstrap fails, see the error:", ex)
      }
   });
