import extensions;

const int NCCS = 32;
const int ICANON = 0000002;   /* Canonical input (erase and kill processing).  */
const int VMIN = 6;
const int TCSANOW = 0;
const int ECHO = 0000010;   /* Enable echo.  */

struct termios
{
    int c_iflag;		/* input mode flags */
    int c_oflag;		/* output mode flags */
    int c_cflag;		/* control mode flags */
    int c_lflag;		/* local mode flags */
    byte c_line;		/* line discipline */
    byte c_cc[NCCS];		/* control characters */
    int c_ispeed;		/* input speed */
    int c_ospeed;		/* output speed */

    int LFlag
    {
        get()
            = c_lflag;
            
        set(v)
        {
            c_lflag := v
        }
    }
    
    setAt(int index, byte value)
    {
        c_cc[index] := value
    }
}

public program()
{
   console.printLine("Start");

   handle libHandle := extern libdl.dlopen(
                                      "libc.so.6",
                                      1);

   dirty_ptr variable := extern libdl.dlsym(
                    libHandle,
                    "stdin");

   int handle := variable.Value;

   termios savedState;
   termios newState;

   extern libc.tcgetattr(handle, savedState);

    newState := savedState;

    /* disable canonical input and disable echo.  set minimal input to 1. */
   newState.LFlag := newState.LFlag & ~(ECHO | ICANON);
   newState[VMIN] := 1;

   extern libc.tcsetattr(handle, TCSANOW, newState);

//    if (-1 == tcsetattr(STDIN_FILENO, TCSANOW, &newState))
//    {
//        return EOF;     /* error on tcsetattr */
//    }

    int ch := extern libc.getchar();      /* block (withot spinning) until we get a keypress */

    /* restore the saved state */
   extern libc.tcsetattr(handle, TCSANOW, savedState);

//    if (-1 == tcsetattr(STDIN_FILENO, TCSANOW, &savedState))
//    {
//        return EOF;     /* error on tcsetattr */
//    }

           // int ch := extern libc.getchar();

   console.write(".");

   byte retVal := byteConvertor.convert(ch);

   console.write(".");
}
