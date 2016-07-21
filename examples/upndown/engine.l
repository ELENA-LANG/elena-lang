// --- namespace shortcuts ---
#import system.
#import system'collections.
#import system'routines.

// --- GameMaster ---

#class GameMaster
{
    #field theExtension.
    #field thePlayers.
    
    #field(bool)theStarted.

    #constructor new
    [
        thePlayers := Queue new.
        
        theStarted := false.
    ]
        
    #constructor new : extension
        <= (new)
    [
        theExtension := extension.
    ]
    
    #method $appendPlayer:player
    [
        theStarted?
            [ #throw NotSupportedException new. ].
        
        thePlayers push:player.
    ]
    
    #method players =
    {
        append : player [ $owner $appendPlayer:player. ]
        
        enumerator => thePlayers.
        
        length => thePlayers.
    }.
    
    #method start
    [
        theStarted := true.
        
        thePlayers length counting run &each: x
        [
            thePlayers peek start.
            
            thePlayers shift:1.
        ].

//            thePlayerNumber := NewInt32Value::(thePlayers count).
//
//            #shift.
//
//            thePlayerList := 
//            {
//                count'get = thePlayerNumber.
//                
//                run : anAction
//                [
//                    ^ Scan::thePlayers run:anAction.
//                ]
//
//                seek : aPlayer
//                [
//                    #loop (player'PlayerValue::aPlayer != (thePlayers~EArray)@0)?
//                    [
//                        thePlayers seek &first_item:1.
//                    ].
//                ]
//
//                find : aPlayer
//                [
//                    ^ It::thePlayers find:player'PlayerValue::aPlayer content.
//                ]
//
//                __top'proceed
//                [
//                    thePlayers seek &first_item:1.
//                ]
//            }.
//            
//            self run.
    ]
    
    #method => theExtension.
}

//#static GameMaster = config'GameMasterGroup.
//
//// --- BaseMaster ---
//#class BaseMaster
//{
//    #field thePlayers.
//
//    #field theLastID.
//    #field thePlayerNumber.
//
//    #role NotStarted
//    {
//        #method $nextID
//        [
//            theLastID += 1.
//            
//            ^ NewInt32Value::theLastID.
//        ]
//
//        #method players'get =
//        {
//            += aPlayer
//            [
//                aPlayer group_member'append &player'id:(self $nextID).
//    
//                thePlayers += aPlayer group_content.
//            ]
//        }.
//
//        #method start
//        [
//        ]
//    }
//
//    #method new
//    [
//        #shift NotStarted.
//
//        thePlayers := Circle.
//        
//        theLastID := Integer << 0.
//    ]
//
//    #method players'get = thePlayerList.
//
//    #method __top = (thePlayers~EArray)@0.
//
//    #method game'proceed
//    [
//        // new round
//        self players run: aPlayer =>
//        [
//            #try aPlayer game'proceed:self.
//        ].
//    ]
//
//    #method pack'create = cards'Pack.
//
//    #method trump'set : aTrump
//    [
//        self players run: aPlayer =>
//        [
//            #try aPlayer set &trump:aTrump.
//        ].
//    ]
//}
