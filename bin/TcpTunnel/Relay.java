
import java.io.*;
import java.net.*;
import java.awt.TextArea;

/**
 * A <code>Relay</code> object is used by <code>TcpTunnel</code>
 * and <code>TcpTunnelGui</code> to relay bytes from an
 * <code>InputStream</code> to a <code>OutputStream</code>.
 *
 * @author Sanjiva Weerawarana (sanjiva@watson.ibm.com)
 */
public class Relay extends Thread {

  final static int BUFSIZ = 1000;
  InputStream in;
  OutputStream out;
  byte buf[] = new byte[BUFSIZ];
  TextArea ta;

  Relay (InputStream in, OutputStream out, TextArea ta) {
    this.in = in;
    this.out = out;
    this.ta = ta;
  }

  public void run () {
  
    int n;

    try {
      while ((n = in.read (buf)) > 0) {
        out.write (buf, 0, n);
        out.flush ();
        if (ta != null) {
          ta.append (new String (buf, 0, n, "8859_1"));
        }
      }
    } catch (IOException e) {
    } finally {
      try {
        in.close ();
        out.close ();
      } catch (IOException e) {
      }
    }
  }
}
