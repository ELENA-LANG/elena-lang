import extensions;
import system'runtime;

internal A
{
   constructor() { console.writeLine("A created") }
}

internal static A a = new A();

public program()
{
   var a1 := a;
   var a2 := a;

//   console.writeLine($"Processor type:{Environment.ProcessorType}");
//   console.writeLine($"Processor count:{Environment.ProcessorCount}");
}
