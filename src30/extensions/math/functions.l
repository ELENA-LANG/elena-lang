#define system.
//#define extensions.

// --- Functions ---

#symbol exp = (:x)
    [ x~mathOp Exp ].

#symbol ln = (:x)
    [ x~mathOp Ln ].
    
#symbol power = (:x:y)
    [ exp:((ln:x) * y) ].

#symbol round = (:x)
    [ x~mathOp Rounded ].
    
#symbol abs = (:x)
  [ x~mathOp Abs ].

#symbol modulus = (:x:y)
    [ x - (x / y * y) ].
