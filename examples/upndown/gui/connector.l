//// --- default namespaces ---
//#define std'basic'*.
////#define std'dictionary'*.
////#define std'patterns'*.
//#define gui'dictionary'*.
////s#define gui'common'*.
////#define ext'gui'controls'*.
//
//// --- namespace shortcuts ---
////#define dialogs'*  = gui'dialogs'*.
////#define ctrl'*     = std'patterns'*.
//#define forms'*    = gui'forms'*.
//#define controls'* = gui'controls'*.
//
////#define engine'*   = cardgame'engine'*.
////#define config'*   = cardgame'config'*.
////#define player'*   = cardgame'players'*.
////
////#define cardgame'dictionary'*.
//
//// --- private subjects
//#subject $onGameVariantChange.
//
////// --- Control IDs ---
////#symbol SecondPlayerID = 1.
////#symbol ThirdPlayerID  = 2.
////#symbol ForthPlayerID  = 3.
////#symbol GameVariantID  = 4.
////#symbol UserNameID     = 5.
//
//#symbol Stopping
//    = 'program stop. 
//
//// --- Dialog ---
//
//#class Dialog (forms'Dialog)
//{
//    // --- Controls ---
//    #field cmdQuit.
//    #field cmdStart.
//    #field cmdLoad.
//    #field cmdConnect.
//    #field cmdSearch.
//   	#field cmdAccept.
//    #field cmdDeny.
//    #field cmdShowStat.
//
//    #field rbgConnectType.
//    #field pnlClient.
//    #field pnlHost.
//    #field GroupBox1.
//    #field GroupBox2.
//	#field GameVariant.
//	#field PlayersPanel.
//
//    #field txtUserName.
//    #field txtPassword.
//	#field HostIP.
//	#field HostName.
//	#field SPLabel.
//    #field TPLabel.
//    #field FPLabel.
//    #field SecondPlayer.
//    #field ThirdPlayer.
//    #field FourthPlayer.
//
//    #field Label1.
//    #field Label2.
//	#field Label5.
//	#field NoConnectNotification.
//	#field ConnectingNotification.
//	#field ConnectNotification.
//
//    #method initializing : Args
//    [
//        super initializing:Args.
//
//        // --- Create controls ---
//        cmdQuit   := controls'Button::self.
//        cmdStart  := controls'Button::self.
//
//        rbgConnectType := controls'radioButtonGroup::self.
//		pnlClient := controls'Panel::self.
//		pnlHost   := controls'Panel::self.
//        GroupBox1 := controls'Panel::self.
//
//		Label1 := controls'StaticLabel::GroupBox1.
//		Label2 := controls'StaticLabel::GroupBox1.
//        txtUserName := controls'Edit::(GroupBox1/*~EInjector::EID::UserNameID*/).
//
//		txtPassword := controls'Edit::GroupBox1.
//
//        cmdSearch := controls'Button::pnlHost.
//		cmdAccept := controls'Button::pnlHost.
//		cmdDeny := controls'Button::pnlHost.
//		cmdShowStat := controls'Button::pnlHost.
//
//        cmdConnect := controls'Button::pnlClient.
//		GroupBox2 := controls'Panel::pnlClient.
//		Label5 := controls'StaticLabel::pnlClient.
//		HostIP := controls'Edit::pnlClient.
//
//		NoConnectNotification := controls'StaticLabel::GroupBox2.
//		ConnectingNotification := controls'StaticLabel::GroupBox2.
//		ConnectNotification := controls'StaticLabel::GroupBox2.
//		HostName := controls'StaticLabel::GroupBox2.
//        GameVariant := controls'radioButtonGroup::(pnlHost/*~EInjector::EID::GameVariantID*/).
//		PlayersPanel := controls'Panel::pnlHost.
//
//		SPLabel := controls'StaticLabel::PlayersPanel.						
//        TPLabel := controls'StaticLabel::PlayersPanel.
//		FPLabel := controls'StaticLabel::PlayersPanel.
//        SecondPlayer := controls'ComboBox::(PlayersPanel/*~EInjector::EID::SecondPlayerID*/).
//		ThirdPlayer  := controls'ComboBox::(PlayersPanel/*~EInjector::EID::ThirdPlayerID*/).
//		FourthPlayer := controls'ComboBox::(PlayersPanel/*~EInjector::EID::ForthPlayerID*/).
//
//        // Form
//        self
//            coordinates'set &x:100 &y:100 dimensions'set &width:560 &height:375
//            caption'set: "Up'N'Down: Establish connections with other players".
//
//        // cmdQuit
//        cmdQuit
//            coordinates'set &x:379 &y:322 dimensions'set &width:145 &height:25
//            caption'set: "Quit Game"
//            open.
//
//        // cmdStart
//        cmdStart
//            coordinates'set &x:13 &y:322 dimensions'set &width:145 &height:25
//            caption'set: "Start Game"
//            enabled'set:false
//            open.
//
//		// rbgConnectType
//        rbgConnectType
//            coordinates'set &x:7 &y:1 dimensions'set &width:522 &height:60
//            caption'set: " Begin game as... "
//            control_item'append &caption:"&Host" 
//            control_item'append &caption:"&Client"
//            enabled'set:false
//            open.
//
//        rbgConnectType << 0.
//
//		// GroupBox1
//        GroupBox1
//            coordinates'set &x:7 &y:62 dimensions'set &width:522 &height:49
//            caption'set: " Player specification "
//            open.
//
//		// --- pnlClient
//        pnlClient
//            coordinates'set &x:7 &y:110 dimensions'set &width:522 &height:207
//            caption'set: " Client panel ".
//
//		// --- pnlHost
//        pnlHost
//            coordinates'set &x:7 &y:110 dimensions'set &width:522 &height:207
//            caption'set: " Host panel "
//            open.
//
//		// Label1
//        Label1
//            coordinates'set &x:16 &y:20 dimensions'set &width:79 &height:20
//            caption'set: "Your Name:"
//            open.
//
//		// Label2
//        Label2
//            coordinates'set &x:263 &y:20 dimensions'set &width:95 &height:20
//            caption'set: "Your log-in:"
//            open.
//
//		// txtUserName
//        txtUserName
//            coordinates'set &x:98 &y:20 dimensions'set &width:157 &height:24
//            open.
//
//		// txtPassword
//        txtPassword
//            coordinates'set &x:377 &y:20 dimensions'set &width:115 &height:24
//            open.
//
//        // cmdSearch
//        cmdSearch
//            coordinates'set &x:300 &y:149 dimensions'set &width:145 &height:25
//            caption'set: "Search players"
//            enabled'set:false
//            open.
//
//		// cmdAccept
//        cmdAccept
//            coordinates'set &x:300 &y:178 dimensions'set &width:145 &height:25
//            caption'set: "Accept players"
//            enabled'set:false
//            open.
//
//		// cmdDeny
//        cmdDeny
//            coordinates'set &x:300 &y:178 dimensions'set &width:145 &height:25
//            caption'set: "Refuse players".
//
//		// cmdShowStat
//        cmdShowStat
//            coordinates'set &x:7 &y:176 dimensions'set &width:171 &height:25
//            caption'set: "Show All-Game Statistics".
//
//        // cmdConnect
//        cmdConnect
//            coordinates'set &x:290 &y:18 dimensions'set &width:177 &height:25
//            caption'set: "Try to connect"
//            enabled'set:false
//            open.
//
//		// GroupBox2
//        GroupBox2
//            coordinates'set &x:48 &y:48 dimensions'set &width:369 &height:105
//            caption'set: "Connection status "
//            open.
//
//		// Label5
//        Label5
//            coordinates'set &x:25 &y:22 dimensions'set &width:95 &height:20
//            caption'set: "Enter Host IP:"
//            open.
//
//		// HostIP
//        HostIP
//            coordinates'set &x:124 &y:20 dimensions'set &width:108 &height:24
//            open.
//
//		// NoConnectNotification
//        NoConnectNotification
//            coordinates'set &x:32 &y:21 dimensions'set &width:241 &height:20
//            caption'set: "Client not connected to anyone"
//            open.
//
//		// ConnectingNotification
//        ConnectingNotification
//            coordinates'set &x:32 &y:45 dimensions'set &width:241 &height:20
//            caption'set: "Client searching for any host".
//
//		// ConnectNotification
//        ConnectNotification
//            coordinates'set &x:32 &y:69 dimensions'set &width:241 &height:20
//            caption'set: "Client has connected with:".
//
//		// HostName
//        HostName
//            coordinates'set &x:244 &y:69 dimensions'set &width:73 &height:20
//            caption'set: "HostName".
//
//		// PlayersPanel
//        PlayersPanel
//            coordinates'set &x:215 &y:21 dimensions'set &width:274 &height:122
//            caption'set: "Choose players"
//            open.
//
//		// GameVariant
//        GameVariant
//            coordinates'set &x:10 &y:21 dimensions'set &width:191 &height:121
//            caption'set: " Choose game variant "
//            control_item'append &caption:"Two players"
//            control_item'append &caption:"Three players"
//            control_item'append &caption:"Four players"
//            open.
//            
//        GameVariant << 2.
//
//        SPLabel
//            coordinates'set &x:14 &y:25 dimensions'set &width:100 &height:20
//            caption'set: "Second player"
//            open.
//
//        TPLabel
//            coordinates'set &x:14 &y:54 dimensions'set &width:100 &height:20
//            caption'set: "Third player"
//            open.
//
//        FPLabel
//            coordinates'set &x:14 &y:85 dimensions'set &width:100 &height:20
//            caption'set: "Fourth player"
//            open.
//
//		// SecondPlayer
//        SecondPlayer
//            coordinates'set &x:122 &y:25 dimensions'set &width:141 &height:120
//            open.
//            
//		// ThirdPlayer
//        ThirdPlayer
//            coordinates'set &x:122 &y:54 dimensions'set &width:141 &height:129
//            open.
//
//		// FourthPlayer
//        FourthPlayer
//            coordinates'set &x:122 &y:85 dimensions'set &width:141 &height:120
//            open.
//
//        // event handlers		
//        cmdQuit event_handle'append:
//        { 
//            clicking : Args = self forms'dialog_result'set:#symbol Stopping.
//        }.
//        
////        rbgConnectType~eevents += 
////        {
////            onindex_changed &selected_index:anIndex = self $onConnectTypeChange:anIndex. 
////        }.
////
////        (SecondPlayer, ThirdPlayer, FourthPlayer)~egroupevents +=
////        { 
////            onChange'eval [ self $onPlayerSelect. ] 
////        }.
//
//        GameVariant event_handle'append:
//        {
//            index_changing &selected_index:anIndex = self $onGameVariantChange:anIndex. 
//        }.
////        cmdSearch~eevents +=
////        {
////            onClick = self $searchClients. 
////        }.
////        cmdStart~eevents +=
////        {
////            onClick'eval = self $startGame. 
////        }.
//////        #group(events'EHandler, cmdConnect) +=
//////        {
//////            ongui'Click = self $connectToServer. 
//////        }.
////        cmdAccept~eevents +=
////        {
////            onClick'eval = self $acceptPlayers. 
////        }.
////        cmdDeny~eevents +=
////        {
////            onClick'eval = self $denyPlayers. 
////        }.
//    ]
//
////    #method $onPlayerSelect 
////    [
////        #var aCheck := { 
////            if : aCombo
////            [
////                #if aCombo visible'is
////                    | [ ^ $self. ].
////
////                #var aText := aCombo selected_item.
////
////                ctrl'Control if:((aText length != 0) and:("Not responded" != aText)).
////            ]
////        }.
////
////        cmdAccept enabled'set:(aCheck if:SecondPlayer if:ThirdPlayer if:FourthPlayer back:true | back:false).
////    ]
////
////    #method $acceptPlayers
////    [
////        cmdStart set &enabled:true.
////        cmdAccept close.
////        cmdDeny open.
////    ]
////
//////    #method $onConnectTypeChange : anIndex
//////    [
//////        #if (anIndex != 0)?
//////        [
//////            pnlClient open.
//////            pnlHost close.
//////        ]
//////        |
//////        [
//////            pnlClient close.
//////            pnlHost open.
//////            
//////            GameVariant << 0.
//////        ].
//////    ]
//
//    #method $onGameVariantChange : anIndex
//    [
//        #if anIndex
//            ==0 ?
//            [
//                __cast(ThirdPlayer, TPLabel, FourthPlayer, FPLabel) close.
//            ]
//            | ==1 ?
//            [
//                __cast(ThirdPlayer, TPLabel) open.
//                __cast(FourthPlayer, FPLabel) close.
//            ]
//            | ==2 ?
//            [
//                __cast(ThirdPlayer, TPLabel, FourthPlayer, FPLabel) open.
//            ].
//    ]
//
////    #method $startGame
////    [
////        // add human player
////        engine'GameMaster players
////            += config'HostPlayerGroup::
////            {
////                playground = theParent.
////
////                player = player'Human::{ &player'name:txtUserName }.
////            }.
////
////        // add other players                                    
////        ctrl'Scan::(SecondPlayer, ThirdPlayer, FourthPlayer) run: aControl =>
////        [
////            #if aControl visible'is
////            [
////                #var aPlayerInfo := aControl selected_item control_tag.
////
////                engine'GameMaster players
////                    += config'ClientPlayerGroup::aPlayerInfo.
////            ].
////        ].
////
////        self set &forms'dialog_result: => (engine'GameMaster start | ~econtrol then: =>
////        [
////            dialogs'MessageBox::{ &dialogs'error_text:"The program flow is broken." &caption:"Up'N'Down" &control_parent:self } open &forms'modal.
////
////            'program stop.
////
////            ^ nil.
////        ]).
////    ]
//}