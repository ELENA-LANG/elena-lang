//// --- CommandLine ---
//
//singleton CommandLineFactory
//{
//    new
//    [
//        dirty_ptr aCommandLinePtr := system'external'KERNEL32 GetCommandLineW.
//
//        ^ aCommandLinePtr wide.
//    ]
//}
//
//public commandLine = 
//    CommandLineFactory new.
//
//// --- startUp ---
//
//class StartUpEvents
//{
//    T<Func1> event onStop :: theFinializeEvent.
//    
//    /*internal */stopping : arg
//    [
//        ifnot (nil == theFinializeEvent)
//            [ theFinializeEvent(arg) ]
//    ]
//}
//
//public static startUpEvents = StartUpEvents new.

public symbol startUp =
    {
        start(/*StartUpEvents events*/)
        {
//            system'core_routines'__exception register.
//            
            try
            {
                forward program()
            }
            catch(Exception e)
            {
                //console writeLine(e literal)
            }
//            catch(AbortException e)
//            {
//            }

//            events stopping:nil
        }        
    }
    .start()/*:startUpEvents*/;
