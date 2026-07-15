//import ltests;

public Program()
{
   Console.writeLine("--- ELENA 7 System API Functional Tests ---");

//   Engine.run();

   auto d := system'collections'Dictionary.new(0);

   Console.writeLine("Start");

   for (int i := 0; i < 200; i += 1)
      { d[i] := i * 2; Console.writeLine(i) };

   Console.writeLine("End");



   Console.writeLine("--- Passed ---")
}