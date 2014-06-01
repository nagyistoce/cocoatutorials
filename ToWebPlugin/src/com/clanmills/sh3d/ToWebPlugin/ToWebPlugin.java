package com.clanmills.sh3d.ToWebPlugin;

import java.awt.Graphics2D;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.io.BufferedOutputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.URL;
import java.net.URLDecoder;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.security.CodeSource;
import java.util.ArrayList;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

import javax.imageio.ImageIO;
import javax.swing.JDialog;
import javax.swing.JOptionPane;
import javax.swing.event.ListSelectionListener;

import org.stringtemplate.v4.*;

import com.eteks.sweethome3d.model.Camera;
import com.eteks.sweethome3d.model.Home;
import com.eteks.sweethome3d.model.Level;
import com.eteks.sweethome3d.model.UserPreferences;
import com.eteks.sweethome3d.plugin.Plugin;
import com.eteks.sweethome3d.plugin.PluginAction;
import com.eteks.sweethome3d.swing.HomeComponent3D;
import com.eteks.sweethome3d.swing.PlanComponent;








import com.eteks.sweethome3d.viewcontroller.HomeController;

import com.eteks.sweethome3d.viewcontroller.HomeController3D;

/* ListDemo.java requires no other files. */
import java.awt.*;
import java.awt.event.*;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

import javax.swing.*;
import javax.swing.event.*;

