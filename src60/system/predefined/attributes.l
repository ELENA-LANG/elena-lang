#new attributesSymbols;

/// scope_accessors:
#let attributesSymbols["get"]         := 80005001h;
#let attributesSymbols["set"]         := 80005002h;

/// visibility:
#let attributesSymbols["public"]      := 80004001h;
#let attributesSymbols["private"]     := 80004002h;
#let attributesSymbols["internal"]    := 80004003h;
#let attributesSymbols["protected"]   := 80004004h;

/// property:
#let attributesSymbols["sealed"]      := 80003001h;
#let attributesSymbols["abstract"]    := 80003002h;
#let attributesSymbols["predefined"]  := 80003005h;

/// scope_prefix:
#let attributesSymbols["const"]       := 80002001h;
#let attributesSymbols["embeddable"]  := 80002002h;
#let attributesSymbols["ref"]         := 80002003h;
#let attributesSymbols["multiret"]    := 8000200Ah;
#let attributesSymbols["params"]      := 8000200Bh;
                                                 
/// scope:
#let attributesSymbols["class"]       := 80001001h;
#let attributesSymbols["struct"]      := 80001002h;
#let attributesSymbols["symbol"]      := 80001003h;
#let attributesSymbols["constructor"] := 80001004h;
#let attributesSymbols["extension"]   := 80001005h;
#let attributesSymbols["singleton"]   := 80001006h;
#let attributesSymbols["interface"]   := 80001007h;
#let attributesSymbols["method"]      := 80001008h;
#let attributesSymbols["field"]       := 80001009h;
#let attributesSymbols["nonstructural"] := 8000100Ah;
#let attributesSymbols["function"]    := 8000100Ch;
#let attributesSymbols["var"]         := 8000100Dh;
#let attributesSymbols["this"]        := 8000100Eh;
#let attributesSymbols["static"]      := 8000100Fh;
#let attributesSymbols["cast"]        := 80001011h;
#let attributesSymbols["new"]         := 80001012h;
#let attributesSymbols["dispatch"]    := 80001013h;
#let attributesSymbols["extern"]      := 80001015h;
#let attributesSymbols["intern"]      := 80001016h;
#let attributesSymbols["forward"]     := 80001017h;
#let attributesSymbols["import"]      := 80001018h;
#let attributesSymbols["namespace"]   := 80001021h;
#let attributesSymbols["super"]       := 80001024h;
#let attributesSymbols["inline"]      := 80001025h;
#let attributesSymbols["__probe"]     := 80001026h;
#let attributesSymbols["template"]    := 80001027h;

/// primitive types
#let attributesSymbols["__string"]    := 80000801h;
#let attributesSymbols["__int"]       := 80000803h;
//#let attributesSymbols["__meta"]      := 80000804h;
#let attributesSymbols["__word"]      := 80000805h;
#let attributesSymbols["mssgconst"]   := 80000806h;
#let attributesSymbols["mssg"]        := 80000806h;
#let attributesSymbols["__symbol"]    := 80000808h;
#let attributesSymbols["__mssg"]      := 80000809h;

#reload attributesSymbols;
