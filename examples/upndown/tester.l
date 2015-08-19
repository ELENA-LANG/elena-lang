//// --- namespace shortcuts ---
//#define gui'dictionary'*.
//#define ext'gui'controls'*.
//
//#define connector'* = cardgame'gui'connector'*.
////#define cards'*     = cardgame'cards'*.
//
//// --- JokerHandler ---
////
////#class JokerHandler
////{
////    #field thePack.
////
////    #role JokerSecond
////    {
////        #method card'get
////        [
////            $self $setJokerOnTop.
////            
////            #var aCard := thePack card'get.
////            
////            #if (aCard == cards'Joker)?
////            [
////                aCard := thePack card'get.
////            ].
////            
////            ^ aCard.
////        ]
////    }
////                
////    #role JokerOnTop
////    {
////        #method card'get
////        [
////            #shift.
////            
////            ^ cards'Joker.
////        ]
////    }
////    
////    #method card'get
////    [
////        #var aCard := thePack card'get.
////        
////        #if (aCard == cards'Joker)?
////        [
////            aCard := thePack card'get.
////        ].
////        
////        ^ aCard.
////    ]
////    
////    #method new : aPack
////    [
////        thePack := aPack.
////        
////        #shift JokerSecond.
////    ]
////    
////    #method $setJokerOnTop
////    [
////        #shift JokerOnTop.
////    ]
////    
////    #annex thePack.
////}
////
////// --- JokerTester ---
////
////#symbol EJokerTester =
////{
////    pack'get = JokerHandler::(self~enext pack'get).
////}.
//
//// --- EConnector ---
//
//#symbol EConnector =
//{
//    gui'forms'modal'open
//    [
//        (self~EAccessor @ connector'SecondPlayerID) set &selected_index:0.
//        (self~EAccessor @ connector'ThirdPlayerID) set &selected_index:0.
//        (self~EAccessor @ connector'ForthPlayerID) set &selected_index:0.
//        
//        self~EAccessor @ connector'UserNameID << "human".
//        
//        self~EAccessor @ connector'GameVariantID << 2.
//
//        ^ $next open &gui'forms'modal.
//    ]
//}.
// --- aiConfigurator ---

#symbol testConfigurator =
{
    configConnector : aConnector
    [
    ]
}.