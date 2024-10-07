singleton CommandLineFactory
{
    wide load()
    {
        pointer commandLinePtr := extern KERNEL32.GetCommandLineW();

        ^ cast wide(commandLinePtr);
    }
}

public CommandLine = 
    CommandLineFactory.load();
