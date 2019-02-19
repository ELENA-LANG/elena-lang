//import system'routines'stex.
////import system'text.
////import system'io.
//import system'threading.
////import extensions.
////import extensions'dynamic.
import net;

//sealed class ChatMember :: BaseClient
//{
//    T<TcpRemoteClient> theOwner.
//    T<ChatServer>      theServer.
//    
//    literal prop Name :: theName.
//
//    constructor new(TcpRemoteClient owner) of(ChatServer server)
//        <= new;
//    [
//        theOwner := owner.
//        
//        theServer := server.
//    ]    
//    
//    T<ChatServer> server = theServer.
//}

sealed class ChatServer
{
//    object     theLock.
//    
    TcpServer theNetworkServer;
//    T<Thread>    theBroadcastThread.
    
    constructor assign(int port)
    {
//        theLock := Object new.
        
        theNetworkServer := TcpServer.assign(port);
        
//        theNetworkServer Blocking := false.
    }
    
//    translate : command
//    [
//        literal json := command toJson.
//        
//        theNetworkServer sendAll(json length); sendAll(json).
//    ]
//    
//    private proceed
//    [
//        theNetworkServer Clients; forEach(:client)<TcpRemoteClient>
//        [
//            lock (theLock)
//            [
//                auto member := T<ChatMember>(client Parameter).
//                if(nil != member)
//                [
//                    var command := member readCommand.
//                    if (nil != command)
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
//                                ]
//                        ]
//                ]
//            ]
//        ].
//        
//        ^ true.
//    ]
//    
//    start
//    [
//        theNetworkServer onConnect := (:client)<TcpRemoteClient>
//        [
//                lock (theLock)
//                [
//                    console writeLine:"new client joint".
//                
//                    client Parameter := ChatMember new:client of:self.
//                ].                                
//        ].
//        
//        theNetworkServer onLeave := (:client)<TcpRemoteClient>
//        [
//                lock (theLock)
//                [
//                    console writeLine:"client left".
//                ].
//        ].
//        
//        theNetworkServer onError := (:client:e)<TcpRemoteClient,Exception>
//        [
//                lock (theLock)
//                [
//                    console printLine("exception #",e error_code).
//                ].
//        ].
//        
//        theNetworkServer onReceived := (:client:data:length)<TcpRemoteClient,ByteArray,int>
//        [
//                lock (theLock)
//                [
//                    T<ChatMember>(client Parameter) write(data, length).
//                ].                        
//        ].
//        
//        theNetworkServer start.
//        
//        theBroadcastThread := Thread new:
//        [
//            while(self proceed) [].
//        ].
//       
//        theBroadcastThread start.
//    ]
}