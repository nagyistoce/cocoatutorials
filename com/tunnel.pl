$from = shift or $from = 8070 ;
$to   = shift or $to   = $from == 70 ? 80 : 8080 ;

system("start java org.apache.soap.util.net.TcpTunnelGui $from localhost $to") ;