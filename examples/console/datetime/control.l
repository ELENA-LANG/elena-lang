import system'calendar;
import extensions;

// --- Control ---

public singleton control
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
            1 { ^now.toString() }
            2 { ^utcNow.toString() }
            3 { ^now.Year.toString() }
            4 { ^now.toShortTimeString() }
            7 { forward program.stop(); ^ emptyString }
            : { ^"Invalid choice" }
    }
}