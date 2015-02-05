#define system.
#define system'calendar.
#define extensions.

// --- Control ---

#symbol control =
{
    prompt : anOutput
    [
        consoleEx writeLine:#10"Please select your option:"
              writeLine:" 1 - current local date"
              writeLine:" 2 - current UTC date"
              writeLine:" 3 - current year"
              writeLine:" 4 - current time"
              writeLine:" 7 - exit"
              write:">".
              
    ]
    
    read : anInput
    [
        #var aChoice := anInput readLine:(Integer new).
        
        ^ aChoice =>
            1 ? [ now literal ]
            2 ? [ utcNow literal ]
            3 ? [ now year literal ]
            4 ? [ dateTimeControl toShortTime &long:now ]
            7 ? [ 'program stop. ^ emptyLiteralValue. ]
            ! [ "Invalid choice" ].
    ]
}.