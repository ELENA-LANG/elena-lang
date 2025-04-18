import extensions;

use extern KERNEL32;
use system'text'StringBuilder; 

public program()
{
   /*extern */KERNEL32.SetConsoleCtrlHandler(-1);

   Console.writeLine("Hello")
}
