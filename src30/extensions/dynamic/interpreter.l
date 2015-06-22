////#define system.
////#define extensions.
//////#define system'dynamic.
////
////#subject(class:system'BaseFunction1) ScriptError.
////#subject(class:system'LiteralValue)  script.
////
////// --- IOException ---
////
////#symbol $lastScriptError = control do:
////[
////    #var(type:chararray,size:256)aBuffer.
////    #var(type:int)aLength := system'external'elenasm GetStatus
////        &chararray:aBuffer
////        &int:255.
////        
////    ^ LiteralValue new &length:aLength &chararray:aBuffer.
////].
////
////#class ScriptEngineException :: Exception
////{
////    #constructor new
////        <= %new &message:$lastScriptError &error_type: %ScriptError.
////        
////    #constructor new &message:aMessage
////        <= %new &message:aMessage &error_type: %ScriptError.
////}
////
////// --- scriptEngine ---
////
////#symbol scriptEngine =
////{
////    evaluate &script:aScript
////        => system'internal'evaluate.
////        
////    load &path:aPath
////        <= %load &path:aPath &int:0 &int:-1.
////        
////    load &path:aPath &int:anEncoding &int:anAutoDetect
////        => system'internal'load.
////}.
////
//////// --- globlas ---
//////
//////#static globals = DynamicStruct new.