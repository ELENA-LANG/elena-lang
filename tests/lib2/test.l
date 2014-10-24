#class(role)Tester
{
    #method step
    [
        #var n := 2.
        
        ^ n.
    ]
}

#symbol Symbol
   = Tester step.

// --- Program ---

#symbol Program =
[
    Symbol eval.
].