#define system.
#define system'calendar.
#define extensions.

//// --- Action ---
//
//#symbol Action : aChoice =
//[
//        #if aChoice
//            == 1 ? [ ^ WideStrValue &&calendar:Now &:elongdateformatter. ]
//            | == 2 ? [ ^ WideStrValue &&calendar:UTCNow &:elongdateformatter. ]
//            | == 3 ? [ ^ Now year. ]
//            | == 4 ? [ ^ WideStrValue &&calendar:Now &:etimeformatter. ]
//            | == 7 ? [ 'program stop. ^ "Exiting...". ]
//            | [ ^ "Invalid choice". ].
//].

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
    write : anInput
    [
        #var aChoice := anInput readLine:(Integer new).
        
        ^ aChoice =>
            1 ? [ now Literal ]
            2 ? [ utcNow Literal ]
            3 ? [ now Year Literal ]
            4 ? [ dateTimeControl toShortTime:now ]
            7 ? [ 'program stop. ^ emptyLiteralValue. ]
            ! [ "Invalid choice" ].
    ]
}.