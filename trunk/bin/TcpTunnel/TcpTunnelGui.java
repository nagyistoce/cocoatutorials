
import java.net.*;
import java.io.*;
import java.awt.*;
import java.awt.event.*;

/**
 * A <code>TcpTunnelGui</code> object listens on the given port,
 * and once <code>Start</code> is pressed, will forward all bytes
 * to the given host and port. All traffic is displayed in a
 * UI.
 *
 * @author Sanjiva Weerawarana (sanjiva@watson.ibm.com)
 */
public class TcpTunnelGui extends Frame {

  // Change this value to change the size of the text displayed in the
  // "listen" and "tunnel" panels of the UI. (Added by JCBeatty)
  int mainPanelTextSize = 14;

  int listenPort;
  String tunnelHost;
  int tunnelPort;
  TextArea listenText, tunnelText;
  Label status;
  Relay inRelay, outRelay;

  public TcpTunnelGui (int listenPort, String tunnelHost, int tunnelPort) {
  
    Panel p;
    
    this.listenPort = listenPort;
    this.tunnelHost = tunnelHost;
    this.tunnelPort = tunnelPort;

    addWindowListener (new WindowAdapter () {
      public void windowClosing (WindowEvent e) {
        System.exit (0);
      }
    });

    // show info
    setTitle ("TCP Tunnel/Monitor: Tunneling localhost:" + listenPort +
              " to " + tunnelHost + ":" + tunnelPort);

    // labels
    p = new Panel ();
    p.setLayout (new BorderLayout ());
    Label l1, l2;
    p.add ("West",
           l1 = new Label ("From localhost:" + listenPort, Label.CENTER));
    p.add ("East",
           l2 = new Label ("From " + tunnelHost + ":" + tunnelPort,
                           Label.CENTER));
    add ("North", p);

    // the monitor part
    p = new Panel ();
    p.setLayout (new GridLayout (-1,2));
    p.add (listenText = new TextArea ());
    p.add (tunnelText = new TextArea ());
    add ("Center", p);
    
    // Added by JCBeatty
    Font mainPanelFont = new Font( "SansSerif", Font.PLAIN, mainPanelTextSize );
    listenText.setFont( mainPanelFont );
    tunnelText.setFont( mainPanelFont );

    // clear and status
    Panel p2 = new Panel ();
    p2.setLayout (new BorderLayout ());

    p = new Panel ();
    Button b = new Button ("Clear");
    b.addActionListener (new ActionListener () {
      public void actionPerformed (ActionEvent e) {
        listenText.setText ("");
        tunnelText.setText ("");
      }
    });
    p.add (b);
    p2.add ("Center", p);

    p2.add ("South", status = new Label ());
    add ("South", p2);

    pack ();
    show ();

    Font f = l1.getFont ();
    l1.setFont (new Font (f.getName (), Font.BOLD, f.getSize ()));
    l2.setFont (new Font (f.getName (), Font.BOLD, f.getSize ()));
  }

  public int getListenPort () {
    return listenPort;
  }

  public String getTunnelHost () {
    return tunnelHost;
  }

  public int getTunnelPort () {
    return tunnelPort;
  }
  
  public TextArea getListenText () {
    return listenText;
  }

  public TextArea getTunnelText () {
    return tunnelText;
  }

  public Label getStatus () {
    return status;
  }

  public static void main (String args[]) throws IOException {
  
    if (args.length != 3) {
      System.err.println ("Usage: java TcpTunnelGui listenport tunnelhost " +
                          "tunnelport");
      System.exit (1);
    }
    
    int listenPort = Integer.parseInt (args[0]);
    String tunnelHost = args[1];
    int tunnelPort = Integer.parseInt (args[2]);
    final TcpTunnelGui ttg = 
      new TcpTunnelGui (listenPort, tunnelHost, tunnelPort);

    // create the server thread
    Thread server = new Thread () {
      public void run () {
        ServerSocket ss = null;
        Label status = ttg.getStatus ();
        try {
          ss = new ServerSocket (ttg.getListenPort ());
        } catch (Exception e) {
          e.printStackTrace ();
          System.exit (1);
        }
        while (true) {
          try {
            status.setText ("Listening for connections on port " + 
                            ttg.getListenPort () + " ...");
            // accept the connection from my client
            Socket sc = ss.accept ();
            
            // connect to the thing I'm tunnelling for
            Socket st = new Socket (ttg.getTunnelHost (),
                                    ttg.getTunnelPort ());
            
            status.setText ("Tunnelling port " + ttg.getListenPort () +
                            " to port " + ttg.getTunnelPort () + 
                            " on host " + ttg.getTunnelHost () + " ...");
            
            // relay the stuff thru
            new Relay (sc.getInputStream (), st.getOutputStream (),
                       ttg.getListenText ()).start ();
            new Relay (st.getInputStream (), sc.getOutputStream (),
                       ttg.getTunnelText ()).start ();
            
            // that's it .. they're off; now I go back to my stuff.
          } catch (Exception ee) {
            status.setText ("Ouch! [See console for details]: " + 
                            ee.getMessage ());
            ee.printStackTrace ();
          }
        }
      }
    };
    server.start ();
  }
}
