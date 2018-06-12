import extensions.
import extensions'scripting.

public program =
[
    escript eval("system'console writeLine(""Hello World"").").

    var o := escript eval(
       "^ { eval(x) [ ^extensions'math'mathControl power(x, 2 ) ]}").

    console printLine(o eval(2)).
].