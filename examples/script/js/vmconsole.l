#import extensions.
#import extensions'scripting.

// --- Control ---

control =
{
    initializing
    [
        1 till:('program'arguments length) &doEach:(:arg) 
                   [ 
                      try(scriptEngine load &path:('program'arguments@arg))
                      {
                          ! e [ console writeLine:(e message). ]
                      }
                   ].
    ]
    
    prompt
        = ">".
    
    proceed : command
    [
        if(command == "exit")
            [ 'program stop. ^ "leaving...". ];
            [
                try(scriptEngine eval:command eval:(Library new))
                {
                    ! e [ console writeLine:(e message). ]
                }.
                
                ^ 'newLine.
            ].
    ]
}.