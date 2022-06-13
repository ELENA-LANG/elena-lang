// --- aliasTypes ---

#new __symbol aliasTypes;

#let aliasTypes["object"] := system'Object;
#let aliasTypes["bool"]   := system'BoolValue;
#let aliasTypes["byte"]   := system'ByteNumber;
#let aliasTypes["int"]    := system'IntNumber;
#let aliasTypes["string"] := system'String;

#reload aliasTypes;