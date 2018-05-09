import extensions.
import extensions'text.

public symbol program =
[
    true doWhile:
    [
        var aSource := console writeLine:"Enter the cyrillic text for transliteration or prese enter to exit:"; readLineTo(String new).

        if (0 == aSource length)
            [ AbortException new; raise. ].

        console writeLine(Transliteration(aSource)).
    ].
].
