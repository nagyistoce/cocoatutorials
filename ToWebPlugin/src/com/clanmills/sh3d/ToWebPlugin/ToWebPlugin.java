package com.clanmills.sh3d.ToWebPlugin;

// import java.awt.Frame;
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

import java.security.CodeSource;

import java.util.ArrayList;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;
import java.util.List;

import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.FileSystems;
import java.nio.file.Path;
import java.nio.file.Paths;

import javax.imageio.ImageIO;
//import javax.swing.JDialog;
// import javax.swing.JOptionPane;
import javax.swing.SwingWorker;

import org.stringtemplate.v4.*;

import com.eteks.sweethome3d.model.Camera;
import com.eteks.sweethome3d.model.Home;
import com.eteks.sweethome3d.model.Level;
import com.eteks.sweethome3d.model.UserPreferences;
import com.eteks.sweethome3d.plugin.Plugin;
import com.eteks.sweethome3d.plugin.PluginAction;
import com.eteks.sweethome3d.swing.HomeComponent3D;
import com.eteks.sweethome3d.swing.PlanComponent;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.text.BadLocationException;
import javax.swing.text.Document;

// import java.beans.*;
// import java.util.Random;

/* ListDemo.java requires no other files. */
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

/**
 * ToWebPlugin
 * @author Robin Mills
 */

public class ToWebPlugin extends Plugin {
	public static class ToWebPluginMonitor extends JPanel
	implements ActionListener,
	PropertyChangeListener {

		/**
		 * 
		 */
		 private static final long serialVersionUID = 276503287;
		 private ProgressMonitor progressMonitor;
		 private JButton         startButton;
		 private JTextArea       taskOutput;
		 private JTextArea       taskHelp;
		 private Task            task;
		 private UserPreferences prefs;
		 private Home            home;
		 private ToWebPluginWork work;

		 class Task extends SwingWorker<Void, Void> {
			 @Override
			 public Void doInBackground() {
				 // save camera
                 Camera       camera1= home.getCamera();

                 int max      = home.getStoredCameras().size();
				 int progress = 0;
				 setProgress(0);
				 try {
				      while (progress < max  && !isCancelled()) {
	                    Camera camera = home.getStoredCameras().get(progress);
	                    String name   = camera.getName();
	   				    int percent   = progress*100/max;
	   		   			String message = String.format("View: %d%% Current: %s\n", percent,name);
	   		   			progressMonitor.setNote(message);
	   		   			progressMonitor.setProgress(percent);
						setProgress(percent);
	                    System.out.println("doInBackground() Camera: " + name);
						work.execute(home,prefs,progress);
						progress++;
	                    Thread.sleep(100);
	                  }
				 } catch (InterruptedException ignore) { progress = max + 100 ; }
				 if ( progress == max ) work.execute(home, prefs, -1);

				 // restore camera
                 home.setCamera(camera1);
				 return null;
			 }

			 @Override
			 public void done() {
				 Toolkit.getDefaultToolkit().beep();
				 startButton.setEnabled(true);
				 progressMonitor.setProgress(0);
			 }
			 // ToWebPluginMonitor.this.work,ToWebPluginMonitor.this.home,ToWebPluginMonitor.this.prefs
			 // ToWebPluginWork work;
			 Home            home;
			 UserPreferences prefs;
			 String          story;
			 ProgressMonitor progressMonitor;
		 }

		 public String story;

		 public ToWebPluginMonitor() {
			 super(new BorderLayout());
			 // story = _story;

			 //Create the demo's UI.
			 startButton = new JButton("Start");
			 startButton.setActionCommand("start");
			 startButton.addActionListener(this);

			 taskOutput = new JTextArea(5, 60);
			 // taskOutput.setMargin(new Insets(5,5,5,5));
			 taskOutput.setEditable(true);

			 taskHelp  = new JTextArea();
			 // taskHelp.setMargin(new Insets(5,5,5,5));
			 taskHelp.setEditable(false);
			 taskHelp.setEditable(false);
			 taskHelp.setText("What a load of bollocks\nGoing on and on\nfor a few lines!");
			 taskOutput.setText(story);

			 add(new JScrollPane(taskHelp), BorderLayout.PAGE_START);
			 add(new JScrollPane(taskOutput), BorderLayout.CENTER);
			 add(startButton, BorderLayout.PAGE_END);
			 setBorder(BorderFactory.createEmptyBorder(20, 20, 20, 20));
		 }


		 /**
		  * Invoked when the user presses the start button.
		  */
		 public void actionPerformed(ActionEvent evt) {
			 progressMonitor = new ProgressMonitor(ToWebPluginMonitor.this,
					 "Running a Long Task",
					 "", 0, 100);

			 task = new Task();
			 try {
				 Document doc = taskOutput.getDocument();
				 int      l   = doc.getLength();
				 String   s   = doc.getText(0, l);
				 ToWebPluginMonitor.this.story = s;
				 task.story   = s;
				 // JOptionPane.showMessageDialog(null,s);
				 doc.remove(0, l);
			 } catch (HeadlessException | BadLocationException e) {
				 e.printStackTrace();
			 }
			 /// --- start the background task ----
//			 ToWebPluginMonitor.this.work.execute(ToWebPluginMonitor.this.home,ToWebPluginMonitor.this.prefs);

			 task.home  = this.home ;
			 task.prefs = this.prefs;
			 // task.work  = this.work ;
			 task.progressMonitor = progressMonitor;
			 task.addPropertyChangeListener(this);
			 // ToWebPluginMonitor.this.work,ToWebPluginMonitor.this.home,ToWebPluginMonitor.this.prefs
			 task.execute();
			 startButton.setEnabled(false);
		 }

		 /**
		  * Invoked when task's progress property changes.
		  */
		 public void propertyChange(PropertyChangeEvent evt) {
			 if ("progress" == evt.getPropertyName() ) {
				 taskOutput.append(progressMonitor.getNote());
				 if (progressMonitor.isCanceled() || task.isDone()) {
					 Toolkit.getDefaultToolkit().beep();
					 if (progressMonitor.isCanceled()) {
						 task.cancel(true);
						 taskOutput.append("Task canceled.\n");
					 } else {
						 taskOutput.append("Task completed.\n");
					 }
					 startButton.setEnabled(true);
				 }
			 }
		 }
	}
	
	
	/**
	 * Create the GUI and show it.  For thread safety,
	 * this method should be invoked from the
	 * event-dispatching thread.
	 */
	void createAndShowGUI(ToWebPluginWork work,Home home,UserPreferences prefs) {
		//Create and set up the window.
		JFrame frame = new JFrame("ProgressMonitorDemo");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		//Create and set up the content pane.
		ToWebPlugin.ToWebPluginMonitor monitor = new ToWebPlugin.ToWebPluginMonitor();
		JComponent newContentPane              = monitor ;
		newContentPane.setOpaque(true); //content panes must be opaque
		frame.setContentPane(newContentPane);

		//Display the window.
		frame.pack();
		frame.setVisible(true);
		monitor.work = work;
		monitor.home = home;
		monitor.prefs = prefs;
	}
	

