
import java.net.*;
import java.io.*;
import java.util.*;
import java.lang.reflect.*;

/**
 * A bunch of utility stuff for doing HTTP things.
 * <p>
 * 2000/07/30 W. Cloetens     added Multipart Mime support
 *
 * @author Sanjiva Weerawarana (sanjiva@watson.ibm.com)
 * @author Matthew J. Duftler (duftler@us.ibm.com)
 * @author Wouter Cloetens (wcloeten@raleigh.ibm.com)
 * @author Scott Nichol (snichol@computer.org)
 */
public class HTTPUtils {

  private static final String HTTP_VERSION = "1.0";
  private static final int    HTTP_DEFAULT_PORT = 80;
  private static final int    HTTPS_DEFAULT_PORT = 443;

  public  static final int    DEFAULT_OUTPUT_BUFFER_SIZE = 512;

  /**
   * This method either creates a socket or calls SSLUtils to
   * create an SSLSocket.  It uses reflection to avoid a compile time
   * dependency on SSL.
   *
   * @author Chris Nelson
   */
  private static Socket buildSocket(URL url, int targetPort,
                                    String httpProxyHost, int httpProxyPort,
				    Boolean tcpNoDelay)
     throws Exception {
      Socket s = null;
      String host = null;
      int port = targetPort;
      host = url.getHost();

      if (url.getProtocol().equalsIgnoreCase("HTTPS")) {
          // Using reflection to avoid compile time dependencies
          Class SSLUtilsClass =
              Class.forName("SSLUtils");
          Class[] paramTypes = new Class[] {String.class, int.class, String.class, int.class};
          Method buildSSLSocket = SSLUtilsClass.getMethod(
              "buildSSLSocket", paramTypes);
          Object[] params = new Object[] {host, new Integer(port),
              httpProxyHost, new Integer(httpProxyPort)};
          s = (Socket)buildSSLSocket.invoke(null, params);
      } else {
          if (httpProxyHost != null) {
              host = httpProxyHost;
              port = httpProxyPort;
          }
          s = new Socket(host, port);
      }
      
      if (tcpNoDelay != null)
      {
	if (s != null) 
	  s.setTcpNoDelay(tcpNoDelay.booleanValue());
      }

      return s;
   }

  /**
   * Utility function to determine port number from URL object.
   *
   * @param url URL object from which to determine port number
   * @return port number
   */
  private static int getPort(URL url) throws IOException {
      int port = url.getPort();
      if (port < 0)  // No port given, use HTTP or HTTPS default
          if (url.getProtocol().equalsIgnoreCase("HTTPS"))
              port = HTTPS_DEFAULT_PORT;
          else
              port = HTTP_DEFAULT_PORT;
      return port;
  }
}
