import system'io;
import system'threading;
import system'collections;
import system'collections'threadsafe;
import system'routines;
import extensions;
import extensions'routines;

public sample1()
{
   auto list := new ConcurrentQueue<string>();
   auto dirNames := new string[]{ ".", "..\threadpool" };
   auto tasks := new List<Task>();

   foreach(string dirName; in dirNames) {
      Task t := Task.run( { foreach(auto path; in Directory.getFiles(dirName)) 
                                    list.push(path); } );
      tasks.append(t);
   };

   Task.waitAll(tasks.Value);

   list.Snapshot.forEach(PrintingLn);

   Console.writeLine("Main thread exits")
}