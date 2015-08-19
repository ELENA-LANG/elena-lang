// --- namespace shortcuts ---
#import system.
#import forms.

//// --- gui subjects ---
//#subject player_labels, player_desk.
//
//#subject player_label/*, player_card*/.
//#subject player_status, turn_status.
//
//// --- gui events ---
//
//#subject onPlayGroundPaint, onDeskHit.
//
////// --- PlayerInfo ---
////
////#class PlayerInfo
////{
////    #field theID.    
////    #field theName.    
////    #field theLabel.
////
////    #method player'id = theID.
////    
////    #method player'name = theName.
////
////    #method player_label = theLabel.
////
////    #method new : aPlayer
////    [
////        theID := NewInt32Value::(aPlayer player'id).
////        theName := WideStrValue::(aPlayer player'name).
////        theLabel := aPlayer player_label.
////
////        theLabel set &caption:theName.
////    ]
////
//////    #annex theLabel.
////}
////
////// === Constants ===
////
////#symbol StatusBarID = 10h.
////
////#symbol hndShift = 23.
////#symbol hndLeft  = 32.
////#symbol hndTop   = 295.
////
////#symbol trmCoord = { x = 11. y = 88. }.
////
////#static tblCoords = 
////( 
////    { x = 216. y = 150. },
////    { x = 192. y = 112. },
////    { x = 232. y = 80. },
////    { x = 256. y = 128. }
////).
////
//////// --- EJokerAdapter --
//////
//////#symbol Diamonds_Min= -1.
//////#symbol Hearts_Min=   -2.
//////#symbol Clubs_Min=    -3.
//////#symbol Spades_Min=   -4.
//////
//////#symbol Big_Joker=    37.
//////#symbol Big_Card=     38.
////
////#symbol EJokerAdapter =
////{
////    @ aCard
////    [
////        #if aCard < 0 ? [ ^ $next @ cards'Joker. ]
////            | [ ^ $next @ aCard. ].
////    ]
////}.

// --- Playground ---

