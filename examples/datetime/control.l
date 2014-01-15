#define system.
#define system'calendar.
#define extensions.

// --- Output ---

#symbol output
    = consoleEx writeLine:"%nPlease select your option:"
              writeLine:" 1 - current local date"
              writeLine:" 2 - current UTC date"
              writeLine:" 3 - current year"
              writeLine:" 4 - current time"
              writeLine:" 7 - exit"
              write:">".

// --- Control ---

#symbol control =
{
    read : anInput
    [
        #var aChoice := anInput readLine:(Integer new).
        
        ^ aChoice =>
            1 ? [ now literal ]
            2 ? [ utcNow literal ]
            3 ? [ now Year literal ]
            4 ? [ dateTimeControl toShortTime:now ]
            7 ? [ 'program stop. ^ emptyLiteralValue. ]
            ! [ "Invalid choice" ].
    ]
}.