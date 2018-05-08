import system'calendar.
import extensions.

// --- Control ---

public control =
{
    prompt
        = "Please select your option:" + forwards'newLine +
          " 1 - current local date" + forwards'newLine +
          " 2 - current UTC date" + forwards'newLine +
          " 3 - current year" + forwards'newLine +
          " 4 - current time" + forwards'newLine +
          " 7 - exit" + forwards'newLine +
          ">".
    
    proceed : aChoice
    [
        (aChoice toInt) =>
            1 [ ^now literal. ];
            2 [ ^utcNow literal ];
            3 [ ^now year; literal ];
            4 [ ^now toShortTimeLiteral ];
            7 [ forwards'program stop. ^ emptyLiteral ];
            ! [ ^"Invalid choice" ].
    ]
}.