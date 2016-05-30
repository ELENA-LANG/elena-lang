#import system.
#import system'routines.
#import system'text.
#import system'io.
#import system'threading.
#import extensions.
#import extensions'dynamic.
#import net.

#subject chatter = ChatMember.

#class(sealed) ChatMember
{
    #field(membuffer)theBuffer.
    #field(tcp_remoteclient) theOwner.

    #field(prop:Name) theName.

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
        #var(int)len := theBuffer length.
        (len > 0)?
        [
            #var(int)package_len.
            theBuffer read &int:0 &vint:package_len.
            
            (package_len + 4 <= len)?
            [
                #var literal := UTF8Encoding toLiteral &int:4 &int:package_len &bytearray:(theBuffer bytearray).
                
                #var llen := literal length.
                
                theBuffer delete &int:0 &int:(package_len + 4).
                
                ^ literal fromJson.
            ].
        ].
        
        ^ $nil.
    ]
    
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
                
                    client set &Parameter:(ChatMember new:client).
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