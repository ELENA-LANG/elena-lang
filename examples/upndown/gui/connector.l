// --- default namespaces ---
#import system.
#import system'routines.
#import extensions.
#import forms.

#import upndown.

// --- Dialog ---

#class(extension)$helper
{
    #method(bool)ifPlayerSelected
    [
        self is &visible
            ? [
                #var text := self selected key.
                
                ^ text is &nonempty and:("Not responded" != text).
            ].
            
        ^ true.
    ]
}

#class Connector
{
    #field theForm.
    #field theUI.
    
    // --- Controls ---
    #field cmdQuit.
    #field cmdStart.
    #field cmdLoad.
    #field cmdConnect.
    #field cmdSearch.
   	#field cmdAccept.
    #field cmdDeny.
    #field cmdShowStat.

    #field rbgConnectType.
    #field pnlClient.
    #field pnlHost.
    #field GroupBox1.
    #field GroupBox2.
	#field GameVariant.
	#field PlayersPanel.

    #field txtUserName.
    #field txtPassword.
	#field HostIP.
	#field HostName.
	#field SPLabel.
    #field TPLabel.
    #field FPLabel.
    #field SecondPlayer.
    #field ThirdPlayer.
    #field FourthPlayer.

    #field Label1.
    #field Label2.
	#field Label5.
	#field NoConnectNotification.
	#field ConnectingNotification.
	#field ConnectNotification.

    #constructor new &parent:aParent
    [
        theUI := aParent.
        
        theForm := ChildForm new &parent:aParent.

        // --- Create controls ---
        cmdQuit   := Button new.
        cmdStart  := Button new.

        rbgConnectType := RadioButtonGroup new.
        pnlClient := Panel new.
        pnlHost   := Panel new.
        GroupBox1 := Panel new.

		Label1 := StaticLabel new.
		Label2 := StaticLabel new.
        txtUserName := Edit new.
        txtPassword := Edit new.
        
        cmdSearch   := Button new.
        cmdAccept   := Button new.
        cmdDeny     := Button new.
        cmdShowStat := Button new.

        cmdConnect := Button new.
		GroupBox2 := Panel new.
		Label5 := StaticLabel new.
		HostIP := Edit new.

		NoConnectNotification := StaticLabel new.
		ConnectingNotification := StaticLabel new.
		ConnectNotification := StaticLabel new.
		HostName := StaticLabel new.
        GameVariant := RadioButtonGroup new.
		PlayersPanel := Panel new.

		SPLabel := StaticLabel new.						
        TPLabel := StaticLabel new.
		FPLabel := StaticLabel new.
        SecondPlayer := Combobox new.
		ThirdPlayer  := Combobox new.
		FourthPlayer := Combobox new.
    
        PlayersPanel controls
            append:SPLabel
            append:TPLabel
            append:FPLabel
            append:SecondPlayer
            append:ThirdPlayer
            append:FourthPlayer.

        GroupBox2 controls
            append:NoConnectNotification
            append:ConnectingNotification
            append:ConnectNotification
            append:HostName.
    
        GroupBox1 controls
            append:Label1
            append:txtUserName
            append:Label2
            append:txtPassword.

        pnlHost controls
            append:GameVariant
            append:PlayersPanel
            append:cmdSearch
            append:cmdAccept
            append:cmdDeny
            append:cmdShowStat.

        pnlClient controls
            append:cmdConnect
            append:GroupBox2
            append:Label5
            append:HostIP.
                
        theForm controls 
            append:rbgConnectType
            append:GroupBox1
            append:pnlHost
            append:pnlClient
            append:cmdStart
            append:cmdQuit.

        // Form
        theForm
            set &x:100 &y:100 
            set &width:560 &height:375
            set &caption: "Up'N'Down: Establish connections with other players".

        // cmdQuit
        cmdQuit
            set &x:379 &y:312 
            set &width:145 &height:25
            set &caption: "Quit Game".

        // cmdStart
        cmdStart
            set &x:13 &y:312 
            set &width:145 &height:25
            set &caption: "Start Game"
            set &enabled:false.

		// rbgConnectType
        rbgConnectType
            set &x:7 &y:1 
            set &width:522 &height:60
            set &caption: " Begin game as... "
            items
                append:"&Host"
                append:"&Client".

        rbgConnectType set &selected_index:0.

		// GroupBox1
        GroupBox1
            set &x:7 &y:62 
            set &width:522 &height:49
            set &caption: " Player specification ".

		// --- pnlClient
        pnlClient
            set &x:7 &y:110 
            set &width:522 &height:197
            set &caption: " Client panel "
            set &visible:false.

		// --- pnlHost
        pnlHost
            set &x:7 &y:110 
            set &width:522 &height:197
            set &caption: " Host panel ".

		// Label1
        Label1
            set &x:16 &y:20 
            set &width:79 &height:20
            set &caption: "Your Name:".

		// Label2
        Label2
            set &x:263 &y:20 
            set &width:95 &height:20
            set &caption: "Your Password:".

		// txtUserName
        txtUserName
            set &x:98 &y:20 
            set &width:157 &height:24.

		// txtPassword
        txtPassword
            set &x:377 &y:20 
            set &width:115 &height:24.

        // cmdSearch
        cmdSearch
            set &x:300 &y:139 
            set &width:145 &height:25
            set &caption: "Search players"
            set &enabled:false.

		// cmdAccept
        cmdAccept
            set &x:300 &y:168 
            set &width:145 &height:25
            set &caption: "Accept players"
            set &enabled:false.

		// cmdDeny
        cmdDeny
            set &x:300 &y:168 
            set &width:145 &height:25
            set &caption: "Refuse players"
            set &visible:false.

		// cmdShowStat
        cmdShowStat
            set &x:7 &y:168
            set &width:171 &height:25
            set &caption: "Show All-Game Statistics"
            set &visible:false.

        // cmdConnect
        cmdConnect
            set &x:290 &y:18 
            set &width:177 &height:25
            set &caption: "Try to connect"
            set &enabled:false.

		// GroupBox2
        GroupBox2
            set &x:48 &y:48 
            set &width:369 &height:105
            set &caption: "Connection status ".

		// Label5
        Label5
            set &x:25 &y:22 
            set &width:95 &height:20
            set &caption: "Enter Host IP:".

		// HostIP
        HostIP
            set &x:124 &y:20 
            set &width:108 &height:24.

		// NoConnectNotification
        NoConnectNotification
            set &x:32 &y:21 
            set &width:241 &height:20
            set &caption: "Client not connected to anyone".

		// ConnectingNotification
        ConnectingNotification
            set &x:32 &y:45 
            set &width:241 &height:20
            set &caption: "Client searching for any host"
            set &visible:false.

		// ConnectNotification
        ConnectNotification
            set &x:32 &y:69 
            set &width:241 &height:20
            set &caption: "Client has connected with:"
            set &visible:false.

		// HostName
        HostName
            set &x:244 &y:69 
            set &width:73 &height:20
            set &caption: "HostName"
            set &visible:false.

		// PlayersPanel
        PlayersPanel
            set &x:215 &y:18
            set &width:274 &height:118
            set &caption: "Choose players".

		// GameVariant
        GameVariant
            set &x:10 &y:18 
            set &width:191 &height:118
            set &caption: " Choose game variant "
            items
                append:"Two players"
                append:"Three players"
                append:"Four players".
            
        GameVariant set &selected_index:2.

        SPLabel
            set &x:14 &y:25 
            set &width:100 &height:20
            set &caption: "Second player".

        TPLabel
            set &x:14 &y:54 
            set &width:100 &height:20
            set &caption: "Third player".

        FPLabel
            set &x:14 &y:85 
            set &width:100 &height:20
            set &caption: "Fourth player".

		// SecondPlayer
        SecondPlayer
            set &x:122 &y:25 
            set &width:141 &height:120.
            
		// ThirdPlayer
        ThirdPlayer
            set &x:122 &y:54 
            set &width:141 &height:129.

		// FourthPlayer
        FourthPlayer
            set &x:122 &y:85 
            set &width:141 &height:120.

        // event handlers		
        cmdQuit set &onClick: args
            [ theForm close. 'program stop. ].

        rbgConnectType set &onIndexChanged: 
            args [ $owner $onConnectTypeChange:(rbgConnectType selected_index). ].

        txtUserName set &onChange:
            args [ $owner $onUserNameChange. ].

        #var onChange := (&onIndexChange:sender &int:index)
        [
            $owner $onPlayerSelect.
        ].

        SecondPlayer set &index_changed:onChange.
        ThirdPlayer set &index_changed:onChange.
        FourthPlayer set &index_changed:onChange.

        GameVariant set &onIndexChanged: 
            args [ $self $onGameVariantChange:(GameVariant selected_index). ].
            
//        cmdSearch~eevents +=
//        {
//            onClick = self $searchClients. 
//        }.
        cmdStart onClick := sender
        [
            $owner $startGame. 
        ].
//        #group(events'EHandler, cmdConnect) +=
//        {
//            ongui'Click = self $connectToServer. 
//        }.
        cmdAccept onClick := sender
        [
            $owner $acceptPlayers. 
        ].
        cmdDeny onClick := sender
        [
            $owner $denyPlayers. 
        ].

        theForm onOpen := sender
        [
            connectorGroup configConnector:
            {
                addPlayer : aPlayer
                [
                    $owner $addPlayer:aPlayer.
                ]
                
                SecondPlayer = SecondPlayer.
                
                ThirdPlayer  = ThirdPlayer.
                
                ForthPlayer  = FourthPlayer.
                
                GameVariant  = GameVariant.
                
                UserName     = txtUserName.
            }.
        ].
    ]
    
    #method $addPlayer : aPlayerInfo
    [
        SecondPlayer items append:aPlayerInfo &as:(aPlayerInfo caption).
        ThirdPlayer items append:aPlayerInfo &as:(aPlayerInfo caption).
        FourthPlayer items append:aPlayerInfo &as:(aPlayerInfo caption).
    ]
    
    #method $onUserNameChange
    [
        cmdSearch enabled := txtUserName value is &nonempty.
    ]

    #method $onPlayerSelect 
    [
        cmdAccept enabled := (SecondPlayer ifPlayerSelected) and:(ThirdPlayer ifPlayerSelected) and:(FourthPlayer ifPlayerSelected)
    ]

    #method $acceptPlayers
    [
        cmdStart enabled := true.
        cmdAccept visible := false.
        cmdDeny visible := true.
        
        SecondPlayer enabled := false.
        ThirdPlayer enabled := false.
        FourthPlayer enabled := false.
        
        rbgConnectType enabled := false.
        GameVariant enabled := false.
        
        txtUserName enabled := false.
        txtPassword enabled := false.
    ]
    
    #method $denyPlayers
    [
        cmdStart enabled := false.
        cmdAccept visible := true.
        cmdDeny visible := false.
        
        SecondPlayer enabled := true.
        ThirdPlayer enabled := true.
        FourthPlayer enabled := true.
        
        rbgConnectType enabled := true.
        GameVariant enabled := true.
        
        txtUserName enabled := true.
        txtPassword enabled := true.
    ]

    #method $onConnectTypeChange : anIndex
    [
        (anIndex != 0)
            ? [
                pnlClient visible := true.
                pnlHost visible := false.
              ]
            ! [
                pnlClient visible := false.
                pnlHost visible := true.
                
                GameVariant set &selected_index:0.
            ].
    ]

    #method $onGameVariantChange : anIndex
    [
        anIndex =>
            0 ? [
                ThirdPlayer visible := false.
                TPLabel visible := false.
                FourthPlayer visible := false.
                FPLabel visible := false.
            ]
            1 ? [
                ThirdPlayer visible := true.
                TPLabel visible := true.
                FourthPlayer visible := false.
                FPLabel visible := false.
            ]
            2 ? [
                ThirdPlayer visible := true.
                TPLabel visible := true.
                FourthPlayer visible := true.
                FPLabel visible := true.
            ].
    ]

    #method $startGame
    [
        #var gameMaster := 'game loadConfiguration.
        
        // add human player
        'hostPlayer loadConfiguration invite:
            {
                name = txtUserName literal.
                
                playground = $owner.
            }
            &to:gameMaster.
        
        // add other players
        (SecondPlayer, ThirdPlayer, FourthPlayer) run &each: control
        [
            (control is &visible)?
            [
                control selected invite:
                {
                    name = control caption.
                    
                    playground = $owner.
                }
                &to:gameMaster.
            ].
        ].
            
        threadControl start &func:
        [
            gameMaster start.
        ].
        
        theForm close.
//        self set &forms'dialog_result: => (engine'GameMaster start | ~econtrol then: =>
//        [
//            dialogs'MessageBox::{ &dialogs'error_text:"The program flow is broken." &caption:"Up'N'Down" &control_parent:self } open &forms'modal.
//
//            'program stop.
//
//            ^ nil.
//        ]).
    ]

    #method => theForm.
}