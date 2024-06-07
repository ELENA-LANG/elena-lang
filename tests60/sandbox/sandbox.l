    import system'routines;
    import system'dynamic;
    import extensions;
    
    public class Duck
    {
       canFly() = true;
        
       fly()
       {
          console.printLine("Duck flying")
       }    
    }

    public class Whale
    {
       swim()
       {
          console.printLine("Whale swimming")
       }
    }

    public program()
    {
       var list := new object[] { new Duck(), new Whale() };
       foreach(var animal; in list) {
         if (animal.canFly() \ back(false)) {
            animal.fly();
         }                
         else console.printLine(animal," can't fly");
       }
    }
