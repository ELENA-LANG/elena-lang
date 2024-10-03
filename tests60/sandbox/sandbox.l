import extensions;
import system'threading;
import system'routines;
import system'collections;

static int[] Numbers = class List<int>.load(Range.for(0, 30000).enumerator()).Value;

public program()
{
   auto task := class Task<long>.run({
                               long total := 0;
                               for (int i := 0; i < Numbers.Length; i++)
                               {
                                   total += Numbers[i];
                               };
                               ^ total;
                            });

   Console.writeLine("Doing some other work...");

   Console.printLine("Sum is ", task.Result);  
}
