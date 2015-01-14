package com.clanmills.sh3d.ToWebPlugin;

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
import java.util.Vector;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;
import java.util.List;

import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.FileSystems;
import java.nio.file.Path;
import java.nio.file.Paths;

import javax.imageio.ImageIO;
import javax.swing.*;

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

import java.awt.*;
import java.awt.event.*;
import java.awt.Graphics2D;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;

import org.stringtemplate.v4.*;

import com.eteks.sweethome3d.model.Camera;
import com.eteks.sweethome3d.model.Home;
import com.eteks.sweethome3d.model.Level;
import com.eteks.sweethome3d.model.UserPreferences;
import com.eteks.sweethome3d.plugin.Plugin;
import com.eteks.sweethome3d.plugin.PluginAction;
import com.eteks.sweethome3d.swing.HomeComponent3D;
import com.eteks.sweethome3d.swing.PlanComponent;


/**
 * ToWebPlugin
 * @author Robin Mills
 */

// http://stackoverflow.com/questions/4545937/java-splitting-the-filename-into-a-base-and-extension
class Filename
{
	private String fullPath;
	private char   pathSeparator;
	private char   extensionSeparator;

	public Filename(String str, char sep, char ext) {
	    fullPath           = str;
	    pathSeparator      = sep;
	    extensionSeparator = ext;
	}
	
	public Filename(File file,char sep, char ext)
	{
		fullPath= file.toString();
	    pathSeparator      = sep;
	    extensionSeparator = ext;
	}

	public String extension() {
	    int dot = fullPath.lastIndexOf(extensionSeparator);
	    return fullPath.substring(dot + 1);
	}

	public String filename() { // gets filename without extension
	    int dot = fullPath.lastIndexOf(extensionSeparator);
	    int sep = fullPath.lastIndexOf(pathSeparator);
	    return fullPath.substring(sep + 1, dot);
	}

	public String path() {
		int sep = fullPath.lastIndexOf(pathSeparator);
		return fullPath.substring(0, sep);
	}
}

public class ToWebPlugin extends Plugin {
	