    @Override
    public PluginAction[] getActions()
    {
        return new PluginAction [] {new ToWebPluginAction()};
    }
    
    public class ToWebPluginAction extends PluginAction
    {
        public ToWebPluginAction()
        {
           putPropertyValue(Property.NAME, "ToWebPlugin...");
           putPropertyValue(Property.MENU, "Tools");
           // Enables the action by default
           setEnabled(true);
        }

        public String myLabel = "";
        void setLabel(String string) {
            myLabel = string;
        }
        
//        class ToWebPluginPanel extends JOptionPane
//        {
//        	static final long serialVersionUID = 2887673;
//        	ToWebPluginPanel() {
//        		super("The only way to close this dialog is by\n"
//                     + "pressing one of the following buttons.\n"
//                     + "Do you understand?"
//                     ,  JOptionPane.QUESTION_MESSAGE
//                     ,  JOptionPane.YES_NO_OPTION
//                     );
//        	}
//
//            public Home             home;
//            public UserPreferences  prefs;
//            public String           story;
//            public List<String>     lines;
//            boolean            r1 = false;
//            
//            class ToWebPluginWorker extends SwingWorker<String, Void>{
//                @Override
//                public String doInBackground() {
//                    System.out.println("ToWebPluginWorker:doInBackground:starting!");
//    	        	ToWebPluginWork work = new ToWebPluginWork();
//    	        	work.execute(home, prefs);
//    	        	return "success" ;
//                }
//
//                @Override
//                protected void done() {
//                    try {
//                        System.out.println("ToWebPluginWorker:done:done!");
//                    } catch (Exception ignore) {
//                        System.out.println("ToWebPluginWorker:done:exception!");
//                    }
//                }            	
//            }
//            
//            int displayPanel() {
//                Frame frame          = null;
//                final JDialog dialog = new JDialog(frame , 
//                                             "Click a button",
//                                             true);
//                dialog.setContentPane(this);
//                dialog.setDefaultCloseOperation(
//                    JDialog.DO_NOTHING_ON_CLOSE);
//                dialog.addWindowListener(new WindowAdapter() {
//                    public void windowClosing(WindowEvent we) {
//                        setLabel("Thwarted user attempt to close window.");
//                        dialog.setTitle("I have changed");
//                    }
//                });
//                
//                final ToWebPluginWorker worker = new ToWebPluginWorker();
//                
//                dialog.setModal(true);
//                this.addPropertyChangeListener(
//                    new PropertyChangeListener() {
//                        public void propertyChange(PropertyChangeEvent e) {
//                            String prop = e.getPropertyName();
//                            System.out.println(prop);
//                            if (dialog.isVisible() && prop.equals(JOptionPane.VALUE_PROPERTY) ) {
//                             // && (e.getSource() == this)
//                             // && (prop.equals(JOptionPane.VALUE_PROPERTY))) {
//                                //If you were going to check something
//                                //before closing the window, you'd do
//                                //it here.
//
//                            	//int button = JOptionPane.YES_OPTION; // ((Integer)super.getValue()).intValue();
//                            	if ( r1  ) {
//                            		r1=false;
//                            		worker.cancel(true);
//                                    dialog.setVisible(false);
//                            	} else {
//                            		r1=true;
//                                    worker.execute();
//                            	}
//                            }
//                        }
//
//                        @SuppressWarnings("unused")
//                        public void propertyChange1(PropertyChangeEvent evt)
//                        {
//                            String x= String.format("propertyChange1 %d",this.counter++);
//                            System.out.println(x);
//                        }
//                        public int counter = 0;
//                    });
//                dialog.pack();
//                dialog.setVisible(true);
//                
//                
//                
//                return  ((Integer)this.getValue()).intValue();
//        	}
//        }
        