@SuppressWarnings("serial")
class ListDemo extends JPanel
                      implements ListSelectionListener {
    @SuppressWarnings("rawtypes")
	private JList list;
    @SuppressWarnings("rawtypes")
	private DefaultListModel listModel;
 
    private static final String hireString = "Hire";
    private static final String fireString = "Fire";
    private JButton fireButton;
    private JTextField employeeName;
 
    public ListDemo() {
        super(new BorderLayout());
 
        listModel = new DefaultListModel();
        listModel.addElement("Jane Doe");
        listModel.addElement("John Smith");
        listModel.addElement("Kathy Green");
 
        //Create the list and put it in a scroll pane.
        list = new JList(listModel);
        list.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        list.setSelectedIndex(0);
        list.addListSelectionListener(this);
        list.setVisibleRowCount(5);
        JScrollPane listScrollPane = new JScrollPane(list);
 
        JButton hireButton = new JButton(hireString);
        HireListener hireListener = new HireListener(hireButton);
        hireButton.setActionCommand(hireString);
        hireButton.addActionListener(hireListener);
        hireButton.setEnabled(false);
 
        fireButton = new JButton(fireString);
        fireButton.setActionCommand(fireString);
        fireButton.addActionListener(new FireListener());
 
        employeeName = new JTextField(10);
        employeeName.addActionListener(hireListener);
        employeeName.getDocument().addDocumentListener(hireListener);
        String name = listModel.getElementAt(
                              list.getSelectedIndex()).toString();
 
        //Create a panel that uses BoxLayout.
        JPanel buttonPane = new JPanel();
        buttonPane.setLayout(new BoxLayout(buttonPane,
                                           BoxLayout.LINE_AXIS));
        buttonPane.add(fireButton);
        buttonPane.add(Box.createHorizontalStrut(5));
        buttonPane.add(new JSeparator(SwingConstants.VERTICAL));
        buttonPane.add(Box.createHorizontalStrut(5));
        buttonPane.add(employeeName);
        buttonPane.add(hireButton);
        buttonPane.setBorder(BorderFactory.createEmptyBorder(5,5,5,5));
 
        add(listScrollPane, BorderLayout.CENTER);
        add(buttonPane, BorderLayout.PAGE_END);
    }
 
    class FireListener implements ActionListener {
        public void actionPerformed(ActionEvent e) {
            //This method can be called only if
            //there's a valid selection
            //so go ahead and remove whatever's selected.
            int index = list.getSelectedIndex();
            listModel.remove(index);
 
            int size = listModel.getSize();
 
            if (size == 0) { //Nobody's left, disable firing.
                fireButton.setEnabled(false);
 
            } else { //Select an index.
                if (index == listModel.getSize()) {
                    //removed item in last position
                    index--;
                }
 
                list.setSelectedIndex(index);
                list.ensureIndexIsVisible(index);
            }
        }
    }
 
    //This listener is shared by the text field and the hire button.
    class HireListener implements ActionListener, DocumentListener {
        private boolean alreadyEnabled = false;
        private JButton button;
 
        public HireListener(JButton button) {
            this.button = button;
        }
 
        //Required by ActionListener.
        public void actionPerformed(ActionEvent e) {
            String name = employeeName.getText();
 
            //User didn't type in a unique name...
            if (name.equals("") || alreadyInList(name)) {
                Toolkit.getDefaultToolkit().beep();
                employeeName.requestFocusInWindow();
                employeeName.selectAll();
                return;
            }
 
            int index = list.getSelectedIndex(); //get selected index
            if (index == -1) { //no selection, so insert at beginning
                index = 0;
            } else {           //add after the selected item
                index++;
            }
 
            listModel.insertElementAt(employeeName.getText(), index);
            //If we just wanted to add to the end, we'd do this:
            //listModel.addElement(employeeName.getText());
 
            //Reset the text field.
            employeeName.requestFocusInWindow();
            employeeName.setText("");
 
            //Select the new item and make it visible.
            list.setSelectedIndex(index);
            list.ensureIndexIsVisible(index);
        }
 
        //This method tests for string equality. You could certainly
        //get more sophisticated about the algorithm.  For example,
        //you might want to ignore white space and capitalization.
        protected boolean alreadyInList(String name) {
            return listModel.contains(name);
        }
 
        //Required by DocumentListener.
        public void insertUpdate(DocumentEvent e) {
            enableButton();
        }
 
        //Required by DocumentListener.
        public void removeUpdate(DocumentEvent e) {
            handleEmptyTextField(e);
        }
 
        //Required by DocumentListener.
        public void changedUpdate(DocumentEvent e) {
            if (!handleEmptyTextField(e)) {
                enableButton();
            }
        }
 
        private void enableButton() {
            if (!alreadyEnabled) {
                button.setEnabled(true);
            }
        }
 
        private boolean handleEmptyTextField(DocumentEvent e) {
            if (e.getDocument().getLength() <= 0) {
                button.setEnabled(false);
                alreadyEnabled = false;
                return true;
            }
            return false;
        }
    }
 
    //This method is required by ListSelectionListener.
    public void valueChanged(ListSelectionEvent e) {
        if (e.getValueIsAdjusting() == false) {
 
            if (list.getSelectedIndex() == -1) {
            //No selection, disable fire button.
                fireButton.setEnabled(false);
 
            } else {
            //Selection, enable the fire button.
                fireButton.setEnabled(true);
            }
        }
    }
 
    private static JFrame  frame     = null;
    private static JDialog d3        = null;
    private static boolean bModal    = false;
    private static boolean bModeless = !bModal;

    /**
     * Create the GUI and show it.  For thread safety,
     * this method should be invoked from the
     * event-dispatching thread.
     */
    private static void createAndShowGUI() {
        //Create and set up the window.
    	if ( frame == null ) {
    		frame = new JFrame("ListDemo");
	        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	 
	        //Create and set up the content pane.
	        JComponent newContentPane = new ListDemo();
	        newContentPane.setOpaque(true); //content panes must be opaque
	        frame.setContentPane(newContentPane);
	        
	        if ( bModal ) {
		        //The modeless dialog box
		        JDialog d2 = new JDialog(frame);
		        //The document-modal dialog box
		        d3 = new JDialog(d2, "", Dialog.ModalityType.DOCUMENT_MODAL);
		        d3.setContentPane(newContentPane);
		        d3.pack();
	        } else {
		        //Display the window.
		        frame.pack();
	        }
    	}
    	if (  bModal    && d3    != null ) d3.setVisible(true);
    	if (  bModeless && frame != null ) frame.setVisible(true);
    }
    
    public static void killGUI()
    {
	    if (  bModal    && d3    != null ) d3.setVisible(false);
	    if (  bModeless && frame != null ) frame.setVisible(false);
	    frame=null;
	    d3=null;
    }
 
    public static void main(String[] args) {
        // Schedule a job for the event-dispatching thread:
        // creating and showing this application's GUI.
        createAndShowGUI();
//        EventQueue.invokeLater(
//    		new Runnable() { 
//    			public void run() {
//    				createAndShowGUI();
//    			}
//    		}
//        );
    }
}

