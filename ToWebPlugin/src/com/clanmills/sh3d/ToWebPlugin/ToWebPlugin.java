package com.clanmills.sh3d.ToWebPlugin;

import org.json.simple.*;
import org.json.simple.parser.*;

import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.io.UnsupportedEncodingException;
import java.net.MalformedURLException;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.URL;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashSet;
import java.util.Set;
import java.util.Vector;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

import javax.imageio.ImageIO;
import javax.swing.*;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.awt.*;
import java.awt.event.*;
import java.awt.Graphics2D;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;

import org.stringtemplate.v4.*;

import com.eteks.sweethome3d.io.FileUserPreferences;
import com.eteks.sweethome3d.model.Camera;
import com.eteks.sweethome3d.model.Home;
import com.eteks.sweethome3d.model.Level;
import com.eteks.sweethome3d.model.UserPreferences;
import com.eteks.sweethome3d.plugin.Plugin;
import com.eteks.sweethome3d.plugin.PluginAction;
import com.eteks.sweethome3d.swing.HomeComponent3D;
import com.eteks.sweethome3d.swing.PlanComponent;
import com.eteks.sweethome3d.viewcontroller.HomeController3D;

/**
 * ToWebPlugin
 * @author Robin Mills
 */

/**
 * FileUnzip extracts files and directories of a standard zip file to
 * a destination directory.
 * @author www.codejava.net
 *
 */
// http://www.codejava.net/java-se/file-io/programmatically-extract-a-zip-file-using-java
class FileUnzip {
	/**
	 * Size of the buffer to read/write data
	 */
	private static final int BUFFER_SIZE = 4096;
	/**
	 * Extracts a zip file specified by the zipFilePath to a directory specified by
	 * destDirectory (will be created if does not exists)
	 * @param zipFilePath
	 * @param destDirectory
	 * @throws IOException
	 */
	public Boolean extract(String zipFilePath, String destDirectory) {
		
		Boolean result = false;
		try {
			File destDir = new File(destDirectory);
			if (!destDir.exists()) {
				destDir.mkdir();
			}
			ZipInputStream zipIn = new ZipInputStream(new FileInputStream(zipFilePath));
			ZipEntry entry = zipIn.getNextEntry();
			result         = entry != null;
			// iterates over entries in the zip file
			while (entry != null) {
				String filePath = destDirectory + File.separator + entry.getName();
				if (!entry.isDirectory()) {
					// if the entry is a file, extracts it
					extractFile(zipIn, filePath);
				} else {
					// if the entry is a directory, make the directory
					File dir = new File(filePath);
					dir.mkdir();
				}
				zipIn.closeEntry();
				entry = zipIn.getNextEntry();
			}
			zipIn.close();
		} catch ( IOException e) {
			result = false;
		}
		
		return result;
	}

	/**
	 * Extracts a zip entry (file entry)
	 * @param zipIn
	 * @param filePath
	 * @throws IOException
	 */
	private void extractFile(ZipInputStream zipIn, String filePath) throws IOException {
		BufferedOutputStream bos = new BufferedOutputStream(new FileOutputStream(filePath));
		byte[] bytesIn = new byte[BUFFER_SIZE];
		int read = 0;
		while ((read = zipIn.read(bytesIn)) != -1) {
			bos.write(bytesIn, 0, read);
		}
		bos.close();
	}
}

// http://stackoverflow.com/questions/4545937/java-splitting-the-filename-into-a-base-and-extension
class Filename
{
	private String fullPath;
	private char   pathSeparator;
	private char   extensionSeparator;

	public Filename(String str, char sep, char ext) {
		fullPath		   = str;
		pathSeparator	   = sep;
		extensionSeparator = ext;
	}

	public Filename(String str){
		fullPath		   = str;
		pathSeparator	   = File.separator.charAt(0);
		extensionSeparator = '.';
	}

