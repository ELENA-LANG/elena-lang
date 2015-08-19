// --- namespace shortcuts ---
#import system.
//
//#subject id, name, hand.

// --- BasePlayer ---

#class BasePlayer
{
//    #field theName.
//    #field theHand.
//    #field theTrump.
//
//    #method new & name : aPlayerName
//    [
//        theHand := cards'Hand.
//        theName := WideStrValue::aPlayerName.
//        theTrump := 0.
//    ]
//
//    #method name'get = theName.
//
//    #method hand =
//    {
//        count = theHand count.
//
//        @ aCard = (theHand@aCard) content.
//
//        run : anAction
//        [
//            #if (theHand count > 0)?
//            [
//                It::theHand run: aCard =>
//                [
//                    #if aCard?
//                        [ anAction evaluate:(aCard index). ].
//                ].
//            ].
//        ]
//    }.
//
//    #method trump'get = theTrump.
//
//    #method trump'set : aCard
//    [
//        theTrump := NewInt32Value::aCard.
//    ]
//
//    #method card'append : aCard
//    [
//        theHand@aCard << true.
//    ]
//
//    #method card'reduce : aCard
//    [
//        theHand@aCard << false.
//    ]
//
//    #method game'proceed
//    [
//        theHand clear.
//    
//        theTrump := 0.
//    ]
}

//// --- Human ---
//
//#class Human (Base)
//{
////    #method game'card'enum : anAction
////    [
////        #group(ctrl'EIndexEnum, theHand@0) run:anAction.
////    ]
//}
//
//// --- PlayerValue ---
//
//#symbol PlayerValue : thePlayer = Magnitude
//{
//    //ifequal &id : anID
//    ifequal : aParam
//    [
//        #var anID := aParam cardgame'players'id.
//        
//        anID ifequal:(thePlayer id).
//    ]
//}.
