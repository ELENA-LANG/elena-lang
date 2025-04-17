import extensions;

/*
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
    switch (fdwCtrlType)
    {
        // Handle the CTRL-C signal.
    case CTRL_C_EVENT:
        printf("Ctrl-C event\n\n");
        Beep(750, 300);
        return TRUE;

        // CTRL-CLOSE: confirm that the user wants to exit.
    case CTRL_CLOSE_EVENT:
        Beep(600, 200);
        printf("Ctrl-Close event\n\n");
        return TRUE;

        // Pass other signals to the next handler.
    case CTRL_BREAK_EVENT:
        Beep(900, 200);
        printf("Ctrl-Break event\n\n");
        return FALSE;

    case CTRL_LOGOFF_EVENT:
        Beep(1000, 200);
        printf("Ctrl-Logoff event\n\n");
        return FALSE;

    case CTRL_SHUTDOWN_EVENT:
        Beep(750, 500);
        printf("Ctrl-Shutdown event\n\n");
        return FALSE;

    default:
        return FALSE;
    }
}

int main(void)
{
    if (SetConsoleCtrlHandler(CtrlHandler, TRUE))
    {
        printf("\nThe Control Handler is installed.\n");
        printf("\n -- Now try pressing Ctrl+C or Ctrl+Break, or");
        printf("\n    try logging off or closing the console...\n");
        printf("\n(...waiting in a loop for events...)\n\n");

        while (1) {}
    }
    else
    {
        printf("\nERROR: Could not set control handler");
        return 1;
    }
    return 0;
}
*/

const int CTRL_C_EVENT = 0;
const int CTRL_BREAK_EVENT = 1;

extern int myFunction(uint fdwCtrlType)
{
   fdwCtrlType =>
      CTRL_C_EVENT : { Console.writeLine("Ctrl-C event"); ^ -1 }
      CTRL_BREAK_EVENT : { Console.writeLine("Ctrl-Break event"); ^ 0 };
      
   ^ 0;      
}

public program()
{
   extern KERNEL32.SetConsoleCtrlHandler(&myFunction, -1);
   
   Console.writeLine("The Control Handler is installed.");
   Console.writeLine("-- Now try pressing Ctrl+C or Ctrl+Break");
   
   while(true) {}
}