/**
 * ToWebPlugin
 * @author Robin Mills
 */
public class ToWebPlugin extends Plugin {
    @Override
    public PluginAction[] getActions()
    {
        return new PluginAction [] {new ToWebAction()};
    }

    public class Photo extends Object {
        public String file;
        public String name;
        public Photo(String f,String n) {
            this.file = f;
            this.name = n;
        }
    }

    public class ToWebAction extends PluginAction
    {
        public ToWebAction()
        {
           putPropertyValue(Property.NAME, "ToWebPlugin...");
           putPropertyValue(Property.MENU, "Tools");
           // Enables the action by default
           setEnabled(true);
            String path = ToWebPlugin.class.getProtectionDomain().getCodeSource().getLocation().getPath();
            try {
                pluginDir     = URLDecoder.decode(path, "UTF-8");
                pluginDir     = Paths.get(pluginDir).getParent().toString();
                templateDir   = Paths.get(pluginDir).resolve("ToWebPlugin").toString();
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            }
        }

        private String pluginDir   ="";
        private String templateDir ="";

        private String unCamel(String s)
        {
            return s.replaceAll("(.)([A-Z])","$1 $2").replaceAll("\\s+", " ");
        }

        private boolean saveImage(BufferedImage image,String pathName,String fileType)
        {
            boolean result=false;
            try {
                ImageIO.write(image, fileType, new File(pathName));
                result=true;
            } catch (IOException e) {
                System.err.println("Bad News: Exception in ToWebPlugin.java");
                e.printStackTrace();
            }
            return result;
        }

        // http://stackoverflow.com/questions/5688042/how-to-convert-a-java-string-to-an-ascii-byte-array
//      private byte[] toBytes(String mString)
//      {
//          int len = mString.length(); // Length of the string
//          byte[] result = new byte[len];
//          for (int i = 0; i < len; ++i) {
//             char c = mString.charAt(i);
//             result[i]= (byte) c;
//          }
//          return result;
//      }

        private boolean writeOut(DataOutputStream out,String s,boolean bClose)
        {
            boolean result=false;
            try {
                out.write(s.getBytes("US-ASCII"));
                result=true;
                if ( bClose ) out.close();
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
            return result;
        }

        private boolean writeOut(String path,String s,boolean bClose)
        {
            boolean          result = false;
            DataOutputStream out    = openStream(path);
            if ( out != null ) {
                result = writeOut(out,s,bClose);
            }
            return result;
        }

        private DataOutputStream openStream(String path)
        {
            DataOutputStream result = null;
            try {
                result = new DataOutputStream(new BufferedOutputStream(new FileOutputStream(path)));
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            }
            return result;
        }

        private ZipInputStream openJar(URL jar)
        {
            ZipInputStream result=null;
            try {
                result = new ZipInputStream(jar.openStream());
            } catch (IOException e) {
                e.printStackTrace();
            }
            return result;
        }

        private boolean paintPlan(PlanComponent plan,int width,int height,String name,Path directory,String ext,String type)
        {
            BufferedImage image = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);
            Graphics2D    g     = image.createGraphics();
            Rectangle2D   bounds= plan.getPlanBounds();
            String        file  = name + ext;
            String        path  = directory.resolve(file).toString();

            double scalex       = (double) width  / bounds.getWidth() ;
            double scaley       = (double) height / bounds.getHeight();
            float  scale1       = plan.getScale();
            plan.setScale( scalex < scaley ? (float) scalex : (float) scaley);
            plan.setSize(width, height);
            plan.paint(g);
            plan.setScale(scale1);
            return saveImage(image,path,type);
        }

