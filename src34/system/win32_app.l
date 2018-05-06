// --- CommandLine ---

singleton CommandLineFactory
{
    new
    [
        dirty_ptr aCommandLinePtr := system'external'KERNEL32 GetCommandLineW.

        ^ aCommandLinePtr wide.
    ]
}

public commandLine = 
    CommandLineFactory new.

//// --- startUp ---
//
//class $StartUpEvents
//{
//    func1 event onStop :: theFinializeEvent.
//    
//    $onStop : arg
//    [
//        ifnot ($nil == theFinializeEvent)
//            [ theFinializeEvent eval:arg ]
//    ]
//}
//
//static startUpEvents = $StartUpEvents new.

public symbol startUp =
    {
        start /*: events*/
        [
            system'core_routines'__exception register.
            
            try(forwards'program())
                {
                    on(Exception e) [ console writeLine(e literal) ]
                    
                    on(AbortException e) []                    
                }.
    
//            events $onStop:$nil
        ]        
    }
    start/*:startUpEvents*/.
