import system'calendar;
import extensions;

// --- Control ---

public singleton Control
{
    get string Prompt
        = "Please select your option:
 1 - current local date
 2 - current UTC date
 3 - current year
 4 - current time
 7 - exit
>";
    
    proceed(s)
    {
        auto choice := s.toInt();
        
        choice =>
            1 : { ^Now.toString() }
            2 : { ^UtcNow.toString() }
            3 : { ^Now.Year.toString() }
            4 : { ^Now.toShortTimeString() }
            7 : { forward program.stop(); ^ EmptyString }
            ! : { ^"Invalid choice" }
    }
}