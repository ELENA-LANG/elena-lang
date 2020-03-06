import system'routines;
import system'text;
import extensions;

// --- RusLatRules ---

const rusLatRules = new object[]
{
    new { from = "а"; to = "a"; },
    new { from = "б"; to = "b"; },
    new { from = "в"; to = "v"; },
    new { from = "г"; to = "g"; },
    new { from = "д"; to = "d"; },
    new { from = "е"; to = "e"; },
    new { from = "ë"; to = "jo"; },
    new { from = "ж"; to = "zh"; },
    new { from = "з"; to = "z"; },
    new { from = "и"; to = "i"; },
    new { from = "й"; to = "ij"; },
    new { from = "к"; to = "k"; },
    new { from = "л"; to = "l"; },
    new { from = "m"; to = "m"; },
    new { from = "н"; to = "n"; },
    new { from = "о"; to = "о"; },
    new { from = "п"; to = "p"; },
    new { from = "р"; to = "r"; },
    new { from = "c"; to = "s"; },
    new { from = "т"; to = "t"; },
    new { from = "у"; to = "u"; },
    new { from = "ф"; to = "f"; },
    new { from = "х"; to = "h"; },
    new { from = "ц"; to = "c"; },
    new { from = "ч"; to = "ch"; },
    new { from = "ш"; to = "sh"; },
    new { from = "щ"; to = "w"; },
    new { from = "ы"; to = "y"; },
    new { from = "ь"; to = "'"; },
    new { from = "э"; to = "je"; },
    new { from = "ю"; to = "ju"; },
    new { from = "я"; to = "ja"; }
};

// === Transliteration ===

Transliteration(text)
{
    var output := new TextBuilder();
    
    text.forEach:(ch)
    {
        var rule := rusLatRules.seekEach:
            (x => x.from == ch.toPrintable() );
                
        if (nil == rule)
        { 
            output.write(ch) 
        }
        else
        { 
            output.write(rule.to)
        }
    };
    
    ^ output.Value
}