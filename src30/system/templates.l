//// ==== ver 3.4.2 ===
//
//#template(target:field) prop
//{
//    #method set : value
//    [
//        $target := value.
//    ]
//    
//    #method get = $target.
//}
//
//#template(target:method) vrnprop
//{
//   #method(stacksafe,type:int,embeddable,suppress:w3) get
//   [
//        #var(type:int)aLength.
//        $target eval &vint:aLength.
//        
//        ^ aLength.
//   ]
//}
