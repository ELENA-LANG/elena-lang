import extensions.
//import extensions'scripting.

const script0 = "program=[ ^ 1. ].".
const script1 = "import console = system'console. program=[ console writeLine(""Hello World!!"") ].".

singleton $scriptInterpreter
{
    eval(literal l)
    [
//        scriptEngine 
//            load path:"~\scripts\assembly.es";
//            load path:"~\scripts\escript.es";
//            eval(script);
//            eval(Library new); 
//            first    
    ]
}

InterpreterTest = (:script)
[
    var o := $scriptInterpreter eval(script).
].

program =
[
    Assert notFailed:[InterpreterTest(script0)].
].