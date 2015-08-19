// --- namespace shortcuts ---
#define system.
//
//#symbol JokerOptions = (cards'Big_Card, cards'Diamonds_Min, cards'Hearts_Min, cards'Clubs_Min, cards'Spades_Min).
//
//// --- EConnector ---
//
//#symbol EConnector =
//{
//    gui'forms'modal'open
//    [
//        (self~EAccessor @ connector'SecondPlayerID) append &control_item &caption:"Chaotic AI" &control_tag:ai'EChaoticPlayerInfo::"Chaotic 1".
//        (self~EAccessor @ connector'ThirdPlayerID) append &control_item &caption:"Chaotic AI" &control_tag:ai'EChaoticPlayerInfo::"Chaotic 2".
//        (self~EAccessor @ connector'ForthPlayerID) append &control_item &caption:"Chaotic AI" &control_tag:ai'EChaoticPlayerInfo::"Chaotic 3".
//
//        ^ $next open &gui'forms'modal.
//    ]
//}.
//
//// --- EChaoticPattern ---
//
//#symbol EChaoticPlayerInfo : aName =
//{
//    player = __group(ai'EChaotic, player'Base::{ &player'name:aName }).
//}.
//
//// --- Chaotic ---
//
//#role EChaotic
//{
//    #method turn'proceed []
//
//    #method winner'proceed []
//    
////    #method score'proceed []
//
//    #method onDeskCard : Params []
//
//    #method onPlayerScore : Params []
//
//    #method order'save : anAgent
//    [
//        #var aFlag := Boolean << true.
//        
//        #loop aFlag?
//        [
//            #if anAgent order'write:(utils'RandomValue::4)
//                [ aFlag << false. ].
//        ].
//    ]
//
//    #method card'reduce : aCard
//    [
//        #if aCard 
//            < 0? [ $next card'reduce:cards'Joker. ]
//            | > cards'Spades_Ace? [ $next card'reduce:cards'Joker. ]
//            | [ $next card'reduce:aCard. ].
//    ]
//
//    #method card'save : aDesk
//    [
//        #var aHand := self player'hand.
//        #loop true?
//        [
//            #var aCard := 1 + utils'RandomValue::36.
//            
//            #if (aHand@aCard)?
//            [
//                #if Control if:(aCard == cards'Joker) if:(aDesk trump != 0)
//                [
//                    aCard := JokerOptions@utils'RandomValue::(JokerOptions count).
//                ].
//                
//                #if aDesk card'write:aCard
//                    [ ^ self. ].
//            ].
//        ].
//    ]
//}
//
////// --- Chaotic ---
////
////#class EChaotic2 (EChaotic)
////{
////    #method order'save : anAgent
////    [
////        ^ $next order'save:anAgent.
////    ]
////}

// --- aiConfigurator ---

#symbol aiConfigurator =
{
    configConnector : aConnector
    [
        aConnector addPlayer:(upndown'BasePlayer new).
    ]
}.