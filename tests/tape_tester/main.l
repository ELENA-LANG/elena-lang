import extensions.
import extensions'scripting.

const script0 = "^system'console writeLine(""Hello World"")".
const script1 = "^ { eval(x) [ ^extensions'math'mathControl power(x, 2 ) ] }".

InterpreterTest = (:script)
[
    escript eval(script).
].

InterpreterTestWithParam = (:script:param)
[
    var o := escript eval(script).
    
    ^ o eval(param).
].

program =
[
    Assert notFailed:[InterpreterTest(script0)].
    Assert ifEqual(InterpreterTestWithParam(script1,2), 4).
].