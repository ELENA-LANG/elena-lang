// --- aliasTypes ---

#new __symbol aliasTypes;

#let aliasTypes["object"]  := system'Object;
#let aliasTypes["bool"]    := system'BoolValue;
#let aliasTypes["byte"]    := system'ByteNumber;
#let aliasTypes["short"]   := system'ShortNumber;
#let aliasTypes["int"]     := system'IntNumber;
#let aliasTypes["char"]    := system'CharValue;
#let aliasTypes["string"]  := system'String;
#let aliasTypes["message"] := system'Message;

#reload aliasTypes;