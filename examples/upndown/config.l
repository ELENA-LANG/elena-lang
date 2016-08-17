// --- default namespaces ---
#import system.
#import system'routines.
#import system'collections.
#import system'dynamic.

#import extensions'text.

#class(extension)configOp
{    
    #method loadConfiguration
        = self loadConfiguration:$nil.

    #method loadConfiguration : target
    [
        #var stack := Stack new push:target.
        #var counter := Integer new.
        
        TokenEnumerator new &literal:(self literal) &token_dfa:(ExpressionStatemachine new) run &each:token
        [
            token =>
                "(" ? [ counter *= 16. ]
                ")" ? [
                    #var(int) argCounter := counter int mod:16.
                    counter /= 16.
                    
                    #var arg := $nil.
                    #loop (argCounter > 0)?
                    [
                        arg := stack pop group &with:arg.
                        
                        argCounter -= 1.
                    ].
                    
                    #var target := stack pop.
                    ($nil != target)?
                    [
                        ($nil == arg)
                            ? [ stack push:(target new). ]
                            ! [ stack push:(target new:arg). ].                                        
                    ]
                    ! [ stack push:arg. ].  
                ]
                "[" ? [ counter *= 16. ]
                "]" ? [
                    #var(int) argCounter := counter int mod:16.
                    counter /= 16.
                    
                    #var arg := $nil.
                    #loop (argCounter > 0)?
                    [
                        arg := stack pop cast &with:arg.
                        
                        argCounter -= 1.
                    ].
                    
                    #var target := stack pop.
                    ($nil != target)?
                    [
                        ($nil == arg)
                            ? [ stack push:(target new). ]
                            ! [ stack push:(target new:arg). ].                                        
                    ]
                    ! [ stack push:arg. ].  
                ]
                "," ? [ ]
                ! [
                    counter += 1.
                    
                    stack push:(Symbol new &literal:(token literal)).
                ].
        ].
        
        ^ stack pop.
    ]
}

// --- configuations ---

#symbol(preloaded) dummyLoader = (
    upndown'ai'aiConfigurator,
    upndown'testConfigurator,
    upndown'GameMaster,
    upndown'UpNDown,
    upndown'BasePlayer,
    upndown'gui'UIPlayer,
    upndown'ai'ChaoticPlayer).

#symbol testAIConnectorConfig = "[upndown'ai'aiConfigurator,upndown'testConfigurator]".

#symbol chaoticAIConfig = "upndown'BasePlayer(upndown'ai'ChaoticPlayer())".

#symbol upndownGame = "upndown'GameMaster(upndown'UpNDown())".

#symbol hostUpndownPlayer = "upndown'BasePlayer(upndown'gui'UIPlayer())".

// === Groups ==

//#static configuator = Cast new &args:upndown'ai'aiConfigurator:upndown'testConfigurator.

#symbol connectorGroup = 'connectorConfig loadConfiguration.

//#symbol ConnectorGroup = GroupVariable
///*    insert &group_member:cardgame'tester'EConnector
//    insert &group_member:upndown'ai'EConnector*/.
//
////#symbol GameMasterGroup = GroupVariable
////    insert &group_member:engine'BaseMaster
////    insert &group_member:cardgame'upndown'egame
/////*    insert:cardgame'tester'EJokerTester*/.
////
////#symbol HostPlayerGroup : aProfile = GroupVariable
////    insert &group_member:(aProfile player)
////    insert &group_member:(cardgame'gui'main'EPlayer::aProfile)
////    insert &group_member:(cardgame'upndown'gui'EHumanPlayer::aProfile).
////
////#symbol ClientPlayerGroup : aProfile = GroupVariable
////    insert &group_member:(aProfile player).
