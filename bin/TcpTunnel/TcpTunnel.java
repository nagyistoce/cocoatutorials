// package org.apache.soap.util.net;

import java.net.*;
import java.io.*;

/**
 * A <code>TcpTunnel</code> object listens on the given port,
 * and once <code>Start</code> is pressed, will forward all bytes
 * to the given host and port.
 *
 * @author Sanjiva Weerawarana (sanjiva@watson.ibm.com)
 */
public class TcpTunnel {
  public static void main (String args[]) throws IOException {
    if (args.length != 3) {
      System.err.println ("Usage: java TcpTunnel listenport tunnelhost tunnelport");
      System.exit (1);
    }

    int listenport = Integer.parseInt (args[0]);
    String tunnelhost = args[1];
    int tunnelport = Integer.parseInt (args[2]);

    System.out.println ("TcpTunnel: ready to rock and roll on port " + 
                        listenport);

    ServerSocket ss = new ServerSocket (listenport);
    while (true) {
      // accept the connection from my client
      Socket sc = ss.accept ();
      
      // connect to the thing I'm tunnelling for
      Socket st = new Socket (tunnelhost, tunnelport);
      
      System.out.println ("TcpTunnel: tunnelling port " + listenport +
                          " to port " + tunnelport + " on host " + 
                          tunnelhost);

      // relay the stuff thru
      new Relay (sc.getInputStream(), st.getOutputStream(), null).start ();
      new Relay (st.getInputStream(), sc.getOutputStream(), null).start ();
      
      // that's it .. they're off; now I go back to my stuff.
    }
  }
}
