
import java.net.*;
import java.io.*;
import java.util.*;
import javax.net.ssl.*;
import java.security.*;

/**
 * A bunch of utility stuff for doing SSL things.
 *
 * @author Chris Nelson (cnelson@synchrony.net)
 */
public class SSLUtils {
        static String tunnelHost;
        static int tunnelPort;
	
	/** This method builds an SSL socket, after auto-starting SSL */
	public static Socket buildSSLSocket(String host, int port, String httpProxyHost,
                                            int httpProxyPort)
		throws IOException, UnknownHostException
	{
           SSLSocket sslSocket =  null;
           SSLSocketFactory factory =
              (SSLSocketFactory)SSLSocketFactory.getDefault();

           // Determine if a proxy should be used. Use system properties if set
           // Otherwise use http proxy. If neither is set, dont use a proxy
           tunnelHost = System.getProperty("https.proxyHost");
           tunnelPort = Integer.getInteger("https.proxyPort", 80).intValue();

           if (tunnelHost==null) {
              // Try to use http proxy instead
              tunnelHost = httpProxyHost;
              tunnelPort = httpProxyPort;
           }

           /*
           System.out.println("https proxyHost=" + tunnelHost +
                              " proxyPort=" + tunnelPort +
                              " host=" + host +
                              " port=" + port);
           */

           /*                         
            * If a proxy has been set...
            * Set up a socket to do tunneling through the proxy.
            * Start it off as a regular socket, then layer SSL
            * over the top of it.
            */
           if (tunnelHost==null) {
              sslSocket = (SSLSocket)factory.createSocket(host, port);
           } else {
              Socket tunnel = new Socket(tunnelHost, tunnelPort);
              doTunnelHandshake(tunnel, host, port);

              // Overlay tunnel socket with SSL
              sslSocket = (SSLSocket)factory.createSocket(tunnel, host, port, true);
           }

           /*
            * Handshaking is started manually in this example because
            * PrintWriter catches all IOExceptions (including
            * SSLExceptions), sets an internal error flag, and then
            * returns without rethrowing the exception.
            *
            * Unfortunately, this means any error messages are lost,
            * which caused lots of confusion for others using this
            * code.  The only way to tell there was an error is to call
            * PrintWriter.checkError().
            */
           sslSocket.startHandshake();   

           return  sslSocket;  
	    
	}

        static private void doTunnelHandshake(Socket tunnel, String host, int port)
         throws IOException
        {
             OutputStream out = tunnel.getOutputStream();
             String msg = "CONNECT " + host + ":" + port + " HTTP/1.0\n"
                          + "User-Agent: "
                          + sun.net.www.protocol.http.HttpURLConnection.userAgent
                          + "\r\n\r\n";
             byte b[];
             try {
                 /*
                  * We really do want ASCII7 -- the http protocol doesn't change
                  * with locale.
                  */
                 b = msg.getBytes("ASCII7");
             } catch (UnsupportedEncodingException ignored) {
                 /*
                  * If ASCII7 isn't there, something serious is wrong, but
                  * Paranoia Is Good (tm)
                  */
                 b = msg.getBytes();
             }
             out.write(b);
             out.flush();

             /*
              * We need to store the reply so we can create a detailed
              * error message to the user.
              */
             byte		reply[] = new byte[200];
             int		replyLen = 0;
             int		newlinesSeen = 0;
             boolean		headerDone = false;	/* Done on first newline */

             InputStream	in = tunnel.getInputStream();
             boolean		error = false;

             while (newlinesSeen < 2) {
                 int i = in.read();
                 if (i < 0) {
                     throw new IOException("Unexpected EOF from proxy");
                 }
                 if (i == '\n') {
                     headerDone = true;
                     ++newlinesSeen;
                 } else if (i != '\r') {
                     newlinesSeen = 0;
                     if (!headerDone && replyLen < reply.length) {
                         reply[replyLen++] = (byte) i;
                     }
                 }
             }

             /*
              * Converting the byte array to a string is slightly wasteful
              * in the case where the connection was successful, but it's
              * insignificant compared to the network overhead.
              */
             String replyStr;
             try {
                 replyStr = new String(reply, 0, replyLen, "ASCII7");
             } catch (UnsupportedEncodingException ignored) {
                 replyStr = new String(reply, 0, replyLen);
             }

             // Parse response, check for status code
             StringTokenizer st = new StringTokenizer(replyStr);
             st.nextToken(); // ignore version part
             if (!st.nextToken().startsWith("200")) {
                throw new IOException("Unable to tunnel through "
                        + tunnelHost + ":" + tunnelPort
                        + ".  Proxy returns \"" + replyStr + "\"");
             }

             /* tunneling Handshake was successful! */
         }
}
