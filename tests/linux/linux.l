import extensions;

const int NCCS = 32;

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
}

public program()
{
   console.printLine("Start")//.fail();

           handle libHandle := extern libdl.dlopen(
                                              "libc.so.6",
                                              1);
    
           dirty_ptr variable := extern libdl.dlsym(
                            libHandle,
                            "stdin");

            int handle := variable.Value;


   termois savedState;
   termois newState;

   extern libc.tcgetattr(handle, savedState);

    newState = savedState;

    /* disable canonical input and disable echo.  set minimal input to 1. */
    newState.c_lflag &= ~(ECHO | ICANON);
    newState.c_cc[VMIN] = 1;

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



/*
    struct termios savedState, newState;
    int c;

    if (-1 == tcgetattr(STDIN_FILENO, &savedState))
    {
        return EOF;     /* error on tcgetattr */
    }

    newState = savedState;

    /* disable canonical input and disable echo.  set minimal input to 1. */
    newState.c_lflag &= ~(ECHO | ICANON);
    newState.c_cc[VMIN] = 1;

    if (-1 == tcsetattr(STDIN_FILENO, TCSANOW, &newState))
    {
        return EOF;     /* error on tcsetattr */
    }

    c = getchar();      /* block (withot spinning) until we get a keypress */

    /* restore the saved state */
    if (-1 == tcsetattr(STDIN_FILENO, TCSANOW, &savedState))
    {
        return EOF;     /* error on tcsetattr */
    }

    return c;
*/


           // int ch := extern libc.getchar();

   // console.write(".");

      //     byte retVal := byteConvertor.convert(ch);

   console.write(".");
}