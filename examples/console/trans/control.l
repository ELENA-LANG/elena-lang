#import system.
#import extensions.
#import extensions'text.

symbol program =
[
    control while:true &do:
    [
        var aSource := console writeLine:"Enter the cyrillic text for transliteration or prese enter to exit:" readLine:(String new).

        (0 == aSource length)
            ? [ AbortException new raise. ].

        console writeLine:(Transliteration eval:aSource).
    ].
].
