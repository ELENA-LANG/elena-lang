//// --- default namespaces ---
//#define std'dictionary'*.
//#define std'basic'*.
//#define std'patterns'*.
//#define std'collections'*.
//#define sys'dynamics'*.
////#define ext'patterns'*.
//#define ext'convertors'*.
//
//// --- namespace shortcuts ---
//#define utils'*     = ext'utils'*.
//#define engine'*    = cardgame'engine'*.
//#define player'*    = cardgame'players'*.
//#define cards'*     = cardgame'cards'*.
//
//#define cardgame'dictionary'*.
//#define cardgame'dictionary'upndown'*.
//
//// protected subjects
//#subject __distributor.
//#subject __order, __taken, __score.
//
//// private operations
//#subject __next_distributor, __beforeRound, __afterRound.
//
//// --- PlayerInfo ---
//
//#class PlayerInfo
//{
//    #field theOrder.
//    #field theTaken.
//    #field theScore.
//    
//    #method new
//    [
//        theOrder := 0.
//        theTaken := Integer::0.
//        theScore := Integer::0.
//    ]
//
//    #method order'get = theOrder.
//    #method taken'get = NewInt32Value::theTaken.
//    #method score'get = NewInt32Value::theScore.
//
//    #method __order'set : anOrder
//    [
//        theOrder := NewInt32Value::anOrder.
//    ]
//
//    #method __taken'append : aTaken
//    [
//        theTaken += aTaken.
//    ]
//
//    #method __score'append : aScore
//    [
//        theScore += aScore.
//    ]
//
//    #method game'proceed : aRound
//    [
//        theOrder := 0.
//        theTaken << 0.
//        
//        $next proceed &game:aRound.
//    ]
//}
//
//// --- OrderValidator ---
//
//#class OrderValidator
//{
//    #field theCardnumber.
//    #field thePlayernumber.
//    #field theTotal.
//
//    #role LastOrder
//    {
//        #method order'write : anOrder
//        [
//            (anOrder <= theCardnumber)?.
//            
//            (theTotal + anOrder != theCardnumber)?.
//            
//            theTotal += anOrder.
//        ]
//    }
//    
//    #method card_count = theCardnumber.
//
//    #method new : aRound
//    [
//        theTotal := Integer::0.
//        theCardnumber := aRound card_count.
//        thePlayernumber := Integer << aRound players count.
//    ]
//
//    #method order'write : anOrder
//    [
//        (anOrder <= theCardnumber)?.
//        
//        theTotal += anOrder.
//        
//        thePlayernumber -= 1.
//        
//        #if (thePlayernumber == 1)?
//            [ #shift LastOrder. ].
//    ]
//}
//
//// --- DeskValidator ---
//
//#class DeskValidator
//{
//    #field theTrump.
//    #field theCards.
//    #field theTrumpSuit.
//
//    #role Empty
//    {
//        #method clear []
//
//        #method append &player:aPlayer &card:aCard
//        [
//            #if (aCard == cards'Big_Card)?
//                [ 
//                    theCards += { card = cards'Big_Joker. player = aPlayer. } . 
//                ]
//                | [ theCards += $param. ].
//            
//            #shift.
//        ]
//    }
//
//    #method append &player:aPlayer &card:aLastCard
//    [
//        #var aHand := aPlayer player'hand.
//        #var aLeaderCard := theCards first_item content card.        
//
//        #if (aLeaderCard == cards'Big_Joker)?
//        [
//            // if no trump can put
//            // else - only if it is the bigest trump
//            #if theTrumpSuit if &player'hand:aHand
//            [
//                (aLastCard == cards'BigestCard::theTrumpSuit find:aHand)?.
//            ].
//        ]
//        | [
//            #var aLeadingSuit := cards'Suit::aLeaderCard.
//            
//            // if the bigest card
//            #if (aLastCard == cards'Big_Card)?
//            | [
//                #var aCarsSuit := cards'Suit::aLastCard.
//            
//                // if the same suit can put it
//                #if (aCarsSuit == aLeadingSuit)or:(aCarsSuit == theTrumpSuit)or:(aLastCard == cards'Big_Card)?
//                | [
//                    // make sure the user doesn't have any card of the leading suit
//                    #if aLeadingSuit if &player'hand:aHand
//                    [
//                        $self fail.
//                    ].
//                    
//                    // make sure the user doesn't have any trump card
//                    #if theTrumpSuit if &player'hand:aHand
//                    [
//                        $self fail.
//                    ].
//                ].
//            ].
//        ].
//        
//        theCards += $param.
//    ]
//
//    #method clear
//    [
//        theCards clear.
//        
//        #shift Empty.
//    ]
//
//    #method trump = theTrump.
//
//    #method new : aTrump
//    [
//        theTrump := aTrump.
//        theCards := List.
//        theTrumpSuit := cards'Suit::aTrump.
//
//        #shift Empty.
//    ]
//
//    #method run : anAction
//    [
//        ^ Scan::theCards run:anAction.
//    ]
//}
//
//// --- WinnerSearch ---
//
//#class WinnerSearch
//{
//    #field theWinner.
//    #field theOldestSuit.
//    #field theTrumpSuit.
//    
//    #role Found
//    {
//        #method evaluate []
//    }
//    
//    #role Leading
//    {
//        #method evaluate : aPlayer
//        [
//            theWinner := aPlayer.
//
//            #if (aPlayer card == cards'Big_Joker)?
//                [ #shift Found. ]
//                | [ 
//                    theOldestSuit := cards'Suit::(aPlayer card).
//                    
//                    #shift. 
//                ] .
//        ]
//    }
//    
//    #method evaluate : aPlayer
//    [
//        #var aCard := aPlayer card.
//        #if (aCard == cards'Big_Joker) or:(aCard == cards'Big_Card)?
//        [ 
//            theWinner := aPlayer.
//
//            #shift Found.
//
//            ^ $self.
//        ].
//
//        #if Control if:(theOldestSuit == cards'Suit::aCard)
//        [
//            #if (theWinner card < aCard)?
//                [ theWinner := aPlayer. ].
//        ]
//        | if:(cards'Suit::aCard == theTrumpSuit)
//        [
//            theWinner := aPlayer.
//            theOldestSuit := theTrumpSuit.
//        ].
//    ]
//    
//    #method new : aTrump
//    [
//        #shift Leading.
//        
//        theTrumpSuit := cards'Suit::aTrump.
//    ]
//    
//    #method winner = theWinner player.
//}
//
//// --- BaseRound ---
//
//#class BaseRound
//{
//    #field theCardNumber.
//    #field theDistributor.
//
//    #method card_count = NewInt32Value::theCardNumber.
//
//    #method distributor = theDistributor.
//
//    #method __distributor'set : aPlayer
//    [
//        theDistributor := self players find:aPlayer.
//
//        self players seek:theDistributor.
//    ]
//
//    #method __distributor =
//    {
//        load : aPack
//        [
//            loop::{ &for:1 &to:theCardNumber } run: =>
//            [
//                self players run: aPlayer =>
//                [
//                    aPlayer append &card:(aPack get).
//                ].
//            ].
//        ]
//    }.
//
//    #method new
//    [
//        theCardNumber := Integer::0.
//        theDistributor := nil.
//    ]
//}
//
//// --- NormalRound ---
//#class NormalRound (BaseRound)
//{
//    #field theTrump.
//    #field theMaximalCardNumber.
//    #field theRoundNumber.
//
//    #role Up
//    {
//        #method game'proceed
//        [
//            self __next_distributor.
//
//            theCardNumber += 1.
//
//            #if (theCardNumber == theMaximalCardNumber)?
//            [ 
//                $self start &topstage.
//            ].
//
//            $next game'proceed.
//
//            ^ true.
//        ]
//
//        #method round_name = "Up(" + IntToLiteral::theCardNumber + ")".
//    }
//
//    #role Top
//    {
//        #method round_name = "Top(" + IntToLiteral::theCardNumber + ")".
//        
//        #method game'proceed
//        [
//            self start &downstage.
//            
//            self __next_distributor.
//
//            $next game'proceed.
//            
//            ^ true.
//        ]
//    }
//
//    #role Down
//    {
//        #method round_name = "Down(" + IntToLiteral::theCardNumber + ")".
//        
//        #method game'proceed
//        [
//            self __next_distributor.
//            
//            #if (theCardNumber == 1)?
//            [ 
//                $self start &darkstage.
//            ]
//            | [ theCardNumber -= 1. ].
//
//            $next game'proceed.
//            
//            ^ true.
//        ]
//    }
//
//    #role Dark
//    {
//        #method round_name = "Dark(" + IntToLiteral::theCardNumber + ")".
//        
//        #method game'proceed
//        [
//            self __next_distributor.
//
//            #if (theRoundNumber == 3)?
//            [ 
//                $self start &trumplessstage.
//            ]
//            | [ theRoundNumber += 1. ].
//            
//            $next game'proceed.
//
//            ^ true.
//        ]
//        
//        #method __beforeRound
//        [
//            // make orders
//            self proceed &order.
//        
//            #var aPack := self create &pack.
//            
//            // distribute cards
//            self __distributor load:aPack.    
//            
//            // show trump
//            #var aTrump := aPack get | last.
//            
//            self set &trump:aTrump.
//    
//            self players proceed &engine'__top.
//        ]
//        
//        #method score'proceed : aPlayer
//        [
//            #var anOrder := aPlayer order.
//            #var aTaken := aPlayer taken.
//            
//            #if control 
//                if:(anOrder == aTaken)
//                [
//                    #if(anOrder == 0)?
//                        [ aPlayer append &__score:10. ]
//                        | [ aPlayer append &__score:(aTaken*20). ].
//                ]
//                | if:(anOrder < aTaken)
//                [
//                    aPlayer append &__score:(aTaken * 2).
//                ]
//                | [ aPlayer append &__score:(anOrder*(-20)). ].
//
//            
//            self players run: aNotifier => (aNotifier onPlayerScore:aPlayer).
//        ]
//    }
//
//    #role Trumpless
//    {
//        #method round_name = "Trumpless(" + IntToLiteral::theCardNumber + ")".
//        
//        #method __beforeRound
//        [
//            #var aPack := self create &pack.
//            
//            // distribute cards
//            self __distributor load:aPack.    
//                
//            // make orders
//            self proceed &order.
//        
//            self players proceed &engine'__top.
//        ]
//        
//        #method game'proceed
//        [
//            self __next_distributor.
//
//            #if (theRoundNumber == 3)?
//            [ 
//                $self start &goldstage.
//            ]
//            | [ theRoundNumber += 1. ].
//            
//            $next game'proceed.
//            
//            ^ true.
//        ]
//        
//        #method score'proceed : aPlayer
//        [
//            #var anOrder := aPlayer order.
//            #var aTaken := aPlayer taken.
//            
//            #if Control 
//                if:(anOrder == aTaken)
//                [
//                    #if(anOrder == 0)?
//                        [ aPlayer append &__score:10. ]
//                        | [ aPlayer append &__score:(aTaken*20). ].
//                ]
//                | if:(anOrder < aTaken)
//                [
//                    aPlayer append &__score:(aTaken * 2).
//                ]
//                | [ aPlayer append &__score:(anOrder*(-20)). ].
//
//            self players run: aNotifier => (aNotifier onPlayerScore:aPlayer).
//        ]        
//    }
//
//    #role Gold
//    {
//        #method goldstage'is []
//
//        #method round_name = "Gold(" + IntToLiteral::theCardNumber + ")".
//
//        #method __beforeRound
//        [
//            #var aPack := self create &pack.
//            // distribute cards
//            self __distributor load:aPack.
//            
//            // show trump
//            #var aTrump := aPack get | last.
//            
//            self set &trump:aTrump.
//        
//            self players proceed &engine'__top.
//        ]
//        
//        #method game'proceed
//        [
//            self __next_distributor.
//
//            #if (theRoundNumber == 3)?
//            [
//                ^ false.
//            ]
//            | [ theRoundNumber += 1. ].
//            
//            $next game'proceed.
//            
//            ^ true.
//        ]
//        
//        #method score'proceed : aPlayer
//        [
//            #var aTaken := aPlayer taken.
//            
//            aPlayer append &__score:(aTaken*20).
//
//            self players run: aNotifier => (aNotifier onPlayerScore:aPlayer).
//        ]        
//    }
//
//    #method score'proceed : aPlayer
//    [
//        #var anOrder := aPlayer order.
//        #var aTaken := aPlayer taken.
//        
//        #if Control 
//            if:(anOrder == aTaken)
//            [
//                #if(anOrder == 0)?
//                    [ aPlayer append &__score:5. ]
//                    | [ aPlayer append &__score:(aTaken*10). ].
//            ]
//            | if:(anOrder < aTaken)
//            [
//                aPlayer append &__score:aTaken.
//            ]
//            | [ aPlayer append &__score:(anOrder*(-10)). ].
//
//        self players run: aNotifier => (aNotifier onPlayerScore:aPlayer).
//    ]
//
//    #method new : anOwner
//    [
//        super new.
//
//        theTrump := Integer::0.
//        
//        theMaximalCardNumber := 36 / anOwner players count.
//        theRoundNumber := Integer::0.
//    ]
//
//    #method trump'get = NewInt32Value::theTrump.
//
//    #method trump'set : aTrump
//    [
//        theTrump << aTrump.
//
//        $next set &trump:aTrump.
//    ]
//
//    #method __next_distributor
//    [
//        self players seek:theDistributor.
//        
//        self players proceed &engine'__top.
//
//        self set &__distributor:(self engine'__top).
//    ]
//
//    #method upstage'start
//    [
//        #shift Up.
//
//        theCardNumber << 0.
//    ]
//
//    #method topstage'start
//    [
//        theCardNumber << theMaximalCardNumber.
//        
//        #shift Top.
//    ]
//    
//    #method downstage'start
//    [
//        theCardNumber << theMaximalCardNumber - 1.
//        
//        #shift Down.
//    ]
//    
//    #method darkstage'start
//    [
//        theRoundNumber << 1.
//        theCardNumber << theMaximalCardNumber.
//        
//        #shift Dark.
//    ]
//    
//    #method trumplessstage'start
//    [
//        theRoundNumber << 1.
//        theCardNumber << theMaximalCardNumber.
//        theTrump << 0.
//        
//        #shift Trumpless.
//    ]
//    
//    #method goldstage'start
//    [
//        theRoundNumber << 1.
//        theCardNumber << theMaximalCardNumber.
//        
//        #shift Gold.
//    ]
//    
//    #method order'proceed
//    [
//        #var aValidator := OrderValidator::self.
//        self players run: aPlayer =>
//        [
//            aPlayer order'save:#join(aValidator) 
//            { 
//                order'write : anOrder 
//                [
//                    aValidator write &order:anOrder.
//
//                    aPlayer set &__order:anOrder.
//
//                    self players run: aNotifier => [ #try aNotifier onPlayerOrder:aPlayer. ].
//                ] 
//            }.
//        ].
//    ]
//    
//    #method __beforeRound
//    [
//        #var aPack := self create &pack.
//        // distribute cards
//        self __distributor load:aPack.
//
//        // show trump
//        #var aTrump := aPack get | last.
//        
//        self set &trump:aTrump.
//
//        // make orders
//        self proceed &order.
//        
//        self players proceed &engine'__top.
//    ]
//
//    #method round'proceed
//    [
//        self __beforeRound.
//
//        // play round
//        Loop &&for:1 &to:(self card_count) run: =>
//        [
//            #var aWinner := self proceed &turn.
//
//            self players run: aPlayer => (aPlayer proceed &winner:aWinner).
//
//            self players find:aWinner append &__taken:1.
//
//            self players seek:aWinner.
//
//            'program wait:2000.
//        ].
//
//        self __afterRound.
//    ]
//
//    #method turn'proceed
//    [
//        self players run: aPlayer => (aPlayer proceed &turn:self).
//
//        // play card round
//        #var aDesk := DeskValidator::theTrump.
//
//        self players run: aPlayer =>
//        [
//            aPlayer card'save:
//            {
//                trump = theTrump.
//
//                card'write : aCard
//                [
//                    #var aCardInfo := { &player:aPlayer &card:aCard }.
//                    aDesk += aCardInfo.
//
//                    aPlayer reduce &card:aCard.
//
//                    self players run: aNotifier => (aNotifier onDeskCard:aCardInfo).
//                ]
//            }.
//        ].
//
//        // calculate turn
//        #var anAction := WinnerSearch::theTrump.
//        aDesk run:anAction.
//        
//        ^ anAction winner.
//    ]
//    
//    #method __afterRound
//    [
//        self players run: aPlayer =>
//        [
//            self proceed &score:aPlayer.
//        ].
//    ]
//
//    #method game'stop
//    [
//        #var aWinner := Variable.
//        self players run: aPlayer =>
//        [
//            #if aWinner content nillable'isnot
//            [
//                #if (aWinner score < aPlayer score)?
//                [
//                    aWinner new:aPlayer. 
//                ].
//            ]
//            | [ aWinner new:aPlayer. ].
//        ].
//        
//        self players run: aNotifier => [ #try aNotifier stop &game:(aWinner content). ].
//    ]
//}
//
//// --- UpRound ---
//
//#symbol UpRound : anOwner = NormalRound::anOwner start &upstage.
//
//// --- EGame ---
//
//#role EGame
//{
//    #method players'get
//    [
//        #var thePlayers := $next players.
//        
//        ^ #join (thePlayers)
//        {
//            += aPlayer
//            [
//                aPlayer insert &group_member:PlayerInfo.
//                
//                thePlayers += aPlayer.
//            ]
//        }.
//    ]
//
//    #method game'start
//    [
//        $next game'start.
//
//        #var anID := 1 + utils'RandomValue::(self players count).
//
//        self~egroup group_member'replace &from:$self &to:(UpRound::self).
//
//        self __distributor'set &player'id:anID.
//    ]
//    
//    #method run
//    [
//        self start &game.
//
//        // play rounds
//        #loop (self game'proceed)?
//        [
//            self round'proceed.
//        ].
//        
//        self stop &game.
//    ]
//}
