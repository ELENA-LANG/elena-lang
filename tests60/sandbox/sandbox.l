//import system'calendar;
//import system'threading;
//
//public class ThreadLocal
//{
//   __threadvar Date _current;
//   
//   constructor()
//   {
//      _current := Date.Now;
//   }
//   
//   printCurrent()
//   {
//      Console.writeLine(_current.toPrintable());
//   }   
//}
//
//public Program()
//{
//   auto t1 := Thread.start(
//   {
//      Thread.sleep(1000);
//      
//      var l := new ThreadLocal();
//      
//      Thread.sleep(10000);      
//      
//      l.printCurrent();      
//   });
//   
//   auto t2 := Thread.start(
//   {
//      Thread.sleep(5000);
//      
//      var l := new ThreadLocal();
//      
//      Thread.sleep(10000);
//      
//      l.printCurrent();      
//   });
//
//   system'runtime'GCManager.collectMinor();
//   
//   t1.join();
//   t2.join();
//}

import system'dynamic;
import extensions;
import system'routines;
import system'collections;
 
extension algorithmOp
{
    s_of_n()
    {
        var counter := new Integer();
        var n := self;
 
        ^ new ArrayList().mixInto(new
        {
            eval(i)
            {
                counter.append(1);

                if (weak self.Length < n)
                { 
                    weak self.append(i) 
                }
                else
                { 
                    if(Random.nextInt(counter) < n)
                        { weak self[Random.nextInt(n)] := i }
                };

                ^ weak self.Value
            }
        })
    }
}
 
public Program()
{
    var bin := Array.allocate(10).populate::(n => new Integer());
    for(int trial := 0; trial < 10000; trial += 1)
    {
        var s_of_n := 3.s_of_n();
 
        for(int n := 0; n < 10; n += 1)
        {
            var sample := s_of_n.eval(n);
 
            if (n == 9)
                { sample.forEach::(i){ bin[i].append(1) } }
        }
    };    
 
    Console.printLine(bin).readChar()
}