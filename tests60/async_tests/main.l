import extensions;
import system'threading;
import ltests;

public Program()
{
   Console.writeLine("--- ELENA 6 Async Functional Tests ---");

   // Optional, test-only configuration: production async/await uses the lazy
   // defaults. The fixed limit keeps the growth/stress assertions deterministic.
   ThreadPool.configure(1, 4);
   Assert.ifEqual(ThreadPool.WorkerCount, 0);

   Engine.run();

   Console.writeLine("--- Passed ---")
}
