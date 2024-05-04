import extensions;

class Controller
{
   constructor()
   {
   }

   greeting()
   {
      console.printLine("ELENA Chat Client v.", VERSION);
        
      string userName := console.write("Enter nickname:").readLine();

//        __target.start(userName, "127.0.0.1", DEFAULT_PORT)
//                    | on:(e){ console.writeLine:"Cannot esteblish connection!"; AbortException.raise() };
//            
      ^ "Enter -q to exit the chat"
   }
   
}

public static control = new Controller();