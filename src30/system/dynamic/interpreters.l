// --- Tape ---

#class(group) Tape
{
    #field(type:array)theRoles.
    
    #method => #inline handle_tape.
}

// --- SelectorTape ---

#class SelectorTape : Tape
{
    #method => #inline handle_selector.
}