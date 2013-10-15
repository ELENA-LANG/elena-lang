//// --- namespace shortcuts ---
//#define std'basic'*.
//#define std'patterns'*.
////#define std'basic'factories'*.
////#define ext'patterns'*.
//#define gui'common'*.
//#define ext'convertors'*.
//#define ext'gui'controls'*.
//
//#define forms'*    = gui'forms'*.
//#define controls'* = gui'controls'*.
//#define graphics'* = gui'graphics'*.
//
//#define main'*     = cardgame'gui'main'*.
//#define player'*   = cardgame'players'*.
//#define cards'*    = cardgame'cards'*.
//
//#define std'dictionary'*.
//#define gui'dictionary'*.
//#define gui'dictionary'events'*.
//
//#define cardgame'dictionary'upndown'*.
//#define cardgame'dictionary'*.
//
//// --- private constants ---
//
//#symbol tblScaleX = 70.
//#symbol tblScaleY = 17.
//
//#symbol tblXCoord = 630.
//#symbol tblYCoord = 80.
//
//#symbol tblHeight = 17.
//
//// --- TricksDlg ---
//
//#class TricksDlg (forms'Dialog)
//{
//    #field theLabel1.
//    #field theTrickNumber.
//    #field cmOK.
//    #field theUpDown.
//
//    #role Modal
//    {
//        #method forms'dialog_result'set : aValue
//        [
//            self order'save:aValue.
//
//            theResult := aValue.
//
//            theParent enabled'set:true.
//            
//            self close.
//
//            #shift.
//        ]
//    }
//
//    #method oninit : Args
//    [
//        super oninit:Args.
//
//        theLabel1 := controls'StaticLabel::self.
//        theTrickNumber := controls'Edit::self.
//        theUpDown := controls'UpDown::self.
//        cmOK := controls'Button::self.
//
//        cmOK~eevents +=
//            { onClick'eval = self $onClick. } .
//
//        self
//            set &x:199 &y:186 set &width:364 &height:85
//            set &caption: "Ordering".
//
//        theLabel1
//            set &x:12 &y:8 set &width:160 &height:34
//            set &caption:"Choose number of cards you are going to take"
//            open. 
//
//        theTrickNumber
//            set &x:192 &y:13 set &width:36 &height:24
//            open. 
//            
//        theTrickNumber << "0".
//        
//        cmOK
//            set &x:274 &y:13 set &width:81 &height:25
//            set &caption:"OK"
//            open. 
//
//        theUpDown
//            set &x:228 &y:13 set &width:19 &height:24
//            open. 
//            
//        theUpDown range'set &from:0 &to:(self card_count).
//    ]
//
//    #method $onClick
//    [
//        #var aRetVal := LiteralToInt::theTrickNumber.
//
//        #if self set &forms'dialog_result:aRetVal
//            | [ ].
//    ]
//}
//
//// --- TagInjection ---
//
//#symbol TagInjection : aTag =
//    TagHolder::aTag.
//
//// --- JokerDlg ---
//
//#class JokerDlg (forms'Dialog)
//{
//    #field cmOK.
//    #field cmCancel.
//    #field rbBiggest.
//    #field rbSmallestDiamond.
//    #field rbSmallestHeart.
//    #field rbSmallestClub.
//    #field rbSmallestSpade.
//    
//    #field theCard.
//    
//    #method $setOffSuit : aSuit
//    [
//        #if aSuit
//            ifequal:cards'Diamonds [ rbSmallestDiamond enabled'set:false. ]
//            | ifequal:cards'Clubs [ rbSmallestClub enabled'set:false. ]
//            | ifequal:cards'Hearts [ rbSmallestHeart enabled'set:false. ]
//            | ifequal:cards'Spades [ rbSmallestSpade enabled'set:false. ].
//    ]
//    
//    #method $setOffBiggest
//    [
//        rbBiggest enabled'set:false. 
//    ]
//    
//    #method oninit : Args
//    [
//        super onInit:Args.
//
//        cmOK := controls'Button::self.
//        cmCancel := controls'Button::self.
//        
//        rbBiggest := controls'RadioButton::(self~EInjector::TagInjection::cards'Big_Card).
//        
//        rbSmallestDiamond := controls'RadioButton::(self~EInjector::TagInjection::cards'Diamonds_Min).
//        rbSmallestHeart := controls'RadioButton::(self~EInjector::TagInjection::cards'Hearts_Min).
//        rbSmallestClub := controls'RadioButton::(self~EInjector::TagInjection::cards'Clubs_Min).
//        rbSmallestSpade := controls'RadioButton::(self~EInjector::TagInjection::cards'Spades_Min).
//
//        self
//            set &x:291 &y:184 set &width:284 &height:144
//            set &caption: "How do you want to use it?".
//            
//        cmOK
//            set &x:166 &y:23 set &width:105 &height:25
//            set &caption:"OK"
//            open. 
//            
//        cmCancel
//            set &x:166 &y:55 set &width:105 &height:25
//            set &caption:"Cancel"
//            open.
//            
//        rbBiggest
//            set &x:23 &y:7 set &width:120 &height:17
//            set &caption:"Biggest"
//            checked'set:true
//            open.
//            
//        theCard := cards'Big_Card.
//            
//        rbSmallestDiamond
//            set &x:23 &y:48 set &width:135 &height:17
//            set &caption:"Smallest Diamond"
//            open.
//            
//        rbSmallestHeart
//            set &x:23 &y:27 set &width:120 &height:17
//            set &caption:"Smallest Heart"
//            open.
//            
//        rbSmallestClub
//            set &x:23 &y:68 set &width:120 &height:17
//            set &caption:"Smallest Club"
//            open.
//            
//        rbSmallestSpade
//            set &x:23 &y:89 set &width:120 &height:17
//            set &caption:"Smallest Spade"
//            open.
//            
//        cmOK~eevents +=
//            { onClick'eval = self set &forms'dialog_result:theCard. } .
//            
//        cmCancel~eevents +=
//            { onClick'eval = self set &forms'dialog_result:0. } .
//
//
//        (rbBiggest, rbSmallestDiamond, rbSmallestHeart, rbSmallestClub, rbSmallestSpade)~egroupevents +=
//        {
//            onclick &sender:aButton
//            [
//                self $setCard:(aButton control_tag).
//            ]
//        }.
//    ]
//    
//    #method $setCard : aCard
//    [
//        theCard := aCard.
//    ]
//}
//
//// --- Score --
//
//#class Score
//{
//    #field theOrder.
//    #field theTaken.
//    #field theScore.
//
//    #method $ordered = theOrder.
//    #method $taken = theTaken.
//    #method $score = theScore.
//    
//    #method new
//    [
//        theOrder := Integer::0.
//        theTaken := Integer::0.
//        theScore := Integer::0.
//    ]
//    
//    #method $clear
//    [
//        theOrder << 0.
//        theTaken << 0.
//    ]
//}
//
//// --- ScroeTable ---
//
//#class ScoreTable
//{
//    #field theTable.
//
//    #field theYOffset.
//    #field theXOffset.
//    
//    #method new : Players
//    [
//        #var aCount := Players count.
//        #var aUpCount := 36 / aCount.
//        #var aRoundNumber := aUpCount * 2 + 10.
//        
//        // adjust x coordinate
//        theXOffset := Integer << tblXCoord.
//        theXOffset -= ((aCount - 1) * tblScaleX).        
//        theYOffset := Integer << 0.
//
//        theTable := NewInitializedArray &&std'math'matrix'm:(aCount + 1) &std'math'matrix'n:(aRoundNumber + 2).
//
//        It::Players run: aPlayer => (theTable refer:0 refer &indexer:(1 + aPlayer index) set &content:(aPlayer player'name)).
//
//        loop &&for:1 &till:aUpCount run: anIndex =>
//        [
//            theTable refer:anIndex refer &indexer:0 set &content:("Up" + IntToLiteral::anIndex).
//            
//            theTable refer:(aUpCount * 2 - anIndex) refer &indexer:0 set &content:("Dn" + IntToLiteral::anIndex).
//        ].
//
//        theTable refer:aUpCount refer &indexer:0 set &content:"Top".
//        
//        It::("Dk1", "Dk2", "Dk3", "Tl1", "Tl2", "Tl3", "Gd1", "Gd2", "Gd3") run: anItem =>
//        [
//            theTable refer:(aUpCount * 2 + anItem index) refer &indexer:0 set &content:(anItem literal).
//        ].
//        
//        It::(theTable @ (theTable count - 1)) run: anItem =>
//        [
//            #if 0 ifequal:(anItem index) 
//            | [
//                anItem set &content:Integer::0.
//            ].
//        ].
//    ]
//
//    #method @ aRoundNumber =
//    {
//        @ aPlayerIndex =
//        {
//            << aScore
//            [
//                theTable @ (theTable count - 1) @ (1 + aPlayerIndex) += aScore.
//
//                theTable refer:aRoundNumber refer &indexer:(1 + aPlayerIndex) set &content:(IntToLiteral::aScore).
//
//                #if (theYOffset + tblHeight <= aRoundNumber)?
//                [
//                    theYOffset << aRoundNumber - tblHeight + 1.
//                ].            
//            ]
//        }.
//    }.
//    
//    #method canvas'read : aCanvas
//    [
//        aCanvas set &graphics'brush:'graphics'Backgroundbrush.
//        aCanvas set &graphics'font_name:"Courier New" &height:14.
//        
//        // header
//        It::(theTable@0) run: aHeader =>
//        [
//            #if aHeader content isnot &nillable
//            [
//                aCanvas write &graphics'text:aHeader &x:(theXOffset + tblScaleX * aHeader index) &y:tblYCoord.
//            ].
//        ].
//
//        // body
//        Loop &&for:1 &to:tblHeight run: anIndex =>
//        [
//            #var aRoundIndex := theYOffset + anIndex.
//            #var aRow := theTable@aRoundIndex.
//  
//            It::aRow run : aCell =>
//            [
//                #var aValue := aCell content.
//                #if aValue isnot &nillable
//                [
//                    aCanvas write &graphics'text:aValue
//                        &x:(theXOffset + tblScaleX * aCell index + 40)
//                        &y:(tblYCoord + anIndex * tblScaleY).
//                ].
//            ].
//        ].
//        
//        // summary
//        It::(theTable @ theTable count subtract:1) run : aScore =>
//        [
//            #if 0 ifequal:(aScore index)
//            | [
//                aCanvas write &graphics'text:(IntToLiteral::aScore)
//                    &x:(theXOffset + tblScaleX * (aScore index) + 50)
//                    &y:(tblYCoord + tblScaleY*(1 + tblHeight)).
//            ].
//        ].
//    ]
//}
//
//// --- EHumanPlayer ---
//
//#class EHumanPlayer
//{
//    #field theForm.
//    #field theLeadingCard.
//    #field theScoreTable.
//    #field theRoundNumber.
//
//    #role FirstCard
//    {
//        #method onDeskCard &card:aCard &player:aPlayer
//        [
//            theLeadingCard << aCard.
//            
//            $self $checkJoker:aCard.
//            
//            $next onDeskCard:$param.
//            #shift.
//        ]
//    }
//
//    #role GoldMode
//    {
//        #method $showScore : aPlayer
//        [
//            #var aCaption := aPlayer player'name + " (" + IntToLiteral::(aPlayer $taken) + ")".
//    
//            aPlayer main'player_label set &caption:aCaption.
//                
//            theForm refresh.
//        ]
//    }
//
//    #method $checkJoker : aCard
//    [
//        #if aCard
//            ifequal:cards'Big_Joker [ theForm set &main'turn_Status:"Biggest joker". ]
//            | ifequal:cards'Big_Card [ theForm set &main'turn_Status:"Biggest card". ]
//            | ifequal:cards'Diamonds_Min [ theForm set &main'turn_Status:"Smallest diamond". ]
//            | ifequal:cards'Hearts_Min [ theForm set &main'turn_Status:"Smallest heart". ]
//            | ifequal:cards'Clubs_Min [ theForm set &main'turn_Status:"Smallest club". ]
//            | ifequal:cards'Spades_Min [ theForm set &main'turn_Status:"Smallest spade". ].            
//    ]
//
//    #method new : aProfile
//    [
//        theForm := aProfile playground.
//        theLeadingCard := Integer::0.
//        theRoundNumber := Integer::0.
//    ]
//
//    #method order'save : anAgent
//    [
//        theForm set &main'player_status:"Please make an order".
//
//        #var aDialog := TricksDlg
//        { 
//            card_count = anAgent card_count.
//
//            order'save : aResult 
//            [ 
//                #if anAgent order'write:aResult
//                | [  
//                    theForm set &main'player_status:"Cannot make this order".
//                    'program wait:300.
//                    theForm set &main'player_status:"Please make an order".
//                    
//                    $self fail.
//                ].
//            ] 
//        }::theForm.
//
//        aDialog open &forms'modal.
//        
//        aDialog free.
//    ]
//
//    #method main'player_label'create : aPlayer = ($next create &main'player_label:aPlayer)~Score.
//
//    #method $showScore : aPlayer
//    [
//        #var aCaption := aPlayer player'name + " (" + IntToLiteral::(aPlayer $taken) + " / "
//            + IntToLiteral::(aPlayer $ordered) + ")".
//
//        aPlayer main'player_label set &caption:aCaption.
//            
//        theForm refresh.
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
//    #method game'proceed : aRound
//    [
//        theRoundNumber += 1.
//
//        // clear the scores
//        self players run: aPlayer =>
//        [
//            aPlayer $clear.
//            
//            aPlayer main'player_label set &caption:(aPlayer player'name).
//        ].
//        
//        theForm refresh.
//        
//        $next game'proceed:aRound.
//
//        #if aRound is &goldstage
//        [
//            #shift GoldMode.
//        ]
//        | [ #shift FirstCard. ].
//    ]
//
//    #method turn'proceed : aRound
//    [        
//        theForm set &main'turn_status:"".
//
//        $next proceed &turn:aRound.
//    ]
//
//    #method onPlayerOrder : anOrderer
//    [
//        #var aPlayer := self players find:anOrderer.
//        
//        aPlayer $ordered << anOrderer order.
//        
//        self $showScore:aPlayer.
//    ]
//
//    #method card'save : aMaster
//    [
//        $next card'save:#join (aMaster)
//        {
//            card'write : aCard
//            [
//                #var aTrump := aMaster trump.
//                #if Control if:(aCard == cards'Joker) if:(aTrump != 0)
//                [
//                    #var aDialog := JokerDlg::theForm.
//                    aDialog $setOffSuit:(cards'Suit::aTrump).
//                    #if (theLeadingCard != 0)?
//                    [
//                        #if (cards'Suit::theLeadingCard != cards'Suit::aTrump)?
//                        [
//                            #if (cards'Suit::theLeadingCard != cards'Diamonds)?
//                                [ aDialog $setOffSuit:cards'Diamonds. ].
//                            #if (cards'Suit::theLeadingCard != cards'Clubs)?
//                                [ aDialog $setOffSuit:cards'Clubs. ].
//                            #if (cards'Suit::theLeadingCard != cards'Hearts)?
//                                [ aDialog $setOffSuit:cards'Hearts. ].
//                            #if (cards'Suit::theLeadingCard != cards'Spades)?
//                                [ aDialog $setOffSuit:cards'Spades. ].
//                                
//                            #if (cards'BigestCard::(cards'Suit::aTrump) find:(self player'hand) != 0)?
//                                [ aDialog $setOffBiggest. ].
//                        ]
//                        | ! [
//                            aDialog $setOffSuit:cards'Diamonds.
//                            aDialog $setOffSuit:cards'Clubs.
//                            aDialog $setOffSuit:cards'Hearts.
//                            aDialog $setOffSuit:cards'Spades.
//                        ].
//                    ].
//                    
//                    #var aJoker := aDialog open &forms'modal.
//                    
//                    aDialog free.
//                    
//                    #if (aJoker == 0)?
//                        [ $self fail. ] | [ ^ aMaster card'write:aJoker. ].
//                ]
//                | [ ^ aMaster card'write:aCard. ].
//            ]
//        }.
//    ]
//
//    #method onDeskCard &card:aCard &player:aPlayer
//    [
//        theLeadingCard << 0.
//        
//        $self $checkJoker:aCard.
//        
//        $next onDeskCard:$param.
//    ]
//
//    #method winner'proceed : aPlayer
//    [
//        #var aPlayerInfo := self players find:aPlayer.
//
//        aPlayerInfo $taken += 1.
//        
//        self $showScore:aPlayerInfo.
//        
//        $next winner'proceed:aPlayer.
//    ]
//    
//    #method game'start &players : Players
//    [
//        $next start &game:$param.
//
//        theScoreTable := ScoreTable::Players.
//
//        theForm~eevents +=
//        {
//            main'onPlaygroundPaint : aCanvas
//            [
//                theScoreTable read &canvas:aCanvas.
//            ]
//        }.
//    ]
//    
//    #method onPlayerScore : aPlayer
//    [
//        #var anInfo := self players find:aPlayer.
//        
//        #var aLastScore := anInfo $score.
//        #var aCurrentScore := aPlayer cardgame'dictionary'upndown'score - aLastScore.
//        
//        aLastScore += aCurrentScore.
//        
//        theScoreTable@theRoundNumber@(anInfo index) << aCurrentScore.
//    ]
//}
