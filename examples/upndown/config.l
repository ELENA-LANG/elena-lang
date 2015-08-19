// --- default namespaces ---
#import system.
#import system'dynamic.

// --- configuator ---

#static configuator = CastOver new &args:upndown'testConfigurator/*:upndown'ai'aiConfigurator*/.

////// --- namespace shortcuts ---
////#define upndown'ai'*  = cardgame'upndown'ai'*.
////#define engine'*      = cardgame'engine'*.
//
//// === Groups ==
//
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