        //Home            home  = getHome();
        //UserPreferences prefs = getUserPreferences();
        
		@Override
        public void execute()
        {
			String       name = getHome().getName();
			Path         path = FileSystems.getDefault().getPath(name.substring(0, name.lastIndexOf('.')) + ".ToWebPlugin");
			String       story=""; 
			List<String> lines= null;
			
			try {
				lines = Files.readAllLines(path,Charset.forName("UTF-8"));
				name  = path.toRealPath().toString();
			} catch (IOException e) {
				e.printStackTrace();
			}
			int     line  = 0 ;
			while ( lines != null && line  < lines.size() ) {
				story = story + lines.get(line).toString() + '\n';
				line  += 1;
			}
			System.out.println("name = " + name + "\nstory = " + story);
 	        javax.swing.SwingUtilities.invokeLater(new Runnable() {
	            public void run() {
	            	createAndShowGUI(new ToWebPluginWork(),getHome(),getUserPreferences());
	            }
	        });

// 	        ToWebPluginPanel panel = new ToWebPluginPanel();
//	   	    panel.home  = home;
//	       	panel.prefs = prefs;
//	       	panel.story = story;
//
//			int value   // 
//	                    = panel.displayPanel();
//       		int no  = JOptionPane.NO_OPTION;
//      		int yes = JOptionPane.YES_OPTION;
//      		System.out.format("value = %d NO,YES = %d,%d\n",value,no,yes);

//      		if ( value == JOptionPane.YES_OPTION ) {
//        		ToWebPluginWork work = new ToWebPluginWork();
//        		work.execute(home, prefs);
//        	}

//              if ( value == JOptionPane.YES_OPTION ) {
//                ToWebPluginWork work = new ToWebPluginWork();
//                work.execute(getHome(),getUserPreferences());
//              }
        }
    }
    
    class ToWebPluginWork {

    	public class Photo extends Object {
            public String file;
            public String name;
            public Photo(String f,String n) {
                this.file = f;
                this.name = n;
            }
        }
    	
    	Home            home;
    	UserPreferences prefs;
    	
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
//          private byte[] toBytes(String mString)
//          {
//              int len = mString.length(); // Length of the string
//              byte[] result = new byte[len];
//              for (int i = 0; i < len; ++i) {
//                 char c = mString.charAt(i);
//                 result[i]= (byte) c;
//              }
//              return result;
//          }

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
            
