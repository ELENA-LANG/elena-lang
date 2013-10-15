////#define io'*   = ext'io'*.
#define game'* = dices'common'*.
#define console'* = dices'console'*.

// --- Subjects ---

#subject _controller.
#subject _exiting.

// --- Adapter ---

#symbol Adapter =
{
    _controller'read : aController
    [
        'program'output << "Please select menu option:%n".
        
        game'Menu _menu'prompt'do.
        
        #if aController run:(game'Menu create:(self char'get))
            | [ 'program'output << "Wrong option%n". ].
        
        ^ aController.
    ]
    
    _controller'write : anOption
    [
        
    ]
}.

// === Menu ===

#hint[subj:_exiting]
#symbol Exiting = { nillable'ifNot [] }.

// --- BaseMenu ---
#hint[dispatchable]
#symbol BaseMenu =
{
    _menu'prompt'do
    [    
        'program'output << "q - exit%n".
    ]

    create : anOption
    [
        ^ self char'create:(anOption char'get).
    ]

    char'create : aChar
    [
        #if ("q"==aChar)?
            [ ^ Exiting. ]
            | [ ^ $next char'create:aChar. ].
    ]
}.

// --- Controller ---
#hint[subj:_controller, dispatchable]
#class Controller
{
    #role Finish
    {
        #method process'do [ $self fail. ]
    }
    
    #method run : anOption
    [
        $next run:anOption.
    ]
    
    #method _exiting'run
    [
        #shift Finish.
    ]
    
    #method new
    [
        'program'output << "ELENA Project:Dice game sample by Alex Rakov%n%n".
    ]
    
//    #method menu'find : anOption
//    [
//        #if("q" == anOption)?
//        [
//            #shift Finish. 
//            
//            ^ Message::"Bye%n.".
//        ]
//        | [ ^ Message::"Wrong option%n". ].
//    ]
    
    #method process'do 
    [
        
    ]
}

// --- Assembler ---

#symbol Assembler =>
[
    #cast(sys'io'StdInput, sys'io'StdOutput)
        group'inject:Adapter.
        
    game'Menu
        group'inject:console'BaseMenu.
].

//// --- Message ---
//
//#symbol Message : aText
//    => ('program'output << aText). 
//// --- KniffelMenu ---
