import extensions.
import extensions'text.

symbol program =
[
    control while:true do:
    [
        var aSource := console writeLine:"Enter the cyrillic text for transliteration or prese enter to exit:"; readLineTo:(String new).

        if (0 == aSource length)
            [ AbortException new; raise. ].

        console writeLine:(Transliteration eval:aSource).
    ].
].