#class Playground
{
    #field theForm.
    
    #field fff.
    #field ddd.
    
////    #field theStatusbar.
////    #field lblPlayer1.
////    #field lblPlayer2.
////    #field lblPlayer3.
////    #field lblPlayer4.
////
////    #field thePaintbox.
////    #field theImageList.
////
////    #field theImageSize.
////    #field theHandRect.

    #constructor new
    [
        theForm := SDIDialog new.
        
//        super initializing:Args.
//
//////        #group(events'EHandler, 'system) +=
//////        {
//////            on'Application'Error'do [ self $onError. ] 
//////        }.
////        
////        theStatusbar := controls'Statusbar::(self~EInjector::EID::StatusBarID). 
////
////        lblPlayer1 := controls'StaticLabel::self.
////        lblPlayer2 := controls'StaticLabel::self.
////        lblPlayer3 := controls'StaticLabel::self.
////        lblPlayer4 := controls'StaticLabel::self.
////
////        thePaintbox := controls'Paintbox::self.
////        theImageList := __group(EJokerAdapter, controls'ImageList::self). // !! probably it should extended in upndown'gui module, because it is game specific
////
////        #if self $loadImageList
////            | [ dialogs'MessageBox::{ &dialogs'error_text:"Cannot load image files" &caption:"Up'N'Down" &control_parent:self } run. $self fail. ].
        
        theForm
            set &x:50 &y:50 
            set &width:800 &height:520
            set &caption:"Up'N'Down 0.9".

////        thePaintbox
////            set &x:0 &y:0 set &width:800 &height:500
////            open.
////
////        theStatusbar
////                    set &x:0 &y:467 set &width:700 &height:20.
////            
////        theStatusBar
////            append &control_item &caption:"Up'N'Down V. 0.4"
////            append &control_item &caption:""
////            append &control_item &caption:""
////            append &control_item &caption:"".
////
////        (theStatusBar @ 0) set &width:104.
////        (theStatusBar @ 1) set &width:284.
////        (theStatusBar @ 2) set &width:464.
////        (theStatusBar @ 3) set &width:690.
////
////        theStatusbar open.
////        
////        lblPlayer1
////            set &x:182 &y:433 set &width:200 &height:15.
//
        theForm
            set &onOpen:aSender [ Connector new &parent:aSender run. ].

////        thePaintbox~eevents +=
////        {
////            onPaint &canvas:aCanvas [ theEvents onPlayGroundPaint:aCanvas. ]
////
////            onButton_Down &location:aLocation &processed:aFlag
////            [ 
////                #if theHandRect if &location:aLocation
////                    [ theEvents onDeskHit:aLocation. ].
////
////                aFlag << true.
////            ]
////        }.             
    ]

////    #method player_labels = (lblPlayer1,lblPlayer2,lblPlayer3,lblPlayer4).
////
////    #method $loadImageList
////    [
////        theImageList += "..\img\Mask.bmp".
////        theImageList += "..\img\diamond_6.bmp".
////        theImageList += "..\img\diamond_7.bmp".
////        theImageList += "..\img\diamond_8.bmp".
////        theImageList += "..\img\diamond_9.bmp".
////        theImageList += "..\img\diamond_10.bmp".
////        theImageList += "..\img\diamond_jack.bmp".        
////        theImageList += "..\img\diamond_queen.bmp".
////        theImageList += "..\img\diamond_king.bmp".
////        theImageList += "..\img\diamond_ace.bmp".
////
////        theImageList += "..\img\club_6.bmp".
////        theImageList += "..\img\club_7.bmp".
////        theImageList += "..\img\club_8.bmp".
////        theImageList += "..\img\club_9.bmp".
////        theImageList += "..\img\club_10.bmp".
////        theImageList += "..\img\club_jack.bmp".
////        theImageList += "..\img\club_queen.bmp".
////        theImageList += "..\img\club_king.bmp".
////        theImageList += "..\img\club_ace.bmp".
////
////        theImageList += "..\img\heart_6.bmp".
////        theImageList += "..\img\heart_7.bmp".
////        theImageList += "..\img\heart_8.bmp".
////        theImageList += "..\img\heart_9.bmp".
////        theImageList += "..\img\heart_10.bmp".
////        theImageList += "..\img\heart_jack.bmp".
////        theImageList += "..\img\heart_queen.bmp".
////        theImageList += "..\img\heart_king.bmp".
////        theImageList += "..\img\heart_ace.bmp".
////
////        theImageList += "..\img\spade_6.bmp".
////        theImageList += "..\img\spade_7.bmp".
////        theImageList += "..\img\spade_8.bmp".
////        theImageList += "..\img\spade_9.bmp".
////        theImageList += "..\img\spade_10.bmp".
////        theImageList += "..\img\spade_jack.bmp".
////        theImageList += "..\img\spade_queen.bmp".
////        theImageList += "..\img\spade_king.bmp".
////        theImageList += "..\img\spade_ace.bmp".
////        
////        // jokers
////        theImageList += "..\img\club_6.bmp".
////        theImageList += "..\img\club_6.bmp".
////
////        theImageSize := (theImageList@0) gui'dictionary'size.
////        theHandRect := Rectangle &&x:hndLeft &y:hndTop &width:(hndShift * 18) &height:(theImageSize height).
////    ]
////
//////    #method $onError
//////    [
//////        #var anError := basic'Integer.
//////        
//////        #external win32'71(anError).
//////    ]

////    #method $setPlayerNumber : aPlayerNumber
////    [
////        lblPlayer1 open.
////
////        #if aPlayerNumber 
////            == 2 ?
////            [
////                lblPlayer2
////                    set &x:201 &y:8 set &width:200 &height:15
////                    open.
////            ]
////            | == 3 ?
////            [
////                lblPlayer2
////                    set &x:45 &y:18 set &width:200 &height:15
////                    open.
////
////                lblPlayer3
////                    set &x:341 &y:18 set &width:200 &height:15
////                    open.
////            ]
////            | == 4 ?
////            [
////                lblPlayer2
////                    set &x:13 &y:32 set &width:200 &height:15
////                    open.
////
////                lblPlayer3
////                    set &x:201 &y:8 set &width:200 &height:15
////                    open.
////                    
////                lblPlayer4
////                    set &x:357 &y:32 set &width:200 &height:15
////                    open.
////            ].
////    ]
////
////    #method cardgame'dictionary'playground'save : aCanvas
////    [
////        // Drawing hand
////        #var aHand := self player'hand.
////        #var aCount := Integer << 9 - (aHand count / 2).
////        aHand run:aCard =>
////        [
////            aCanvas
////                write &graph'image:(theImageList@aCard) &x:(hndLeft + aCount * hndShift) &y:hndTop.
////            
////            aCount += 1.
////        ].
////
////        // Drawing trump card
////        #var aTrump := self trump.
////        #if (0 != aTrump)?
////        [
////            aCanvas
////                write &graph'image:(theImageList@aTrump) &location:trmCoord.
////        ].
////
////        // Drawing table cards
////        Scan::(self player_desk) run: aCard =>
////        [
////            #var aCoord := tblCoords @ aCard index.
////
////            aCanvas
////                write &graph'image:(theImageList@aCard) &location:aCoord.
////        ].
////    ]
////
////    #method onDeskHit : Params
////    [        
////        #var anX := Params x - hndLeft.
////        #var aWidth := theImageSize width.
////    
////        #var aHand := self player'hand.
////        #var aCount := Integer::9 -= (aHand count / 2).
////        #var aSelectedCard := Integer::0.
////        aHand run: aCard =>
////        [
////            #var aCardX := aCount * hndShift.
////            #if ((aCardX < anX)and:(aCardX + aWidth > anX))?
////            [
////                aSelectedCard << aCard.
////            ].
////            
////            aCount += 1.
////        ].
////        #if (aSelectedCard != 0)?
////        [
////            self set &card:aSelectedCard.
////            
////            self refresh.
////        ].
////    ]
////
////    #method player_status'set : aStatus
////    [
////        (theStatusbar @ 2) set &caption:aStatus.
////        
////        self refresh.
////    ]
////
////    #method turn_status'set : aStatus
////    [
////        (theStatusbar @ 3) set &caption:aStatus.
////        
////        self refresh.
////    ]

    #method => theForm.
}

