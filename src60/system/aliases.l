// --- aliasTypes ---

#new __symbol aliasTypes;

#let aliasTypes["object"]  := system'Object;
#let aliasTypes["bool"]    := system'BoolValue;
#let aliasTypes["byte"]    := system'ByteNumber;
#let aliasTypes["sbyte"]   := system'SByteNumber;
#let aliasTypes["short"]   := system'ShortNumber;
#let aliasTypes["ushort"]  := system'UShortNumber;
#let aliasTypes["int"]     := system'IntNumber;
#let aliasTypes["uint"]    := system'UIntNumber;
#let aliasTypes["long"]    := system'LongNumber;
#let aliasTypes["real"]    := system'RealNumber;
#let aliasTypes["char"]    := system'CharValue;
#let aliasTypes["string"]  := system'String;
#let aliasTypes["wide"]    := system'WideString;
#let aliasTypes["message"] := system'Message;
#let aliasTypes["handle"]  := system'Handle;
#let aliasTypes["pointer"] := system'UnsafePointer;

#reload aliasTypes;