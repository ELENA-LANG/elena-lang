import extensions;
import extensions'text;
import system'routines;

const string AllowedCharacters = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";

rnd = randomGenerator;

randomChar 
    = AllowedCharacters[rnd.nextInt(AllowedCharacters.Length)];

public program()
{
   var s := 0.repeatTill("abc".Length).selectBy:(x => randomChar).summarize(new StringWriter());
}