            Rectangle2D.Double   bounds = new Rectangle2D.Double(0,0,100,100) ; // plan.getPlanBounds();
            
            String        file  = name + ext;
            String        path  = directory.resolve(file).toString();
            System.out.println("plan image saved to " + directory.resolve(file).toAbsolutePath().toString());

            double scalex       = (double) width  / bounds.getWidth() ;
            double scaley       = (double) height / bounds.getHeight();
            float  scale1       = plan.getScale();
            plan.setScale( scalex < scaley ? (float) scalex : (float) scaley);
            plan.setSize(width, height);
            plan.paint(g);
            plan.setScale(scale1);
            return saveImage(image,path,type);
        }

        private boolean paintCamera(Home home,Camera camera,int width,int height,Path directory,String ext,String type)
        {
        	boolean result = false;
        	try {
	            home.setCamera (camera);
	            HomeComponent3D comp  = new HomeComponent3D(home);
	            BufferedImage   image = comp.getOffScreenImage(width,height);
	            String          file  = camera.getName() + ext;
	            String          path  = directory.resolve(file).toString();
	            System.out.println("camera image saved to " + directory.resolve(file).toAbsolutePath().toString());
	            result                = saveImage(image,path,type);
        	} catch ( Exception e) {
        		System.err.println("Exception in paintCamera " + camera.getName() + " " + e.toString());
        	}
            return result;
        }
        
        ArrayList<Photo> photos = new ArrayList<Photo>();

        public boolean execute(Home home, UserPreferences prefs,int index)
        {

            // create directory for output 
            String jarPath = ToWebPlugin.class.getProtectionDomain().getCodeSource().getLocation().getPath();
            try {
                pluginDir     = URLDecoder.decode(jarPath, "UTF-8");
                pluginDir     = Paths.get(pluginDir).getParent().toString();
                templateDir   = Paths.get(pluginDir).resolve("ToWebPlugin").toString();
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            }

            Path    Directory = Paths.get(System.getProperty("user.home")).resolve("Documents").resolve("ToWebPlugin");
            Path    Images    = Directory.resolve("Images"); (new File(Images.toString())).mkdirs();
            Path    Thumbs    = Directory.resolve("Thumbs"); (new File(Thumbs.toString())).mkdirs();

            String  message   = ""    ;
            
            int	    x         = 2     ; // multiply up the size of images

            int     twidth    = 240*x ;
            int     theight   = 160*x ;
            int     iwidth    = 720*x ;
            int     iheight   = 480*x ;

            String  type      = "PNG" ;
            String  ext       = ".png";

            boolean bCameras  = true  ;
            boolean bLevels   = false ;

            boolean bWroteHTML= false ;
            System.out.printf("ToWebPluginWork.execute %d\n",index);

            String  template  = "page";
            try {
                // find templates in the jar
                // http://stackoverflow.com/questions/1429172/how-do-i-list-the-files-inside-a-jar-file
                String         pageString = "";
                CodeSource     src = ToWebPlugin.class.getProtectionDomain().getCodeSource();
                ZipInputStream zip = openJar(src.getLocation());

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

                if ( index == 0 ) photos = new ArrayList<Photo>();

                // paint camera
                if ( bCameras ) {
                    // save camera
                    Camera camera = home.getStoredCameras().get(index) ;
                    if ( paintCamera(home,camera,iwidth,iheight,Images,ext,type)
                    &&   paintCamera(home,camera,twidth,theight,Thumbs,ext,type)
                    ) {
                            String name = camera.getName();
                            String file = name + ext;
                            message    += String.format("%s\n",unCamel(name));
                            photos.add(new Photo(file,unCamel(name)));
                            System.out.println("Camera " + message);
                    }
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
                            System.out.println("Level " + message);
                        }
                    }
                    home.setSelectedLevel(level1);
                }

                if ( !photos.isEmpty() && index == -1 ) {
                    System.out.println("generating code");
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

//            // inform user
//            if ( bWroteHTML ) {
//                message = "images:\n" + message;
//            } else if ( message.length() == 0 ) {
//                message = "no images have been written!";
//            }
//            JOptionPane.showMessageDialog(null, message);
            // ListDemo.killGUI();
        	return true;
        }
    }
}
