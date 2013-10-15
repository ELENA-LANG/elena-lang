// --- namespace shortcuts ---
#define game'*    = dices'common'*.
#define console'* = dices'console'*.

#define kniffel'console'* = dices'kniffel'console'*.

// --- ConsoleKniffelAssembler ---

#symbol ConsoleKniffelAssembler  =>
[
    sys'entries'ConsoleStdAssembler evaluate.
    
    console'Assembler evaluate.

    game'Menu
        group'inject:kniffel'console'Menu.
].
