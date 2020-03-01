/// Shuffles the characters of a string in such a way that as many of the character values are in a different position as possible.

import system'routines;
import extensions;
import extensions'text;

extension op
{
    get Shuffled()
    {
        var original := self.toArray();
        var shuffled := self.toArray();
    
        for (int i := 0, i < original.Length, i += 1) {
            for (int j := 0, j < original.Length, j += 1) {
                if (i != j && original[i] != shuffled[j] && original[j] != shuffled[i])
                {
                    shuffled.exchange(i,j)
                }                
            }
        };
    
        ^ shuffled.summarize(new StringWriter()).toString()
    }
    
    score(originalText)
    {
        var shuffled := self.toArray();
        var original := originalText.toArray();
        int score := 0;

        for (int i := 0, i < original.Length, i += 1) {
            if (original[i] == shuffled[i]) { score += 1 }
        };
        
        ^ score
    }
}

// --- Program ---

public program()
{
    new string[]{"abracadabra", "seesaw", "grrrrrr", "pop", "up", "a"}.forEach:(s)
    {
        var shuffled_s := s.Shuffled;

        console.printLine("The best shuffle of ",s," is ",shuffled_s,"(",shuffled_s.score(s),")")
    };

    console.readChar()
}