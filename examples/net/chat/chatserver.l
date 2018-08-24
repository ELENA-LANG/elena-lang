//import system'routines.
//import system'text.
//import system'io.
//import system'threading.
//import extensions.
//import extensions'dynamic.
import net.

//type chatter = ChatMember.
//
//sealed class ChatMember :: BaseClient
//{
//    tcp_remoteclient theOwner.
//    object           theServer.
//    
//    literal prop Name :: theName.
//
//    constructor new:owner of:server
//        <= new;
//    [
//        theOwner := owner tcp_remoteclient.
//        
//        theServer := server.
//    ]    
//    
//    server = theServer.
//    
//    chatter = $self.
//}

sealed class ChatServer
{
//    object     theLock.
//    
    T<TcpServer> theNetworkServer.
//    thread     theBroadcastThread.
    
    constructor new(int port)
    [
//        theLock := Object new.
        
        theNetworkServer := TcpServer new(port).
        
        theNetworkServer Blocking := false.
    ]
    
//    translate : command
//    [
//        literal json := command toJson; literal.
//        
//        theNetworkServer sendAll int(json length); sendAll literal:json.
//    ]
//    
//    proceed
//    [
//        theNetworkServer run eachConnection(:client)
//        [
//            lock (theLock)
//            [
//                var member := client Parameter.
//                if($nil != member)
//                [
//                    var command := member readCommand.
//                    if ($nil != command)
//                        [
//                            (command Command) =>
//                                INTRODUCTION
//                                [
//                                    member Name := command Value.
//                                    
//                                    console printLine(command Value," joined").
//                                ];
//                                SEND
//                                [
//                                    console printLine(member Name,":",command Value).
//                                    
//                                    member server; translate:CommandDTO::{ @_command := TRANSLATE. @_value := command Value. @_user := member Name. }.
//                                ].
//                        ].                
//                ].
//            ].
//        ].        
//        
//        ^ true.
//    ]
    
    start
    [
        theNetworkServer onConnect := (:client)<TcpRemoteClient>
        [
//                lock (theLock)
//                [
//                    console writeLine:"new client joint".
//                
//                    client set Parameter(ChatMember new:client of:$self).
//                ].                                
        ].
        
        theNetworkServer onLeave := (:client)<TcpRemoteClient>
        [
//                lock (theLock)
//                [
//                    console writeLine:"client left".
//                ].
        ].
        
        theNetworkServer onError := (:client:e)<TcpRemoteClient,Exception>
        [
//                lock (theLock)
//                [
//                    console printLine("exception #",e error_code).
//                ].
        ].
        
        theNetworkServer onReceived := (:client:data:length)<TcpRemoteClient,ByteArray,int>
        [
//                lock (theLock)
//                [
//                    client Parameter; chatter; write bytearray:buffer int:length.
//                ].                        
        ].
        
        theNetworkServer start.
        
//        theBroadcastThread := Thread new func:
//        [
//            while($self proceed) [].
//        ].
//        
//        theBroadcastThread start.
    ]
}