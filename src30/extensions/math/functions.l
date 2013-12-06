#define system.
#define extensions.

// --- Functions ---

#symbol exp = &&:x
    [ (realValue:x)~EReal Exp ].

#symbol ln = &&:x
    [ (realValue:x)~EReal Ln ].
    
#symbol power = &&:x:y
    [ exp:((ln:(realValue:x)) * y) ].

#symbol round = &&:x
    [ (realValue:x)~EReal Rounded ].
    
#symbol abs = &&:x
  [ (realValue:x)~EReal Abs ].
