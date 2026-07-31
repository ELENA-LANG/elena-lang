import system'threading;

public Program()
{
   auto e := ManualResetEvent.new(true);
   e.wait();
   Console.writeLine("lib OK")
}
