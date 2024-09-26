import extensions;
import system'calendar;
import system'threading;

sample3()
{
   auto t := class Task<int>.run({
                               // Just loop.
                               int max := 1000000;
                               int ctr := 0;
                               for (ctr := 0; ctr <= max; ctr++) {
                                  if (ctr == max / 2 && Now.Hour <= 12) {
                                     ctr++;
                                     $break;
                                  }
                               };

                               ^ ctr
                            });
   Console.printLine("Finished ", t.Result, " iterations.");
}