	public Filename(File file,char sep, char ext)
	{
		fullPath= file.toString();
		pathSeparator	   = sep;
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

	public String readAsString()
	{
		String result = "";
		try {
			File fileDir = new File(fullPath);
			BufferedReader in = new BufferedReader(  new InputStreamReader(
	                            new FileInputStream(fileDir), "UTF8")
			);
			String line;
			while ((line = in.readLine()) != null) {
			    result = result + line + '\n';
			}
	        in.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
		return result;
	}

	public static String join(String path1, String path2)
	{
	    File file1 = new File(path1);
	    File file2 = new File(file1, path2);
	    return file2.getPath();
	}

	public static String join(String path1, String path2,String path3)
	{
	    return Filename.join(Filename.join(path1,path2),path3);
	}

	public static String join(String path1, String path2,String path3,String path4)
	{
	    return Filename.join(Filename.join(path1,path2,path3),path4);
	}

	public static String mkdirs(String path){
		(new File(path)).mkdirs();
		return path;
	}

	// http://stackoverflow.com/questions/3775694/deleting-folder-from-java
	public static boolean deleteDirectory(File directory)
	{
	    if(directory.exists()){
	        File[] files = directory.listFiles();
	        if( files != null ){
	            for( File file : files ) {
	                if(file.isDirectory()) {
	                    deleteDirectory(file);
	                }
	                else {
	                    file.delete();
	                }
	            }
	        }
	    }
	    return directory.delete() ;
	}

	public static String mkdirs(String path,Boolean blast)
	{
		if ( blast ) rmdir(path);
		return mkdirs(path);
	}

	public static String rmdir(String path){
		deleteDirectory (new File(path));
		return path;
	}
}

public class ToWebPlugin extends Plugin {

    public Home              home               = getHome();
    public String            ignored            = " *ignored*";

	public	static final String sToWebPlugin	= "ToWebPlugin";
	public  static final String sToWebPluginJar = "ToWebPlugin.jar";

    // the following members are initialized args to createAndShowGUI(...)
    public TheJob            theJob             = null;
    public HomeController3D  homeController3D   = null;
    public UserPreferences   prefs              = null;
    public String            jsonPath           = null;
    public String            jarPath            = null;

    // frame is created in createAndShowGUI(...)
    public JFrame            frame            = null;

	public Boolean setView(String name)
	{
		if ( name != null ) {
			int	 nIgnored            = name.indexOf(ignored);
			if ( nIgnored > 0 ) name = name.substring(0,nIgnored);

			for ( Camera camera : this.getHome().getStoredCameras() ) {
				if ( camera.getName().equals(name) ) try {
					if ( camera.getClass().getName().equals("com.eteks.sweethome3d.model.ObserverCamera") ) {
						home.setCamera(camera);
					} else {
						homeController3D.goToCamera(camera);
					}
					return true ;
				} catch (Exception e) {
					System.out.println("setCamera " + name + " failed");
				}
			}
		}
		return false;
	}

    public	static class Panel extends JPanel
			implements	ActionListener,
						WindowListener,
						PropertyChangeListener,
						MouseListener,
						KeyListener ,
						ListSelectionListener
	{
		private static final long	serialVersionUID = 276503287;

		private ProgressMonitor		progressMonitor;
		private JFrame				frame;
		private JButton				startButton;
		private JTextArea			storyArea;
		private JLabel				storyLabel;
		private Boolean				storyEditable;
		public  String              ignored;
		public  ToWebPlugin         plugin;

		HomeController3D            homeController3D;

		private Worker		        worker;
		private UserPreferences		prefs;
		private Home				home;
		private String				jsonPath;
		private String              jarPath;
		private TheJob	            theJob;
		public	String				story;
		JList<String>				viewList;
		JComboBox<String>			templateList;
		DefaultListModel<String>	viewModel = new DefaultListModel<String>();

		class Worker extends SwingWorker<Void, Void> {
			Home			home;
			UserPreferences prefs;
			String			story;
			String			storyLabelText;
			String			template;
			Boolean			bDummy;
			Vector<String>	views;

			@Override
			public Void doInBackground() {
				
				int	   max		= views.size();
				if (   max < 1 ) {
					messageBox("There are no views");
					return null;
				}

				// save camera
				Camera camera	= home.getCamera();

				int	   progress = 0;
				int	   percent	= 0;
				storyLabelText	= storyLabel.getText();
				storyLabel.setText("Output:");
				theJob.bDummy	= bDummy;
				theJob.views	= views;
				setProgress(0);
				try {
					theJob.template = template;
					theJob.story    = story;
					while (progress < max  && !isCancelled()) {
						String view	   = views.get(progress);
						percent		   = progress*100/max;
						String message = String.format("%d%% View: %s\n",percent, view);
						progressMonitor.setNote(message);
						progressMonitor.setProgress(percent);
						setProgress(percent);
						System.out.printf("doInBackground() View:%s (%d of %d) ",view,progress+1,max);
						theJob.generateCode(home,prefs,progress++);
					}
					if ( progress == max ) {
						percent=99;
						progressMonitor.setNote("Generating HTML\n");
						progressMonitor.setProgress(percent);
						setProgress(percent);
						theJob.generateCode(home, prefs, -1);
						setProgress(100);
					}
				} catch (Exception e) { progress = max + 100 ; }

				// restore camera
				home.setCamera(camera);
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

		public void messageBox(String m)
		{
			JOptionPane.showMessageDialog(null,m);
		}

		public Vector<String> getTemplates()
		{
			Vector<String> result = new Vector<String>();
			try {
				char	sep			= File.separator.charAt(0);
				char	dot			= '.';
				String	want		= "zip";
				String	homedir		= javax.swing.filechooser.FileSystemView.getFileSystemView().getDefaultDirectory().toString();
				String	templateDir = Filename.join(homedir,"ToWebPlugin","templates");

//				File[] files	    = new File(templateDir).listFiles();
//				if ( files != null ) for ( File file : files) {
//					Filename filename = new Filename(file,sep,dot);
//					if ( filename.extension().equals(want) ) {
//						result.add(filename.filename());
//					}
//				//	System.out.println("templates:" + filename.filename().toString() + " extension:" + filename.extension());
//				}

				// unzip the jar to a temporary directory
				String tmp = Filename.join(System.getProperty("java.io.tmpdir"),ToWebPlugin.sToWebPlugin);
				Filename.mkdirs(tmp,true);
				new FileUnzip().extract(jarPath,tmp);

				// and enumerates the zips in the jar's templates directory
				File[] files	    = new File(Filename.join(tmp,"templates")).listFiles();
				if ( files != null ) for ( File file : files) {
					Filename filename = new Filename(file,sep,dot);
					if ( filename.extension().equals(want) ) {
						result.add(filename.filename());
					}
				//	System.out.println("templates:" + filename.filename().toString() + " extension:" + filename.extension());
				}

				if ( result.size() == 0 ) messageBox("no templates found in " + templateDir);
			} catch (Exception e) {
				e.printStackTrace();
			}
			return result;
		}

		public Vector<String> getViewsInModel(Home home)
		{
			Vector<String> views = new Vector<String>();

			for (Camera camera : home.getStoredCameras() ) {
				String name = camera.getName();
				//	System.out.println(name);
				views.add(name);
			}
			if ( views.size() == 0 ) messageBox("Please store some camera views in the model");
			return views;
		}

		public void fixViewUI()
		{
			Set<String> views = new HashSet<String>();
			for ( String view : getViewsInModel(home) ) {
				views.add(view);
			}

			// views in the model, but not in UI are marked ignored
			for ( int i = 0 ; i < viewModel.getSize(); i++ ) {
				String	view = viewModel.get(i);

				int	 nIgnored = ignored == null ? -1 : view.indexOf(ignored);
				if ( nIgnored > 0 ) view = view.substring(0,nIgnored);

				if ( !views.contains(view) && nIgnored < 0 ) {
					viewModel.add(viewModel.getSize(), view + ignored);
				}
				views.remove(view);
			}
			for ( String view : views ) {
				viewModel.add(viewModel.getSize(), view + ignored);
			}
		}

		// http://stackoverflow.com/questions/9851688/how-to-align-left-or-right-inside-gridbaglayout-cell
		private static final Insets WEST_INSETS = new Insets(5, 0, 5, 5);
		private static final Insets EAST_INSETS = new Insets(5, 5, 5, 0);
		private GridBagConstraints createGbc(int x, int y) {
			GridBagConstraints gbc = new GridBagConstraints();
			gbc.gridx	   = x;
			gbc.gridy	   = y;
			gbc.gridwidth  = 1;
			gbc.gridheight = 1;

			gbc.anchor	   = (x == 0) ? GridBagConstraints.WEST : GridBagConstraints.EAST;
			gbc.fill	   = (x == 0) ? GridBagConstraints.BOTH
									  : GridBagConstraints.HORIZONTAL
									  ;

			gbc.insets	   = (x == 0) ? WEST_INSETS : EAST_INSETS;
			gbc.weightx	   = (x == 0) ? 0.1			: 1.0;
			gbc.weighty	   = 1.0;
			return gbc;
		 }

		private JLabel addRow(int row,String label,JComponent component)
		{
			 JLabel result = new JLabel(label, JLabel.LEFT);
			 add(result	   , createGbc(0,row));
			 add(component , createGbc(1,row));
			 return result;
		}

		public Panel(ToWebPlugin plugin_)
		{
			 super(new GridBagLayout());
			 setOpaque(true);

			 plugin           = plugin_;
			 home             = plugin.home;
			 homeController3D = plugin.homeController3D;
			 ignored          = plugin.ignored;
			 theJob			  = plugin.theJob;
			 prefs			  = plugin.prefs;
			 frame			  = plugin.frame;
			 jsonPath         = plugin.jsonPath;
			 jarPath          = plugin.jarPath;

			 int row = 0 ;
			 setLayout(new GridBagLayout());
			 setBorder(BorderFactory.createCompoundBorder
			 (	BorderFactory.createTitledBorder("")//ToWebPlugin Control Panel")
			 ,	BorderFactory.createEmptyBorder(5, 5, 5, 5)
			 ));

			 //++++++++++++++++++++++++++++++
			 JPanel viewPanel = new JPanel();
			 viewPanel.setOpaque(false);
			 viewPanel.setLayout(new GridBagLayout() );

			 JButton button;

			 viewList = new JList<String>(viewModel);
			 Vector<String> views = getViewsInModel(home);
			 for (int i = 0; i < views.size() ; i++) {
				  viewModel.addElement(views.get(i));
			 }
			 viewList.addMouseListener(this);
			 viewList.addKeyListener(this);
			 viewList.addListSelectionListener(this);

			 viewList.setSelectionMode(ListSelectionModel.SINGLE_INTERVAL_SELECTION);
			 viewList.setSelectedIndex(0);
			 viewList.setLayoutOrientation(JList.VERTICAL);
			 viewList.setVisibleRowCount(-1);
			 JScrollPane viewScroller = new JScrollPane(viewList);
			 viewScroller.setPreferredSize(new Dimension(250, 150));
			 viewPanel.add(viewScroller);

			 JPanel	   viewOrderButtons = new JPanel();
			 viewOrderButtons.setOpaque(false);
			 BoxLayout viewOrderButtonsLayout = new BoxLayout(viewOrderButtons, BoxLayout.Y_AXIS);
			 viewOrderButtons.setBorder(BorderFactory.createTitledBorder("View order:"));
			 viewOrderButtons.setLayout(viewOrderButtonsLayout);

			 button = new JButton("up");
			 button.addActionListener(this);
			 button.setActionCommand("up");
			 viewOrderButtons.add(button);

			 button = new JButton("ignore");
			 button.addActionListener(this);
			 button.setActionCommand("ignore");
			 viewOrderButtons.add(button);

			 button = new JButton("remove");
			 button.addActionListener(this);
			 button.setActionCommand("remove");
			 viewOrderButtons.add(button);

			 button = new JButton("down");
			 button.addActionListener(this);
			 button.setActionCommand("down");
			 viewOrderButtons.add(button);
			 viewPanel.add(viewOrderButtons);

			 JPanel	   viewSortButtons = new JPanel();
			 viewSortButtons.setOpaque(false);
			 BoxLayout viewButtonsLayout = new BoxLayout(viewSortButtons, BoxLayout.Y_AXIS);
			 viewSortButtons.setBorder(BorderFactory.createTitledBorder("Sort by:"));
			 viewSortButtons.setLayout(viewButtonsLayout);
			 viewSortButtons.add(new JButton("alpha"  ));

			 button = new JButton("help");
			 button.addActionListener(this);
			 button.setActionCommand("help");
		//	 viewSortButtons.add(button);
			 viewPanel.add(button); // viewSortButtons

			 addRow(++row,"Views:",viewPanel);

			 //++++++++++++++++++++++++++++++
			 storyArea = new JTextArea(25,50);
			 storyArea.setEditable(true);
			 storyArea.setMargin(new Insets(5,5,5,5));
			 storyLabel = addRow(++row,"Story:",storyArea);

			 String story			 = new Filename(jsonPath.toString()).readAsString();
			 String viewSelected	 = "";
			 String templateSelected = "" ;

			 JSONParser parser = new JSONParser();
			 try {
			 //	 System.out.print("read: "+ story);
				 JSONObject object	   = (JSONObject) parser.parse(story);
				 story				   = (String) object.get("story") ;
				 JSONArray viewsOnFile = (JSONArray) object.get("views");
				 viewModel.removeAllElements();
				 for ( int i = 0 ; i < viewsOnFile.size(); i++ ) {
					 String view = (String) viewsOnFile.get(i);
					 this.viewModel.add(i,view);
				 }
				 viewSelected	  = (String) object.get("viewSelected"	  );
				 templateSelected = (String) object.get("templateSelected");
			 //	 System.out.println("viewSelected: " + viewSelected + " templateSelected: " + templateSelected);
			 } catch (ParseException pe) {
				 System.out.println("position: " + pe.getPosition());
				 System.out.println(pe);
			 } catch (Exception e) {}

			 storyArea.setText(story);

			 //++++++++++++++++++++++++++++++
			 JPanel optionsPanel = new JPanel();
			 optionsPanel.setOpaque(false);

			 templateList = new JComboBox<String>(getTemplates());
			 templateList.setAutoscrolls(true);

			 optionsPanel.add(new JLabel("Template:"));
			 optionsPanel.add(templateList);

			 button = new JButton("Help...");
			 button.addActionListener(this);
			 button.setActionCommand("template_help");
			 optionsPanel.add(button);

			 button = new JButton("Values...");
			 button.setActionCommand("values");
			 button.addActionListener(this);
			 optionsPanel.add(button);

			 button = new JButton("More...");
			 button.setActionCommand("more");
			 button.addActionListener(this);
			 optionsPanel.add(button);

			 JButton saveButton = new JButton("Save");
			 saveButton.setActionCommand("save");
			 saveButton.addActionListener(this);
			 optionsPanel.add(saveButton);

			 addRow(++row,"Options:",optionsPanel);

			 //++++++++++++++++++++++++++++++
			 startButton = new JButton("Start");
			 startButton.setActionCommand("start");
			 startButton.addActionListener(this);

			 addRow(++row,"Start:",startButton);

			 fixViewUI();
			 templateList.setSelectedItem(templateSelected);
			 viewList	 .setSelectedValue(viewSelected,true);

			 String s = new String();
			 String.format("viewSelected: %s templateSelected %s",viewSelected,templateSelected);
			 System.out.println(s);
		}

		@SuppressWarnings("unchecked")
		void save()
		{
			JSONArray views = new JSONArray();
			for ( int i = 0 ; i < viewModel.getSize() ; i++ ) {
				views.add(viewModel.get(i));
			}

			JSONObject object = new JSONObject();
			object.put("viewSelected"	 ,viewModel.get(viewList.getSelectedIndex()));
			object.put("templateSelected",templateList.getSelectedItem());
			object.put("story",storyArea.getText());
			object.put("views",views);

			StringWriter out = new StringWriter();
			try {
				JSONValue.writeJSONString(object, out);
				String jsonText = out.toString();
				PrintWriter printWriter = new PrintWriter(jsonPath);
				printWriter.println(jsonText);
				printWriter.close();
			//	System.out.print("saved: " + jsonText);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}

		public void updown(int up_down, boolean simple)
		{
			try {
				if ( simple ) {
					viewList.setSelectedIndex(viewList.getSelectedIndex());
				} else {
					int	   selected = viewList.getSelectedIndex();
					String swap     = viewModel.get(selected);
					viewModel.set(selected, viewModel.get(selected+up_down));
					viewModel.set(selected+up_down, swap);
					viewList.setSelectedValue(swap,true);
				}
			} catch (Exception e) {}
		}

		public void ignore() {
			String value = viewList.getSelectedValue();
			int nIgnored = value.indexOf(ignored);
			value = nIgnored > 0 ? value.substring(0,nIgnored) : value + ignored ;
			viewModel.set(viewList.getSelectedIndex(), value);
		}

		public void remove() {
			int index = viewList.getSelectedIndex();
			viewModel.remove(index);
			viewList.setSelectedIndex(index-(index>0?1:0));
		}

		public void start() {
			startButton.setEnabled(false);
			progressMonitor = new ProgressMonitor
					(  Panel.this
					,  "ToWebPlugin: Creating Images"
					,  "", 0, 100
					);
			story		  = storyArea.getText();
			worker		  = new Worker();
			worker.story  = story;

			worker.template = (String) templateList.getSelectedItem();
			storyEditable = storyArea.isEditable();
			storyArea.setEditable(false);
			storyArea.setText("");

			// find the views which are not ignored
			worker.views	  = new Vector<String>();
			for ( int i = 0 ; i < viewModel.getSize() ; i++ ) {
				String value = viewModel.get(i);
				int nIgnored = value.indexOf(ignored);
				if ( nIgnored < 0 ) worker.views.add(value);
			}

			worker.home	  = home ;
			worker.prefs	  = prefs;
			worker.bDummy	  = true ;
			worker.addPropertyChangeListener(this);
			worker.execute();
		}

		public void help(){
			try {
				new ProcessBuilder("open" ,"http://clanmills.com/files/ToWebPlugin/").start();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}

		// action events
		public void actionPerformed(ActionEvent event)
		{
			String cmd = event.getActionCommand();

			if      ( cmd.equals("save"  ) ) save();
			else if ( cmd.equals("start" ) ) start();
			else if ( cmd.equals("ignore") ) ignore();
			else if ( cmd.equals("remove") ) remove();
		    else if ( cmd.equals("help"  ) ) help();
			else if ( cmd.equals("down"  ) ) updown(+1,false);
			else if ( cmd.equals("up"    ) ) updown(-1,false);
			else messageBox(cmd + " not implemented yet");
		}

		// property change events
		public void propertyChange(PropertyChangeEvent evt)
		{
			if ("progress" == evt.getPropertyName() ) {
				storyArea.append(progressMonitor.getNote());
				if (progressMonitor.isCanceled() || worker.isDone()) {
					Toolkit.getDefaultToolkit().beep();
					if (progressMonitor.isCanceled()) {
						worker.cancel(true);
					}
					startButton.setEnabled(true);
				}
			}
		}
		// window events
		@Override
		public void windowClosing(WindowEvent e) {
			save();
		}
		public void windowClosed	 (WindowEvent e) {
			this.frame.dispose();
		}
		public void windowOpened	 (WindowEvent e) {}
		public void windowIconified	 (WindowEvent e) {}
		public void windowDeiconified(WindowEvent e) {}
		public void windowActivated	 (WindowEvent e) {}
		public void windowDeactivated(WindowEvent e) {}

		// mouse events
		@Override
		public void mouseClicked(MouseEvent e) {
			if (e.getClickCount() == 2) ignore();
		}
		public void mousePressed (MouseEvent e) {}
		public void mouseReleased(MouseEvent e) {}
		public void mouseEntered (MouseEvent e) {}
		public void mouseExited	 (MouseEvent e) {}

		// key events
		public void key(String k,KeyEvent e) {
		//	System.out.println(String.format("key%s = %d",k,e.getKeyCode()));
		}
		@Override
		public void keyPressed(KeyEvent e)  {
			int m = e.getModifiers();
			switch ( e.getKeyCode() ) {
				case  8	: remove()        ; break; // Delete
				case 10 : ignore()        ; break; // Return
				case 38 : updown(-1,m==0) ; break; // Up
				case 40 : updown(+1,m==0) ; break; // Down
			}
		}
		public void keyReleased(KeyEvent e) { key("Released",e); }
		public void keyTyped(KeyEvent e)    { key("Typed"   ,e); }

		@Override
		public void valueChanged(ListSelectionEvent e) {
			if ( ! e.getValueIsAdjusting() ) {
				String name = viewList.getSelectedValue();
				plugin.setView(name);
			}
		}
	}

	public void messageBox(String m)
	{
		JOptionPane.showMessageDialog(null,m);
	}

	/**
	 * Create the GUI and show it.
	 */
	void createAndShowGUI(Home home_,HomeController3D homeController3D_,UserPreferences prefs_)
	{
		theJob           = new TheJob();
		home             = home_  ;
		homeController3D = homeController3D_;
		prefs            = prefs_ ;

		String APPLICATION_PLUGINS_SUB_FOLDER = "plugins"; // private in SweetHome3D.java

		File[] applicationPluginsFolders = null;
		try {
			applicationPluginsFolders = ((FileUserPreferences) prefs).getApplicationSubfolders(APPLICATION_PLUGINS_SUB_FOLDER);
		} catch (IOException e) {
			e.printStackTrace();
		}

		// where's the jarPath? (we'll need that for the templates)
		if ( applicationPluginsFolders != null ) {
			for ( File file : applicationPluginsFolders ) {
				jarPath = file.getAbsolutePath() + File.separator.charAt(0) + ToWebPlugin.sToWebPluginJar;
				System.out.println("jarPath: " + jarPath);
			}
		}

		// Create and set up content
		if ( home.getName() == null ) {
			messageBox("Current model has no name.  Please save it!");
		} else {
			String name = home.getName();
			jsonPath    = name.substring(0, name.lastIndexOf('.')) + ".toWebPlugin";
			System.out.println("jsonPath = " + jsonPath);

			// Create, set up and display panel
			frame       = new JFrame("ToWebPlugin Panel");
			Panel panel = new Panel(this);
			frame.setContentPane(panel);
			frame.pack();
			frame.addWindowListener(panel);
			frame.setVisible(true);
		}
	}

	@Override
	public PluginAction[] getActions()
	{
		return new PluginAction [] {new Action()};
	}

	public class Action extends PluginAction
	{
		public Action()
		{
			putPropertyValue(Property.NAME, "ToWebPlugin...");
			putPropertyValue(Property.MENU, "Tools");
			setEnabled(true);
		}

		@Override
		public void execute()
		{
			javax.swing.SwingUtilities.invokeLater(new Runnable() {
				public void run() {
					createAndShowGUI(getHome(),getHomeController().getHomeController3D(),getUserPreferences());
				}
			});
		}
	}
	
	// http://stackoverflow.com/questions/10967451/open-a-link-in-browser-with-java-button
	public void openWebpage(URI uri)
	{
	    Desktop desktop = Desktop.isDesktopSupported() ? Desktop.getDesktop() : null;
	    if (desktop != null && desktop.isSupported(Desktop.Action.BROWSE)) {
	        try {
	            desktop.browse(uri);
	        } catch (Exception e) {
	            e.printStackTrace();
	        }
	    }
	}

	public void openWebpage(URL url) {
	    try {
	        openWebpage(url.toURI());
	    } catch (URISyntaxException e) {
	        e.printStackTrace();
	    }
	}
	
	public void openWebpage(String url)
	{
		try {
			openWebpage(new URL(url));
		} catch (MalformedURLException e) {
			e.printStackTrace();
		}
	}

	class TheJob {

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
		boolean			 bDummy	 = false ;
		ArrayList<Photo> photos	 = new ArrayList<Photo>();
		String			 template;
		String           story;
		Vector<String>	 views;

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

//		private boolean writeOut(String path,String data,boolean bClose)
//		{
//			boolean			 result = false;
//			DataOutputStream out	= openStream(path);
//			if ( out != null ) {
//				result = writeOut(out,data,bClose);
//			}
//			return result;
//		}

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

		private boolean paintPlan(PlanComponent plan,int width,int height,String name,String directory,String ext,String type)
		{
			BufferedImage image = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);
			Graphics2D	  g		= image.createGraphics();

			Rectangle2D.Double	 bounds = new Rectangle2D.Double(0,0,100,100) ; // plan.getPlanBounds();

			String		  file	= name + ext;
			String		  path	= Filename.join(directory,file);
			System.out.println("plan image saved to " + path);

			double scalex		= (double) width  / bounds.getWidth() ;
			double scaley		= (double) height / bounds.getHeight();
			float  scale1		= plan.getScale();
			plan.setScale( scalex < scaley ? (float) scalex : (float) scaley);
			plan.setSize(width, height);
			plan.paint(g);
			plan.setScale(scale1);
			return saveImage(image,path,type);
		}

		private boolean paintCamera(Home home,HomeController3D homeController3D,Camera camera,int width,int height,String directory,String ext,String type)
		{
			boolean result = false;
			try {
				// homeController3D.goToCamera(camera);
				setView(camera.getName());
				HomeComponent3D comp  = new HomeComponent3D(home);
				BufferedImage	image = comp.getOffScreenImage(width,height);
				String			file  = camera.getName() + ext;
				String			path  = Filename.join(directory,file);
				System.out.println("camera image saved to " + path);
				result				  = saveImage(image,path,type);
			} catch ( Exception e) {
				System.err.println("Exception in paintCamera " + camera.getName() + " " + e.toString());
			}
			return result;
		}
		
		public String getCurrentTimeStamp() {
		    SimpleDateFormat sdfDate = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");//dd/MM/yyyy
		    return           sdfDate.format(new Date());
		}

		public boolean generateCode(Home home, UserPreferences prefs,int index)
		{
			// index =  0..max-1   : generate a page in the gallery
			// index = -1          : generate the index page
			// you must call index == -1 last as there are arrays in TheJob class remembering generated pages
			boolean result = false ; // return value == true means HTML is ready

			// create directory for output
			// http://stackoverflow.com/questions/1503555/how-to-find-my-documents-folder
			// String	homedir		 = javax.swing.filechooser.FileSystemView.getFileSystemView().getDefaultDirectory().toString();
			String  filedir      = home.getName();
			String  title        = "";
			try {
                title            = filedir.substring(filedir.lastIndexOf(File.separator.charAt(0))+1);
                title            = title  .substring(0,  title.lastIndexOf('.'));
    			filedir		     = filedir.substring(0,filedir.lastIndexOf('.'));
			} catch (Exception e){}

			String	templatePath = Filename.join(System.getProperty("java.io.tmpdir"),"ToWebPlugin","templates",template + ".zip");
			String	htmlPath	 = Filename.join(filedir, template);
			String	Images		 = Filename.join(htmlPath,"Images");
			Filename.mkdirs(Images);

			int		x			 = 1		; // multiply up the size of images
			int		y			 = 1		; // divide the size of images

			int		iwidth		 = 720*x/y	;
			int		iheight		 = 480*x/y	;

			String	type		 = "PNG"	;
			String	ext			 = ".png"	;
			String  message      = ""       ;

			if ( index == 0 ) photos = new ArrayList<Photo>();
			System.out.printf("TheJob.generateCode %d\n",index);

			if ( index >= 0 ) try {
				String  name  = views.get(index);
				boolean bDone = false ;

				// paint camera
				if ( !bDone ) for ( Camera camera : home.getStoredCameras() ) {
					if ( !bDone && camera.getName().equals(name) ) {
						if ( paintCamera(home,homeController3D,camera,iwidth,iheight,Images,ext,type)) {
							String file = name + ext;
							message	   += String.format("%s\n",unCamel(name));
							photos.add(new Photo(file,unCamel(name)));
							System.out.println("Camera " + message);
							bDone = true;
						}
					}
				}

				// paint level
				if ( !bDone ) for ( Level level : home.getLevels() ) {
					if ( level.getName().equals(name) ) {
						home.setSelectedLevel(level);
						PlanComponent plan	= new PlanComponent(home,prefs, null);
						Level selectedLevel = home.getSelectedLevel();
						String file	 = name + ext;
						if ( paintPlan(plan,iwidth,iheight,name,Images,ext,type)) {
							message += String.format("%s\n", unCamel(name));
							photos.add(new Photo(file,unCamel(name)));
							System.out.println("Level " + message);
							bDone = true;
						}
						home.setSelectedLevel(selectedLevel);
					}
				}
			} catch (Exception e){}

			if ( index == -1 ) {

				String indexStub = "index"     ;
				String indexFile = "index.html";
				String indexPath = Filename.join(htmlPath,indexFile);
				result           = new FileUnzip().extract(templatePath,htmlPath);
				message          = result ? "" : ("unable unzip to "  + htmlPath);

				if ( result ) {
					System.out.println("generating code");

					// use template to generate code
					STRawGroupDir ts = new STRawGroupDir(htmlPath,'$','$'); // $..$ = key
					ST			  tx = ts.getInstanceOf(indexStub); // load index.st
					tx.add("photos",photos);
					tx.add("story" ,story);
					tx.add("title" ,title);
					tx.add("now"   ,getCurrentTimeStamp());
					tx.add("background","skyblue");
					
	                DataOutputStream out = openStream(indexPath) ;
	                if ( out != null ) {
	                	result = writeOut(out,tx.render(),true);
	                }
					if ( ! result ) message = "unable to open " + indexPath;
					System.out.println("Code: "+indexPath);
				}
				
				if ( result ) try {
					openWebpage("file://" + indexPath);
				} catch (Exception e) {
					result = false ;
					message = "unable to open browser on " + indexPath;
				}

				if ( !result ) messageBox(message);
			}
			return result;
		}
	}
}
