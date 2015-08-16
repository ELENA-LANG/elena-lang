#define system.
#define system'calendar.
#define extensions.

// --- Control ---

#symbol control =
{
    prompt
        = "Please select your option:" + 'newLine +
          " 1 - current local date" + 'newLine +
          " 2 - current UTC date" + 'newLine +
          " 3 - current year" + 'newLine +
          " 4 - current time" + 'newLine +
          " 7 - exit" + 'newLine +
          ">".
    
    proceed : aChoice
    [
        ^ (aChoice toInt) =>
            1 ? [ now literal ]
            2 ? [ utcNow literal ]
            3 ? [ now year literal ]
            4 ? [ now toShortTimeLiteral ]
            7 ? [ 'program stop. ^ emptyLiteralValue. ]
            ! [ "Invalid choice" ].
    ]
}.