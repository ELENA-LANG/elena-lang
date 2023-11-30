#new attributeSymbols;

/// modificator:
#let attributeSymbols["__ignore_duplicates"] :=  80006001h;

/// scope_accessors:
#let attributeSymbols["get"]         := 80005001h;
#let attributeSymbols["set"]         := 80005002h;

/// visibility:
#let attributeSymbols["public"]      := 80004001h;
#let attributeSymbols["private"]     := 80004002h;
#let attributeSymbols["internal"]    := 80004003h;
#let attributeSymbols["protected"]   := 80004004h;

/// property:
#let attributeSymbols["sealed"]      := 80003001h;
#let attributeSymbols["abstract"]    := 80003002h;
#let attributeSymbols["closed"]      := 80003003h;
#let attributeSymbols["yieldable"]   := 80003004h;
#let attributeSymbols["predefined"]  := 80003005h;
#let attributeSymbols["override"]    := 80003006h;

/// scope_prefix:
#let attributeSymbols["const"]       := 80002001h;
#let attributeSymbols["embeddable"]  := 80002002h;
#let attributeSymbols["ref"]         := 80002003h;
#let attributeSymbols["readonly"]    := 80002004h;
#let attributeSymbols["retoverload"] := 8000200Ah;
#let attributeSymbols["params"]      := 8000200Bh;
                                                 
/// scope:
#let attributeSymbols["class"]       := 80001001h;
#let attributeSymbols["struct"]      := 80001002h;
#let attributeSymbols["symbol"]      := 80001003h;
#let attributeSymbols["constructor"] := 80001004h;
#let attributeSymbols["extension"]   := 80001005h;
#let attributeSymbols["singleton"]   := 80001006h;
#let attributeSymbols["interface"]   := 80001007h;
#let attributeSymbols["method"]      := 80001008h;
#let attributeSymbols["field"]       := 80001009h;
#let attributeSymbols["nonstructural"] := 8000100Ah;
#let attributeSymbols["generic"]     := 8000100Bh;
#let attributeSymbols["function"]    := 8000100Ch;
#let attributeSymbols["var"]         := 8000100Dh;
#let attributeSymbols["this"]        := 8000100Eh;
#let attributeSymbols["static"]      := 8000100Fh;
#let attributeSymbols["cast"]        := 80001011h;
#let attributeSymbols["new"]         := 80001012h;
#let attributeSymbols["dispatch"]    := 80001013h;
#let attributeSymbols["extern"]      := 80001015h;
#let attributeSymbols["intern"]      := 80001016h;
#let attributeSymbols["forward"]     := 80001017h;
#let attributeSymbols["import"]      := 80001018h;
#let attributeSymbols["mixin"]       := 80001019h;
#let attributeSymbols["auto"]        := 8000101Ch;
#let attributeSymbols["namespace"]   := 80001021h;
#let attributeSymbols["super"]       := 80001024h;
#let attributeSymbols["inline"]      := 80001025h;
#let attributeSymbols["__probe"]     := 80001026h;
#let attributeSymbols["template"]    := 80001027h;
#let attributeSymbols["weak"]        := 80001029h;

/// primitive types
#let attributeSymbols["__string"]    := 80000801h;
#let attributeSymbols["__float"]     := 80000802h;
#let attributeSymbols["__int"]       := 80000803h;
#let attributeSymbols["__uint"]      := 80000804h;
#let attributeSymbols["__word"]      := 80000805h;
#let attributeSymbols["mssgconst"]   := 80000806h;
#let attributeSymbols["mssg"]        := 80000806h;
#let attributeSymbols["__subj"]      := 80000807h;
#let attributeSymbols["__symbol"]    := 80000808h;
#let attributeSymbols["__mssg"]      := 80000809h;
#let attributeSymbols["__ptr"]       := 8000080Ah;
#let attributeSymbols["__ext_mssg"]  := 8000080Bh;
#let attributeSymbols["__typeof"]    := 8000080Ch;
