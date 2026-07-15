import system'collections;

public Program()
{
   auto d := Dictionary.new(0);

   Console.writeLine("Start");

   for (int i := 0; i < 200; i += 1)
      { d[i] := i * 2; Console.writeLine(i) };

   Console.writeLine("End");

}
