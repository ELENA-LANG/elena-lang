import ltests;
import system'threading;

public Program()
{
   Console.writeLine("--- ELENA 7 MTA Functional Tests ---");

   // Test-only bound keeps worker saturation deterministic
   ThreadPool.configure(1, 4);

   Engine.run();

   Console.writeLine("--- Passed ---")
}