        private boolean paintCamera(Camera camera,int width,int height,Path directory,String ext,String type)
        {
            Home            home  = getHome();
            home.setCamera (camera);
            HomeComponent3D comp  = new HomeComponent3D(home);
            BufferedImage   image = comp.getOffScreenImage(width,height);
            String          file  = camera.getName() + ext;
            String          path  = directory.resolve(file).toString();
            return saveImage(image,path,type);
        }

        public String myLabel = "";
		void setLabel(String string) {
			myLabel = string;
		}

        @Override
        public void execute()
        {
        	final JOptionPane optionPane = new JOptionPane(
                    "The only way to close this dialog is by\n"
                    + "pressing one of the following buttons.\n"
                    + "Do you understand?",
                    JOptionPane.QUESTION_MESSAGE,
                    JOptionPane.YES_NO_OPTION);

    		Frame frame = null;
			final JDialog dialog = new JDialog(frame , 
		                                 "Click a button",
		                                 true);
		    dialog.setContentPane(optionPane);
		    dialog.setDefaultCloseOperation(
		        JDialog.DO_NOTHING_ON_CLOSE);
		    dialog.addWindowListener(new WindowAdapter() {
		        public void windowClosing(WindowEvent we) {
		            setLabel("Thwarted user attempt to close window.");
		        }
		    });
		    optionPane.addPropertyChangeListener(
		        new PropertyChangeListener() {
		            public void propertyChange(PropertyChangeEvent e) {
		                String prop = e.getPropertyName();
		
		                if (dialog.isVisible() 
		                 && (e.getSource() == optionPane)
		                 && (prop.equals(JOptionPane.VALUE_PROPERTY))) {
		                    //If you were going to check something
		                    //before closing the window, you'd do
		                    //it here.
		                    dialog.setVisible(false);
		                }
		            }

					@SuppressWarnings("unused")
					public void propertyChange1(PropertyChangeEvent evt)
					{
						String x= String.format("propertyChange1 %d",this.counter++);
			            System.out.println(x);
					}
				    public int counter = 0;
		        });
		    dialog.pack();
		    dialog.setVisible(true);
		
		    int value = ((Integer)optionPane.getValue()).intValue();
		    if (value == JOptionPane.YES_OPTION) {
		        setLabel("Good.");
		    } else if (value == JOptionPane.NO_OPTION) {
		        setLabel("Try using the window decorations "
		                 + "to close the non-auto-closing dialog. "
		                 + "You can't!");
		    }
            // JOptionPane.showMessageDialog(null, "myLabel = " + this.myLabel);
        
            Home             home  = getHome();
            UserPreferences  prefs = getUserPreferences();
            
            // HomeController   hc    = getHomeController();
            // HomeController3D hc3d  = hc.getHomeController3D();
            // hc3d.getView().setProjectionPolicy(javax.media.j3d.View.PARALLEL_PROJECTION);
            // hc3d.getView().setProjectionPolicy(javax.media.j3d.View.PARALLEL_PROJECTION);
            // hc3d.getViewer().getView().setProjectionPolicy(PARALLEL_PROJECTION);
        	//  
        	//ToWebPluginPanel panel = new ToWebPluginPanel(home,prefs,new PhotosController());
            String[] args = { "how","now"};
            ListDemo.main(args);

            // create directory for output 
            Path    Directory = Paths.get(System.getProperty("user.home")).resolve("Documents").resolve("ToWebPlugin");
            Path    Images    = Directory.resolve("Images"); (new File(Images.toString())).mkdirs();
            Path    Thumbs    = Directory.resolve("Thumbs"); (new File(Thumbs.toString())).mkdirs();

            String  message   = ""    ;

            int     twidth    = 240   ;
            int     theight   = 160   ;
            int     iwidth    = 720   ;
            int     iheight   = 480   ;

            String  type      = "PNG" ;
            String  ext       = ".png";

            boolean bCameras  = true  ;
            boolean bLevels   = true  ;

            boolean bWroteHTML= false ;

            String  template  = "page"; 

            try {
                // find templates in the jar
                // http://stackoverflow.com/questions/1429172/how-do-i-list-the-files-inside-a-jar-file
                String         pageString = "";
                CodeSource     src = ToWebPlugin.class.getProtectionDomain().getCodeSource();
                ZipInputStream zip = null;
                if (src != null) 
                    zip = openJar(src.getLocation());

                while( zip != null ) {
                    ZipEntry entry = null;
                    try {
                        entry = zip.getNextEntry();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    if (entry != null) {
                        String entryName = entry.getName();
                        int    size      = 80000 ; // (int) entry.getSize() is usually -1 !
                        if ( pageString.length() == 0 && entryName.endsWith(template + ".st") ) {
                            while ( size > 0 ) {
                                byte[] bytes = new byte[size];
                                size = zip.read(bytes, 0,size);
                                if ( size > 0 ) pageString += new String( bytes,0, size, "UTF-8" );                             
                            }
                        }
                    } else {
                        zip.close();
                        zip = null;
                    }
                }

                // enumerate files in templateDir (overwrite templates in the jar)
//              File   textFolder = new File(templateDir);
//              File[] stFiles    = textFolder.listFiles( new FileFilter() {
//                     public boolean accept( File file ) {
//                         return file.getName().endsWith(".st");
//                     }
//              });

                ArrayList<Photo> photos = new ArrayList<Photo>();

                // paint with every camera
                if ( bCameras ) {
                    // save camera
                    Camera       camera1= getHome().getCamera();
                    for ( Camera camera : getHome().getStoredCameras() ) {
                        if ( paintCamera(camera,iwidth,iheight,Images,ext,type)
                        &&   paintCamera(camera,twidth,theight,Thumbs,ext,type)
                        ) {
                            String name = camera.getName();
                            String file = name + ext;
                            message    += String.format("%s\n",unCamel(name));
                            photos.add(new Photo(file,unCamel(name)));
                        }
                    }
                    // restore camera
                    home.setCamera(camera1);
                }

                // paint every level
                if ( bLevels ) {
                    Level level1 = home.getSelectedLevel();
                    for ( Level level : home.getLevels() ) {
                        home.setSelectedLevel(level);
                        PlanComponent plan  = new PlanComponent(home,prefs, null);
                        String name = level.getName() ;
                        String file = name + ext;
                        if ( paintPlan(plan,iwidth,iheight,name,Images,ext,type)
                        &&   paintPlan(plan,twidth,theight,name,Thumbs,ext,type)
                        ) {
                            message += String.format("%s\n", unCamel(name));
                            photos.add(new Photo(file,unCamel(name)));
                        }
                    }
                    home.setSelectedLevel(level1);
                }

                if ( !photos.isEmpty() ) {
                    // use the template to generate the code
                    STRawGroupDir ts = new STRawGroupDir(templateDir,'$','$');
                    ST            t2 = ts.getInstanceOf(template); // load page.st
                    // if template is not on the disk, write it from zip file to the output directory
                    if ( t2 == null ) {
                        String path = Directory.resolve(template).toString() + ".st";
                        writeOut(path,pageString,true);
                        ts = new STRawGroupDir(Directory.toString(),'$','$');
                        t2 = ts.getInstanceOf(template); // load page.st
                    }
                    t2.add("title", "Generated by ToWebPlugin");
                    t2.add("photos",photos);

                    String name = "index.html";
                    String path = Directory.resolve(name).toString();
                    bWroteHTML  = writeOut(path,t2.render(),true);
                    if ( !bWroteHTML ) message = "unable to write HTML " + path;
                }
            } catch ( IOException e) {
                e.printStackTrace();
            }

            // inform user
            if ( bWroteHTML ) {
                message = "images:\n" + message;
            } else if ( message.length() == 0 ) {
                message = "no images have been written!";
            }
            JOptionPane.showMessageDialog(null, message);
            ListDemo.killGUI();
        }
    }
}
