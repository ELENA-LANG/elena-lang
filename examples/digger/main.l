#define digger'loader'*.

// --- Program ---

#symbol Program =>
[
    Loader run:(RuleNameLoaderArg::"start").
].

// --- TestProgram ---

#symbol TestProgram =>
[
    Loader run:(RuleNameLoaderArg::"test").
].
