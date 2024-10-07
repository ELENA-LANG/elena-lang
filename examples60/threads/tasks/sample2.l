import extensions'threading;
import system'threading;

doWork(string taskName)
{
   Console.printLineConcurrent(taskName," starting");

   Thread.sleep(1000);

   Console.printLineConcurrent(taskName," complete");
}

sample2()
{
   // setting several tasks
   Task task1 := Task.assign({ doWork("Task 1") });
   Task task2 := Task.assign({ doWork("Task 2") });
   Task task3 := Task.assign({ doWork("Task 3") });

   // starting the tasks
   task1.start();
   task2.start();
   task3.start();

   Console.printLineConcurrent("Waiting for Tasks to complete");
   Task.waitAllArgs(task1, task2, task3);
   Console.printLineConcurrent("All Tasks are complete");
}