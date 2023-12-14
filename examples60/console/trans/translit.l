import system'routines;
import system'text;
import extensions;

// --- RusLatRules ---

/*const*/ rusLatRules = new object[]
{
    ( "а", "a" ),
    ( "б", "b" ),
    ( "в", "v" ),
    ( "г", "g" ),
    ( "д", "d" ),
    ( "е", "e" ),
    ( "ë", "jo" ),
    ( "ж", "zh" ),
    ( "з", "z" ),
    ( "и", "i" ),
    ( "й", "ij" ),
    ( "к", "k" ),
    ( "л", "l" ),
    ( "m", "m" ),
    ( "н", "n" ),
    ( "о", "о" ),
    ( "п", "p" ),
    ( "р", "r" ),
    ( "c", "s" ),
    ( "т", "t" ),
    ( "у", "u" ),
    ( "ф", "f" ),
    ( "х", "h" ),
    ( "ц", "c" ),
    ( "ч", "ch" ),
    ( "ш", "sh" ),
    ( "щ", "w" ),
    ( "ы", "y" ),
    ( "ь", "'" ),
    ( "э", "je" ),
    ( "ю", "ju" ),
    ( "я", "ja" )
};

// === Transliteration ===

Transliteration(text)
{
   var output := new TextBuilder();
    
   text.forEach::(ch)
   {
      var rule := rusLatRules.seekEach::
         (x => x.Item1 == ch.toPrintable() );
                
      if (nil == rule)
      { 
         output.write(ch) 
      }
      else
      { 
         output.write(rule.Item2)
      }
   };
    
   ^ output.Value
}