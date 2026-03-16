import extensions;
import extensions'text;

public Program()
{
   while(true)
   {
      var source := Console.writeLine("Enter the cyrillic text for transliteration or prese enter to exit:").loadLineTo(new StringWriter()).Value;

      if (0 == source.Length)
         { AbortException.raise() };

      Console.writeLine(Transliteration(source))
   }
}
