#import extensions.
#import extensions'scripting.

// --- Control ---

control =
{
    initializing
    [
        1 till:('program'arguments length) &doEach:(:arg) 
                   [ 
                      scriptEngine load &path:('program'arguments@arg) 
                   ].
    ]
    
    prompt
        = ">".
    
    proceed : command
    [
        command == "exit"
            ? [ 'program stop. ^ "leaving...". ]
            ! [ scriptEngine eval:command. ^ 'newLine. ].
    ]
}.