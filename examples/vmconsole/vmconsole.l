#define std'dictionary'*.
#define std'basic'*.
//#define std'patterns'*.
#define sys'io'*.
#define vm'* = sys'vm'engine'*.
#define ext'io'*.

// --- Controller ---

#symbol EnteredName = console << "Enter the grammar name:%n" >> String.

#symbol EnteredPath = console << "Enter the path:%n" >> String.

#symbol EnteredScript = console << "Enter the script:%n" >> String.


#static Prompt = 
    "%nVMConsole 0.2 (C)2012 by Alex Rakov%n" +
    "Please select your option:%n" +
    "1 - execute inline script%n" +
    "2 - load inline script%n" +
    "3 - generate CF grammar from the file%n" +
    "4 - load script from the file%n" +
    "5 - execute a script%n" +
    "6 - exit%n".

#symbol Action : aChoice =
[
    #if aChoice
        ifequal:1
        [
            ^ vm'controller write &literal:EnteredScript &:vm'AInlineScript back:"%nexecuted%n"
                | vm'script_error | vm'vm_error.
        ]
        | ifequal:2 
        [
            ^ vm'controller write &path:EnteredPath &:vm'AInlineScriptFile back:"%nexecuted%n"
                | vm'script_error | vm'vm_error.
        ]
        | ifequal:3
        [
            ^ vm'controller write &vm'dsa_name:EnteredName &path:EnteredPath &:vm'ACFGrammarFile back:"%nexecuted%n"
                | vm'script_error | vm'vm_error.
        ]
        | ifequal:4
        [
            ^ vm'controller write &vm'dsa_name:EnteredName &path:EnteredPath &:vm'AScriptFile back:"%nexecuted%n"
                | vm'script_error | vm'vm_error.
        ]
        | ifequal:5
        [
            ^ vm'controller write &literal:EnteredScript &vm'dsa_name:EnteredName &:vm'AScript back:"%nexecuted%n"
                | vm'script_error | vm'vm_error.
        ]
        | ifequal:6
        [
            'program stop.
        ]
        | [
            ^ "Wrong choice".
        ].
].
