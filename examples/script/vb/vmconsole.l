#import extensions'scripting.

// --- Control ---

control =
{
    initializing
    [
        scriptEngine 
            load &path:"~\scripts\elena.es".
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