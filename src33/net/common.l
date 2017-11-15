// --- types ---
type tcp_client       = net'TcpClient.
type tcp_remoteclient = net'TcpRemoteClient.
type tcp_server       = net'TcpServer.

class TcpClientException :: Exception
{
}