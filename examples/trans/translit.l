#define system.
#define system'routines.
#define system'text.

// --- RusLatRules ---

#symbol rusLatRules =
(
    { from = "а". to = "a". },
    { from = "б". to = "b". },
    { from = "в". to = "v". },
    { from = "г". to = "g". },
    { from = "д". to = "d". },
    { from = "е". to = "e". },
    { from = "ë". to = "jo". },
    { from = "ж". to = "zh". },
    { from = "з". to = "z". },
    { from = "и". to = "i". },
    { from = "й". to = "ij". },
    { from = "к". to = "k". },
    { from = "л". to = "l". },
    { from = "m". to = "m". },
    { from = "н". to = "n". },
    { from = "о". to = "о". },
    { from = "п". to = "p". },
    { from = "р". to = "r". },
    { from = "c". to = "s". },
    { from = "т". to = "t". },
    { from = "у". to = "u". },
    { from = "ф". to = "f". },
    { from = "х". to = "h". },
    { from = "ц". to = "c". },
    { from = "ч". to = "ch". },
    { from = "ш". to = "sh". },
    { from = "щ". to = "w". },
    { from = "ы". to = "y". },
    { from = "ь". to = "'". },
    { from = "э". to = "je". },
    { from = "ю". to = "ju". },
    { from = "я". to = "ja". }
).

// === Transliteration ===

#symbol Transliteration = (:aText)
[
    #var anOutput := TextBuffer new.
    
    control run:aText &forEach: aChar
    [
        #var aRule := control run:rusLatRules &seeking: x 
                [ x from == aChar literal ].
        (nil == aRule)
            ? [ anOutput << aChar. ] 
            ! [ anOutput << aRule to. ].
    ].
    
    ^ anOutput literal.
].