	public	static class ToWebPluginMonitor extends JPanel
			implements ActionListener,
			PropertyChangeListener {

		/**
		 * 
		 */
		private static final long serialVersionUID = 276503287;
		private ProgressMonitor	  progressMonitor;
		private JButton			  startButton;
		private JTextArea		  storyArea;
		private JLabel			  storyLabel;
		private Boolean           storyEditable;
		private Task			  task;
		private UserPreferences	  prefs;
		private Home			  home;
		private ToWebPluginWorker worker;
		public	String			  story;
		public  JRadioButton      dummyButton;
		JList<String>	          viewList;
		
		DefaultListModel<String>  viewModel = new DefaultListModel<String>();

	    Vector<String> getTemplates()
		{
			Vector<String> result = new Vector<String>();
			try {
				char   sep         = FileSystems.getDefault().getSeparator().charAt(0);
				char   dot         = '.';
				String want        = "st";
				String jarPath	   = ToWebPlugin.class.getProtectionDomain().getCodeSource().getLocation().getPath();
				String pluginDir   = URLDecoder.decode(jarPath, "UTF-8");
					   pluginDir   = Paths.get(pluginDir).getParent().toString();
				String templateDir = Paths.get(pluginDir).resolve("templates").toString();
				File[] files	   = new File(templateDir).listFiles();
				for ( File file : files) {
					Filename filename = new Filename(file,sep,dot);
					if ( filename.extension().equals(want) )	result.add(filename.filename());
				//  System.out.println("templates:" + filename.filename().toString() + " extension:" + filename.extension());
				}
			} catch (UnsupportedEncodingException e) {
				e.printStackTrace();
			}
			return result;
		}

		public Vector<String> getViews(Home home)
		{
		    Vector<String> views = new Vector<String>();
		 
 		    for (Camera camera : home.getStoredCameras() ) {
			    String name = camera.getName();
			    System.out.println(name);
			    views.add(name);
		    }
 		    return views;
		}

		void sleep(int millisecs) throws InterruptedException
		{
			Thread.sleep(millisecs);
		}

		void sleeps(int millisecs)
		{
			try {
				sleep(millisecs);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}

		class Task extends SwingWorker<Void, Void> {
			Home			home;
			UserPreferences prefs;
			String			story;
			String			storyLabelText;
			Boolean         bDummy;

			@Override
			public Void doInBackground() {
				// save camera
				Camera camera1	= home.getCamera();

				int	   max		= home.getStoredCameras().size() -1;
				int	   progress = 0;
				int	   percent	= 0;
				storyLabelText	= storyLabel.getText();
				storyLabel.setText("Output:");
				worker.bDummy   = bDummy;
				setProgress(0);
				try {
					sleep(500);
					while (progress < max  && !isCancelled()) {
						Camera camera  = home.getStoredCameras().get(progress);
						String name	   = camera.getName();
						percent		   = progress*100/max;
						String message = String.format("View: %s	(%d%%)\n", name,percent);
						progressMonitor.setNote(message);
						progressMonitor.setProgress(percent);
						setProgress(percent);
						System.out.printf("doInBackground() Camera:%s (%d of %d) ",name,progress,max);
						worker.execute(home,prefs,progress);
						progress++;
						sleep(100);
					}
					if ( progress == max ) {
						percent=99;
						progressMonitor.setNote("Generating HTML\n");
						progressMonitor.setProgress(percent);
						setProgress(percent);
						sleep(100);
						worker.execute(home, prefs, -1);
						sleep(500);
					}
				} catch (InterruptedException ignore) { progress = max + 100 ; }

				// restore camera
				home.setCamera(camera1);
				return null;
			}

			@Override
			public void done() {
				storyArea.setText(story);
				storyArea.setEditable(storyEditable);
				storyLabel.setText(storyLabelText);
				startButton.setEnabled(true);
				progressMonitor.setProgress(100);
			}
		}

		// http://stackoverflow.com/questions/9851688/how-to-align-left-or-right-inside-gridbaglayout-cell
		private static final Insets WEST_INSETS = new Insets(5, 0, 5, 5);
		private static final Insets EAST_INSETS = new Insets(5, 5, 5, 0);
		private GridBagConstraints createGbc(int x, int y) {
			GridBagConstraints gbc = new GridBagConstraints();
			gbc.gridx      = x;
			gbc.gridy      = y;
			gbc.gridwidth  = 1;
			gbc.gridheight = 1;

			gbc.anchor     = (x == 0) ? GridBagConstraints.WEST : GridBagConstraints.EAST;
			gbc.fill       = (x == 0) ? GridBagConstraints.BOTH
				                      : GridBagConstraints.HORIZONTAL
				                      ;

			gbc.insets 	   = (x == 0) ? WEST_INSETS : EAST_INSETS;
			gbc.weightx    = (x == 0) ? 0.1         : 1.0;
			gbc.weighty    = 1.0;
			return gbc;
		 }

		public JLabel addRow(int row,String label,JComponent component)
		{
			 JLabel result = new JLabel(label, JLabel.LEFT);
			 add(result	   , createGbc(0,row));
			 add(component , createGbc(1,row));
			 return result;
		}
		
		public ToWebPluginMonitor(Home home_,Path path)
		{
			 super(new GridBagLayout());

			 home=home_;

			 int row = 0 ;
			 setLayout(new GridBagLayout());
			 setBorder(BorderFactory.createCompoundBorder
			 (	BorderFactory.createTitledBorder("")//ToWebPlugin Control Panel")
			 ,	BorderFactory.createEmptyBorder(5, 5, 5, 5)
			 ));

			 //++++++++++++++++++++++++++++++ 
			 JPanel viewPanel = new JPanel();
			 viewPanel.setOpaque(false);

			 JPanel	   viewOrderButtons = new JPanel();
			 viewOrderButtons.setOpaque(false);
			 BoxLayout viewOrderButtonsLayout = new BoxLayout(viewOrderButtons, BoxLayout.Y_AXIS);
			 viewOrderButtons.setBorder(BorderFactory.createTitledBorder("View order:"));
			 viewOrderButtons.setLayout(viewOrderButtonsLayout);
			 
			 JButton button;
			 
			 button = new JButton("up");
			 button.addActionListener(this);
			 button.setActionCommand("up");
			 viewOrderButtons.add(button);

			 button = new JButton("ignore");
			 button.addActionListener(this);
			 button.setActionCommand("ignore");
			 viewOrderButtons.add(button);

			 button = new JButton("down");
			 button.addActionListener(this);
			 button.setActionCommand("down");
			 viewOrderButtons.add(button);

			 JPanel	   viewSortButtons = new JPanel();
			 viewSortButtons.setOpaque(false);
			 BoxLayout viewButtonsLayout = new BoxLayout(viewSortButtons, BoxLayout.Y_AXIS);
			 viewSortButtons.setBorder(BorderFactory.createTitledBorder("Sort by:"));
			 viewSortButtons.setLayout(viewButtonsLayout);
			 viewSortButtons.add(new JButton("alpha"  ));
			 viewSortButtons.add(new JButton("none"	  ));
			 viewSortButtons.add(new JButton("reverse"));
			 
			 viewList = new JList<String>(viewModel);
			 Vector<String> views = getViews(home);
			 for (int i = 0; i < views.size() ; i++) {
				  viewModel.addElement(views.get(i));
		     }

			 viewList.setSelectionMode(ListSelectionModel.SINGLE_INTERVAL_SELECTION);
			 viewList.setSelectedIndex(0);
			 viewList.setLayoutOrientation(JList.VERTICAL);
			 viewList.setVisibleRowCount(-1);
			 JScrollPane viewScroller = new JScrollPane(viewList);
			 viewScroller.setPreferredSize(new Dimension(250, 150));
			 viewPanel.add(viewScroller);
			 viewPanel.add(viewSortButtons);

			 viewPanel.setOpaque(false);
			 viewPanel.setLayout(new GridBagLayout() );
			 viewPanel.add(viewOrderButtons);
			 viewPanel.add(viewSortButtons);
			 
			 addRow(++row,"Views:",viewPanel);

			 //++++++++++++++++++++++++++++++ 
			 String story		  = "";
			 try {
					List<String> lines = Files.readAllLines(path,Charset.forName("UTF-8"));
					int		line  = 0 ;
					while ( lines != null && line  < lines.size() ) {
						story = story + lines.get(line).toString() + '\n';
						line  += 1;
					}
			 } catch (IOException e) {
					e.printStackTrace();
					story = "now is the time\nfor all good men\nto come to the aid\nof the party";
			 }

			 storyArea = new JTextArea(25,50);
			 storyArea.setText(story);
			 storyArea.setEditable(true);
			 storyArea.setMargin(new Insets(5,5,5,5));

			 storyLabel = addRow(++row,"Story:",storyArea);

			 //++++++++++++++++++++++++++++++ 
			 JPanel templatePanel = new JPanel();
			 templatePanel.setOpaque(false);
			 
			 JComboBox<String> templateComboBox = new JComboBox<String>(getTemplates());
			 templateComboBox.setAutoscrolls(true);
			 templatePanel.add(templateComboBox);

			 JPanel optionsPanel = new JPanel();
			 optionsPanel.setOpaque(false);
			 optionsPanel.add(new JLabel("Template:"));
			 optionsPanel.add(templatePanel);

			 optionsPanel.add(new JLabel("JSON:"));
			 JRadioButton json = new JRadioButton();
			 json.setSelected(false);
			 optionsPanel.add(json);

			 optionsPanel.add(new JLabel("bDummy:"));
			 dummyButton = new JRadioButton();
			 optionsPanel.add(dummyButton);
			 dummyButton.setSelected(true);

			 optionsPanel.add(new JLabel("Autosave:"));
			 JRadioButton autoSave = new JRadioButton();
			 autoSave.setSelected(true);
			 optionsPanel.add(autoSave);

			 optionsPanel.add(new JButton("Save now"));

			 addRow(++row,"Options:",optionsPanel);

			 //++++++++++++++++++++++++++++++ 
			 startButton = new JButton("Start");
			 startButton.setActionCommand("start");
			 startButton.addActionListener(this);
			 
			 addRow(++row,"Start:",startButton);
		}

		/**
		 * Invoked when the user presses the start button.
		 */
		public void actionPerformed(ActionEvent evt)
		{
			String cmd = evt.getActionCommand();
			if ( cmd.equals("start") ) {
				startButton.setEnabled(false);
				progressMonitor = new ProgressMonitor
						(ToWebPluginMonitor.this
						,  "ToWebPlugin: Creating Images"
						,  "", 0, 100
						);
				story = storyArea.getText();
				task  = new Task();
				task.story = story;
				storyEditable = storyArea.isEditable();
				storyArea.setEditable(false);
				storyArea.setText("");
	
				task.home			 = home ;
				task.prefs			 = prefs;
				task.bDummy          = dummyButton.isSelected();
				task.addPropertyChangeListener(this);
				task.execute();
			}
			if ( cmd.equals("ignore") ) {
				String ignored = " *ignored* ";
				String value = viewList.getSelectedValue();
				int nIgnored = value.indexOf(ignored);
				value = nIgnored > 0 ? value.substring(0,nIgnored) : value + ignored ;
			    viewModel.set(viewList.getSelectedIndex(), value);
			}
			if ( cmd.equals("up") ) {
				int  selected = viewList.getSelectedIndex();
				if ( selected > 0 ) {
					String swap = viewModel.get(selected);
					viewModel.set(selected, viewModel.get(selected-1));
					viewModel.set(selected-1, swap);
					viewList.setSelectedIndex(selected-1);
					viewList.scrollRectToVisible(viewList.getCellBounds(selected-1, selected +1));
				}
			}
			if ( cmd.equals("down") ) {
				int  selected = viewList.getSelectedIndex();
				String swap = viewModel.get(selected);
				viewModel.set(selected, viewModel.get(selected+1));
				viewModel.set(selected+1 , swap);
				viewList.setSelectedIndex(selected+1);
				viewList.scrollRectToVisible(viewList.getCellBounds(selected-1, selected +1));
			}
		}

		/**
		 * Invoked when task's progress property changes.
		 */
		public void propertyChange(PropertyChangeEvent evt)
		{
			if ("progress" == evt.getPropertyName() ) {
				storyArea.append(progressMonitor.getNote());
				if (progressMonitor.isCanceled() || task.isDone()) {
					Toolkit.getDefaultToolkit().beep();
					if (progressMonitor.isCanceled()) {
						task.cancel(true);
					}
					startButton.setEnabled(true);
				}
			}
		}
	}


	/**
	 * Create the GUI and show it.
	 * For thread safety, this method should be invoked
	 * from the event-dispatching thread.
	 */
	void createAndShowGUI(ToWebPluginWorker worker,Home home,UserPreferences prefs)
	{
		//Create and set up the window.
		JFrame frame = new JFrame("ToWebPlugin"); // ToWebPlugin Control Panel");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		Path path = FileSystems.getDefault().getPath(home.getName().substring(0, home.getName().lastIndexOf('.')) + ".ToWebPlugin");

		//Create and set up the content pane.
		ToWebPluginMonitor monitor = new ToWebPluginMonitor(home,path);
		monitor.worker = worker;
		monitor.home = home;
		monitor.prefs = prefs;

		JComponent newContentPane = monitor ;
		newContentPane.setOpaque(true); //content panes must be opaque
		frame.setContentPane(newContentPane);

		//Display the window.
		frame.pack();
		frame.setVisible(true);
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

		@Override
		public void execute()
		{
			String		 name = getHome().getName();
			Path		 path = FileSystems.getDefault().getPath(name.substring(0, name.lastIndexOf('.')) + ".ToWebPlugin");
			String		 story=""; 
			List<String> lines= null;

			try {
				lines = Files.readAllLines(path,Charset.forName("UTF-8"));
				name  = path.toRealPath().toString();
			} catch (IOException e) {
				e.printStackTrace();
			}
			int		line  = 0 ;
			while ( lines != null && line  < lines.size() ) {
				story = story + lines.get(line).toString() + '\n';
				line  += 1;
			}
			System.out.println("name = " + name + "\nstory = " + story);
			javax.swing.SwingUtilities.invokeLater(new Runnable() {
				public void run() {
					createAndShowGUI(new ToWebPluginWorker(),getHome(),getUserPreferences());
				}
			});
		}
	}

	class ToWebPluginWorker {

		public class Photo extends Object {
			public String file;
			public String name;
			public Photo(String f,String n) {
				this.file = f;
				this.name = n;
			}
		}

		Home			 home;
		UserPreferences	 prefs;
		boolean          bDummy  = false ;
		ArrayList<Photo> photos	 = new ArrayList<Photo>();

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
			boolean			 result = false;
			DataOutputStream out	= openStream(path);
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
			Graphics2D	  g		= image.createGraphics();

			Rectangle2D.Double	 bounds = new Rectangle2D.Double(0,0,100,100) ; // plan.getPlanBounds();

			String		  file	= name + ext;
			String		  path	= directory.resolve(file).toString();
			System.out.println("plan image saved to " + directory.resolve(file).toAbsolutePath().toString());

			double scalex		= (double) width  / bounds.getWidth() ;
			double scaley		= (double) height / bounds.getHeight();
			float  scale1		= plan.getScale();
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
				BufferedImage	image = comp.getOffScreenImage(width,height);
				String			file  = camera.getName() + ext;
				String			path  = directory.resolve(file).toString();
				System.out.println("camera image saved to " + directory.resolve(file).toAbsolutePath().toString());
				result				  = saveImage(image,path,type);
			} catch ( Exception e) {
				System.err.println("Exception in paintCamera " + camera.getName() + " " + e.toString());
			}
			return result;
		}

