// --- CommandLine ---

singleton CommandLineFactory
{
    load()
    {
        byte buf[256];
        int len := 0;
        int argc := extern elenart.l_core_getargc;

        var args := Array.allocate(argc);
        for (int i := 0, i < argc, i := i + 1) {
            int n := i + 1;
            len := extern elenart.l_core_getarg(n, buf, 255);
            
            args[i] := String.fromByteArray(0, len, buf)
        };

        ^ args
    }
}

public commandLineArguments =
    CommandLineFactory.load();
