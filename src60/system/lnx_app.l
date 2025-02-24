// --- CommandLine ---

singleton CommandLineFactory
{
    load()
    {
        byte buf[256];
        int len := 0;
        int argc := extern GetArgCLA();

        var args := Array.allocate(argc);
        for (int i := 0; i < argc; i++) {
            len := extern GetArgLA(i, buf, 255);
            
            args[i] := String.fromByteArray(0, len, buf)
        };

        ^ args
    }
}

public CommandLineArguments =
    CommandLineFactory.load();
