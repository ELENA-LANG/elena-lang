#import system.
#import system'routines.
#import system'text.
#import system'io.
#import system'threading.
#import extensions.
#import net.

#subject(class:ChatMember)chatter.

#class(sealed) ChatMember
{
    #field(type:membuffer)theBuffer.
    #field(type:tcp_remoteclient) theOwner.

    #constructor new : owner
    [
        theBuffer := MemoryBuffer new.
        
        theOwner := owner tcp_remoteclient.
    ]    
    
    #method(stacksafe)write &bytearray:buffer &int:length
    [
        theBuffer write &int:0 &int:length &bytearray:buffer.
    ]
    
    #method readCommand
    [
        #var(type:int)len := theBuffer length.
        (len > 0)?
        [
            #var(type:int)package_len.
            theBuffer read &int:0 &vint:package_len.
            
            (package_len + 4 < len)?
            [
                #var literal := UTF8Encoding toLiteral &int:4 &int:package_len &bytearray:(theBuffer bytearray).
                
                theBuffer delete &int:0 &int:(package_len).
                
                ^ literal toJson.
            ].
        ].
        
        ^ $nil.
    ]
    
    #method chatter = $self.
}

#class ChatServer
{
    #field theLock.
    
    #field(type:tcp_server)theNetworkServer.
    #field(type:thread)theBroadcastThread.
    
    #constructor new &port:port
    [
        theLock := Object new.
        
        theNetworkServer := TcpServer new &port:port.
        
        theNetworkServer set &blocking:false.
    ]
    
    #method proceed
    [
        theNetworkServer connections run &each:client
        [
            #lock (theLock)
            [
                #var command := client Parameter readCommand.
                ($nil != command)
                    ? [
                        (command Command) =>
                            INTRODUCTION ? 
                            [
                                client Parameter set &Name:(command Value).
                                
                                console writeLine:"command Value":" joined".
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
                console writeLine:"new client joint".
                
                client set &Parameter:(ChatMember new:client).
            ]
            
            onLeave:client
            [
                console writeLine:"client left".
            ]
            
            onError:e &for:client
            [
                console writeLine:"exception #":(e error_code).
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