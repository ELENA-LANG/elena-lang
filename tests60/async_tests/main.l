import ltests;

public Program()
{
   Console.writeLine("--- ELENA 6 Async Functional Tests ---");

   Console.write("asyncAwaitMassiveTaskCheck:");
   asyncAwaitMassiveTaskCheck();
   Console.writeLine();

   Engine.run();

   Console.writeLine("--- Passed ---")
}
