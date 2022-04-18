#new attributesSymbols;

/// visibility:
#let attributesSymbols["public"]      := 80004001h;
#let attributesSymbols["private"]     := 80004002h;
#let attributesSymbols["protected"]   := 80004004h;

/// scope_prefix:
#let attributesSymbols["const"]       := 80002001h;
#let attributesSymbols["embeddable"]  := 80002002h;

/// scope:
#let attributesSymbols["class"]       := 80001001h;
#let attributesSymbols["struct"]      := 80001002h;
#let attributesSymbols["symbol"]      := 80001003h;
#let attributesSymbols["constructor"] := 80001004h;
#let attributesSymbols["singleton"]   := 80001006h;
#let attributesSymbols["method"]      := 80001008h;
#let attributesSymbols["field"]       := 80001009h;
#let attributesSymbols["var"]         := 8000100Dh;
#let attributesSymbols["static"]      := 8000100Fh;
#let attributesSymbols["dispatch"]    := 80001013h;
#let attributesSymbols["extern"]      := 80001015h;
#let attributesSymbols["intern"]      := 80001016h;
#let attributesSymbols["forward"]     := 80001017h;
#let attributesSymbols["import"]      := 80001018h;
#let attributesSymbols["inline"]      := 80001025h;

/// primitive types
#let attributesSymbols["__string"]    := 80000801h;
#let attributesSymbols["__int"]       := 80000803h;
#let attributesSymbols["__symbol"]    := 80000808h;
