#import system.
#import system'routines.
#import system'text.
#import system'io.
#import system'threading.
#import extensions.
#import extensions'dynamic.
#import net.

#subject chatter = ChatMember.

#class(sealed) ChatMember :: BaseClient
{
    #field(tcp_remoteclient) theOwner.
    #field theServer.
    
    #field(prop:Name) theName.

    #constructor new : owner &of:server
        <= (new)
    [
        theOwner := owner tcp_remoteclient.
        
        theServer := server.
    ]    
    
    #method server = theServer.
    
    #method chatter = $self.
}

#class ChatServer
{
    #field theLock.
    
    #field(tcp_server)theNetworkServer.
    #field(thread)theBroadcastThread.
    
    #constructor new &port:port
    [
        theLock := Object new.
        
        theNetworkServer := TcpServer new &port:port.
        
        theNetworkServer set &blocking:false.
    ]
    
    #method translate : command
    [
        #var(literal)json := command toJson literal.
        
        theNetworkServer sendAll &int:(json length) sendAll &literal:json.
    ]
    
    #method proceed
    [
        theNetworkServer run &eachConnection:client
        [
            #lock (theLock)
            [
                #var member := client Parameter.
                ($nil != member)? 
                [
                    #var command := member readCommand.
                    ($nil != command)
                        ? [
                            (command Command) =>
                                INTRODUCTION ? 
                                [
                                    member set &Name:(command Value).
                                    
                                    console writeLine:(command Value):" joined".
                                ]
                                SEND ?
                                [
                                    console writeLine:(member Name):":":(command Value).
                                    
                                    member server translate:DTO { Command = TRANSLATE. Value = command Value. User = member Name. }.
                                ].
                        ].                
                ].
            ].
        ].        
        
        ^ true.
    ]
    
    #method start
    [
        theNetworkServer set &tcp_server_listener:TcpServerListener
        {
            onConnect:client
            [
                #lock (theLock)
                [
                    console writeLine:"new client joint".
                
                    client set &Parameter:(ChatMember new:client &of:$owner).
                ].                    
            ]
            
            onLeave:client
            [
                #lock (theLock)
                [
                    console writeLine:"client left".
                ].
            ]
            
            onError:e &for:client
            [
                #lock (theLock)
                [
                    console writeLine:"exception #":(e error_code).
                ].
            ]
            
            onReceived:client &bytearray:buffer &int:length 
            [
                #lock (theLock)
                [
                    client Parameter chatter write &bytearray:buffer &int:length.
                ].                
            ]
        }.
        
        theNetworkServer start.
        
        theBroadcastThread := Thread new &func:
        [
            #loop $self proceed.
        ].
        
        theBroadcastThread start.
    ]
}