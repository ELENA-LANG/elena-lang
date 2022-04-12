// --- aliasTypes ---

#new __symbol aliasTypes;

#let aliasTypes["object"] := system'Object;
#let aliasTypes["byte"]   := system'ByteNumber;
#let aliasTypes["int"]    := system'IntNumber;

#reload aliasTypes;