		public boolean execute(Home home, UserPreferences prefs,int index)
		{
			// create directory for output 
			String jarPath	   = ToWebPlugin.class.getProtectionDomain().getCodeSource().getLocation().getPath();
			String pluginDir   = "";
			String templateDir = "";

			try {
				pluginDir	  = URLDecoder.decode(jarPath, "UTF-8");
				pluginDir	  = Paths.get(pluginDir).getParent().toString();
				templateDir	  = Paths.get(pluginDir).resolve("templates").toString();
			} catch (UnsupportedEncodingException e) {
				e.printStackTrace();
			}

			Path	Directory = Paths.get(System.getProperty("user.home")).resolve("Documents").resolve("ToWebPlugin");
			Path	Images	  = Directory.resolve("Images"); (new File(Images.toString())).mkdirs();
			Path	Thumbs	  = Directory.resolve("Thumbs"); (new File(Thumbs.toString())).mkdirs();

			String	message	  = ""		;

			int		x		  = 1		; // multiply up the size of images
			int		y		  = 1		; // divide the size of images

			int		twidth	  = 240*x/y ;
			int		theight	  = 160*x/y ;
			int		iwidth	  = 720*x/y ;
			int		iheight	  = 480*x/y ;

			String	type	  = "PNG"	;
			String	ext		  = ".png"	;

			boolean bWroteHTML= false	;
			if ( index == 0 ) photos = new ArrayList<Photo>();

			// this is temporary bodgery to speed up template debugging
			boolean bCameras  = true  ;
			boolean bLevels	  = false ;
			if ( bDummy && photos.isEmpty()  && index == -1 ) {
				photos.add(new Photo("Landing.png"		  ,"Landing"		  ));
				photos.add(new Photo("Entrance.png"		  ,"Entrance"		  ));
				photos.add(new Photo("FootOfStairs.png"	  ,"Foot Of Stairs"	  ));
				photos.add(new Photo("Bed4.png"			  ,"Bed4"			  ));
				photos.add(new Photo("OfficeFromDesk.png" ,"Office From Desk" ));
				photos.add(new Photo("Lounge.png"		  ,"Lounge"			  ));
				photos.add(new Photo("BonusRoom.png"	  ,"Bonus Room"		  ));
				photos.add(new Photo("LobbyFromDining.png","Lobby From Dining"));
				photos.add(new Photo("DiningRoom.png"	  ,"Dining Room"	  ));
				photos.add(new Photo("FromGarage.png"	  ,"From Garage"	  ));
			}

			System.out.printf("ToWebPluginWorker.execute %d\n",index);

			String	template  = "page";
			try {

				// paint camera
				if ( !bDummy && bCameras && index >= 0 ) {
					// save camera
					Camera camera = home.getStoredCameras().get(index) ;
					if ( paintCamera(home,camera,iwidth,iheight,Images,ext,type)
					&&	 paintCamera(home,camera,twidth,theight,Thumbs,ext,type)
					) {
						String name = camera.getName();
						String file = name + ext;
						message	   += String.format("%s\n",unCamel(name));
						photos.add(new Photo(file,unCamel(name)));
						System.out.println("Camera " + message);
					}
				}

				// paint every level
				if ( !bDummy && bLevels && index >= 0 ) {
					Level level1 = home.getSelectedLevel();
					for ( Level level : home.getLevels() ) {
						home.setSelectedLevel(level);
						PlanComponent plan	= new PlanComponent(home,prefs, null);
						String name = level.getName() ;
						String file = name + ext;
						if ( paintPlan(plan,iwidth,iheight,name,Images,ext,type)
						&&	 paintPlan(plan,twidth,theight,name,Thumbs,ext,type)
						) {
							message += String.format("%s\n", unCamel(name));
							photos.add(new Photo(file,unCamel(name)));
							System.out.println("Level " + message);
						}
					}
					home.setSelectedLevel(level1);
				}

				if ( !photos.isEmpty() && index == -1 ) {
					// find templates in the jar
					// http://stackoverflow.com/questions/1429172/how-do-i-list-the-files-inside-a-jar-file
					String		   pageString = "";
					CodeSource	   src = ToWebPlugin.class.getProtectionDomain().getCodeSource();
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
							int	   size		 = 80000 ; // (int) entry.getSize() is usually -1 !
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

					System.out.println("generating code");
					// use the template to generate the code
					STRawGroupDir ts = new STRawGroupDir(templateDir,'$','$');
					ST			  t2 = ts.getInstanceOf(template); // load page.st
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
					bWroteHTML	= writeOut(path,t2.render(),true);
					if ( !bWroteHTML ) message = "unable to write HTML " + path;

//					  for ( Photo photo : photos ) {
//						System.out.println("name: " + photo.name + " file: + photo.file);
//					  }

					new ProcessBuilder("ditto","/Users/rmills/Documents/ToWebPlugin/","/Users/rmills/clanmills/ToWebPlugin/").start();
					new ProcessBuilder("open" ,"http://klanmills/ToWebPlugin"						).start();
					System.out.println("ditto and open run");
				}
			} catch ( IOException e) {
				e.printStackTrace();
			}

			return true;
		}
	}
}