////// --- EPlayer ---
////#class EPlayer
////{
////    #field theForm.
////    #field thePlayers.
////    #field theDesk.
////
////    #field theCurrentCard.
////
////    #role CurrentTurn
////    {
////        #method card'set : aCard
////        [
////            theCurrentCard << aCard.
////        ]
////                    
////        #method card'wait : aMaster
////        [
////            #if (theCurrentCard != 0)?
////            [
////                #if aMaster write &card:theCurrentCard
////                    [ ^ false. ]
////                | [
////                    theCurrentCard << 0.
////                    
////                    theForm set &player_status:"Cannot play with this card".
////                    'program wait:500.
////                    theForm set &player_status:"Your turn".
////                ].
////            ].
////            
////            ^ true.
////        ]
////    }
////
////    #method new : aProfile
////    [
////        theForm := aProfile cardgame'dictionary'playground.
////        theCurrentCard := Integer::0.
////    ]
////
////    #method player_desk = theDesk.
////
////    #method players'get =
////    {
////        find : aPlayer
////        [
////            ^ It::thePlayers find:player'PlayerValue::aPlayer.
////        ]
////
////        run : anAction
////        [
////            ^ Scan::thePlayers run:anAction.
////        ]
////    }.
////
////    #method player_label'create : aPlayer = PlayerInfo::aPlayer.
////
////    #method game'start &players : Players
////    [
////        #var anExtendedForm := theForm~self.
////
////        theForm~eevents +=
////        {
////            onPlaygroundPaint : aCanvas = anExtendedForm save &cardgame'dictionary'playground:aCanvas. 
////
////            onDeskHit : anArg = anExtendedForm onDeskHit:anArg.
////        }.
////
////        theDesk := List.
////
////        #var Labels := theForm player_labels.
////        #var aCount := Players count.
////        theForm $setPlayerNumber:aCount.
////
////        thePlayers := NewInitializedArray &&factory'init_action: anItem =>
////        [
////            #var aPlayer := Players~earray @ anItem index.
////            #var aLabel := Labels~earray @ anItem index.
////
////            ^ self player_label'create:(aPlayer~{ player_label = aLabel. }).
////        ] &count:aCount.
////
////        theForm refresh.
////    ]
////
////    #method game'proceed : aRound
////    [
////        $next game'proceed:aRound.
////
////        #var aDistributor := self players find:(aRound distributor).
////        
////        // clear the table
////        theDesk clear.
////
////        #var aStatusBar := theForm~EAccessor @ StatusBarID.
////        (aStatusBar@0) set &caption:(aRound round_name).
////
////        #if (0 == aDistributor index)?
////            [  (aStatusBar @ 1) set &caption:"you are distributor". ]
////            | [ (aStatusBar @1) set &caption:(aDistributor player'name + " is distributor"). ].
////
////        theForm refresh.
////    ]
////
////    #method card'append : aCard
////    [
////        $next card'append:aCard.
////        
////        theForm refresh.
////    ]
////
////    #method onDeskCard &card:aCard &player:aPlayer
////    [
////        #var anIndex := Int32Value::(self players find:aPlayer index).
////
////        theDesk += #join (aCard) { index = anIndex. }.
////
////        theForm refresh.
////    ]
////
////    #method trump'set : aCard
////    [
////        $next set &trump:aCard.
////        
////        theForm refresh.
////    ]
////
////    #method card'save : aMaster
////    [
////        theForm set &player_status:"Your turn".
////        
////        theCurrentCard << 0.
////        
////        #shift CurrentTurn.
////        
////        #loop ($self wait &card:aMaster)?
////        [
////            'program proceed.
////        ].
////        
////        theForm set &player_status:"Waiting for others".
////    ]
////    
////    #method winner'proceed : aPlayer
////    [
////        #var aPlayerInfo := self players find:aPlayer.
////
////        #if (0 == aPlayerInfo index)?
////            [  theForm set &player_status:"You are a winner!". ]
////            | [ theForm set &player_status:(WideStrValue::(aPlayerInfo player'name) + " is a winner!"). ].
////            
////        theForm refresh.
////    ]
////    
////    #method turn'proceed
////    [
////        // clear the desk
////        theDesk clear.
////        
////        theForm refresh.
////    ]
////
////    #method game'stop : aPlayer
////    [
////        #var anInfo := self players find:aPlayer.
////
////        #if (0 == anInfo index)?
////            [  theForm set &player_status:"You are a game winner!". ]
////            | [ theForm set &player_status:(anInfo player'name + " is a game winner!"). ].
////                
////        // clear the table
////        theDesk clear.
////        
////        theForm refresh.
////    ]
////}
//
//// --- Connector ---
//
//#symbol Connector : aParent = config'ConnectorGroup group_member'append:connector'dialog::aParent.