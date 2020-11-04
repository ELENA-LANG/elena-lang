// --- CommandLine ---

singleton CommandLineFactory
{
    wide load()
    {
        dirty_ptr commandLinePtr := extern KERNEL32.GetCommandLineW();

        ^ cast wide(commandLinePtr);
    }
}

public commandLine = 
    CommandLineFactory.load();

// !! temporal

A
{
    prop int X;
}

public static SingleA
    = new A();