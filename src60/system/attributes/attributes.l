#new attributes;

/// modificator:
#let attributes["__ignore_duplicates"] :=  80006001h;

/// scope_accessors:
#let attributes["get"]         := 80005001h;
#let attributes["set"]         := 80005002h;

/// visibility:
#let attributes["public"]      := 80004001h;
#let attributes["private"]     := 80004002h;
#let attributes["internal"]    := 80004003h;
#let attributes["protected"]   := 80004004h;

/// property:
#let attributes["sealed"]      := 80003001h;
#let attributes["abstract"]    := 80003002h;
#let attributes["closed"]      := 80003003h;
#let attributes["yieldable"]   := 80003004h;
#let attributes["predefined"]  := 80003005h;
#let attributes["override"]    := 80003006h;

/// scope_prefix:
#let attributes["const"]       := 80002001h;
#let attributes["embeddable"]  := 80002002h;
#let attributes["ref"]         := 80002003h;
#let attributes["readonly"]    := 80002004h;
#let attributes["out"]         := 80002005h;
#let attributes["retoverload"] := 8000200Ah;
#let attributes["params"]      := 8000200Bh;
                                                 
/// scope:
#let attributes["class"]       := 80001001h;
#let attributes["struct"]      := 80001002h;
#let attributes["symbol"]      := 80001003h;
#let attributes["constructor"] := 80001004h;
#let attributes["extension"]   := 80001005h;
#let attributes["singleton"]   := 80001006h;
#let attributes["interface"]   := 80001007h;
#let attributes["method"]      := 80001008h;
#let attributes["field"]       := 80001009h;
#let attributes["nonstructural"] := 8000100Ah;
#let attributes["generic"]     := 8000100Bh;
#let attributes["function"]    := 8000100Ch;
#let attributes["var"]         := 8000100Dh;
#let attributes["this"]        := 8000100Eh;
#let attributes["static"]      := 8000100Fh;
#let attributes["cast"]        := 80001011h;
#let attributes["new"]         := 80001012h;
#let attributes["dispatch"]    := 80001013h;
#let attributes["extern"]      := 80001015h;
#let attributes["intern"]      := 80001016h;
#let attributes["forward"]     := 80001017h;
#let attributes["import"]      := 80001018h;
#let attributes["mixin"]       := 80001019h;
#let attributes["distributed"] := 8000101Ah;
#let attributes["auto"]        := 8000101Ch;
#let attributes["namespace"]   := 80001021h;
#let attributes["super"]       := 80001024h;
#let attributes["inline"]      := 80001025h;
#let attributes["__probe"]     := 80001026h;
#let attributes["template"]    := 80001027h;
#let attributes["weak"]        := 80001029h;
#let attributes["packed"]      := 8000102Ch;


/// primitive types
#let attributes["__string"]    := 80000801h;
#let attributes["__float"]     := 80000802h;
#let attributes["__int"]       := 80000803h;
#let attributes["__uint"]      := 80000804h;
#let attributes["__word"]      := 80000805h;
#let attributes["mssgconst"]   := 80000806h;
#let attributes["mssg"]        := 80000806h;
#let attributes["__subj"]      := 80000807h;
#let attributes["__symbol"]    := 80000808h;
#let attributes["__mssg"]      := 80000809h;
#let attributes["__ptr"]       := 8000080Ah;
#let attributes["__ext_mssg"]  := 8000080Bh;
#let attributes["__typeof"]    := 8000080Ch;
