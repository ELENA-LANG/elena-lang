import system'threading;

// --- threadControl ---

public singleton threadControl
{
   start(Func f)
   {
      var t := Thread.assign(f);
      
      t.start();
      
      ^ t
   }
   
   sleep (int n)
   {
      Thread.sleep(n)
   }
}
