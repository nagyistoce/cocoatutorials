package com.clanmills.sh3d.ToWebPlugin;

import org.json.simple.*;
import org.json.simple.parser.*;

import java.io.BufferedOutputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Set;
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

/**
 * UnzipUtility extracts files and directories of a standard zip file to
 * a destination directory.
 * @author www.codejava.net
 *
 */
// http://www.codejava.net/java-se/file-io/programmatically-extract-a-zip-file-using-java
class UnzipUtility {
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
	public void unzip(String zipFilePath, String destDirectory) throws IOException {
		File destDir = new File(destDirectory);
		if (!destDir.exists()) {
			destDir.mkdir();
		}
		ZipInputStream zipIn = new ZipInputStream(new FileInputStream(zipFilePath));
		ZipEntry entry = zipIn.getNextEntry();
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
	}

	public void unzip(Path zip,Path dest) throws IOException {
		unzip(zip.toString(),dest.toString());
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
		pathSeparator	   = '/';
		extensionSeparator = '.';
	}

	public Filename(Path path){
		fullPath		   = path.toString();
		pathSeparator	   = '/';
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
			 List<String> lines = Files.readAllLines(Paths.get(fullPath),Charset.forName("UTF-8"));
			 int line = 0 ;
			 while ( lines != null && line	< lines.size() ) {
				 result = result + lines.get(line++).toString() + '\n';
			 }
		} catch (Exception e) {
			 e.printStackTrace();
		}
		return result;
	}
}

public class ToWebPlugin extends Plugin {

	public	static class ToWebPluginPanel extends JPanel
			implements ActionListener,
			WindowListener,
			PropertyChangeListener {

		/**
		 *
		 */
		public	static final String sToWebPlugin	 = "ToWebPlugin";
		private static final long	serialVersionUID = 276503287;

		private ProgressMonitor		progressMonitor;
		private JFrame				frame;
		private JButton				startButton;
		private JTextArea			storyArea;
		private JLabel				storyLabel;
		private Boolean				storyEditable;

		private ToWebPluginTask		task;
		private UserPreferences		prefs;
		private Home				home;
		private Path				path;
		private ToWebPluginWorker	worker;
		public	String				story;
		public	String				ignored = " *ignored* ";
		JList<String>				viewList;
		JComboBox<String>			templateList;
		DefaultListModel<String>	viewModel		= new DefaultListModel<String>();

		class ToWebPluginTask extends SwingWorker<Void, Void> {
			Home			home;
			UserPreferences prefs;
			String			story;
			String			storyLabelText;
			String			template;
			Boolean			bDummy;
			Vector<String>	views;

			@Override
			public Void doInBackground() {

				// save camera
				Camera camera1	= home.getCamera();

				int	   progress = 0;
				int	   percent	= 0;
				int	   max		= views.size();
				storyLabelText	= storyLabel.getText();
				storyLabel.setText("Output:");
				worker.bDummy	= bDummy;
				worker.views	= views;
				setProgress(0);
				try {
					sleep(500);
					worker.template = template;
					while (progress < max  && !isCancelled()) {
						String name	   = views.get(progress);
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
						setProgress(100);
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

		public Vector<String> getTemplates()
		{
			Vector<String> result = new Vector<String>();
			try {
				char	sep			= FileSystems.getDefault().getSeparator().charAt(0);
				char	dot			= '.';
				String	want		= "zip";
				String	docs		= javax.swing.filechooser.FileSystemView.getFileSystemView().getDefaultDirectory().toString();
				String	templateDir = Paths.get(docs).resolve("Documents").resolve("ToWebPlugin").resolve("templates").toString();
			//	System.out.println("templateDir = " + templateDir.toString());

				File[] files	   = new File(templateDir).listFiles();
				for ( File file : files) {
					Filename filename = new Filename(file,sep,dot);
					if ( filename.extension().equals(want) )	result.add(filename.filename());
				//	System.out.println("templates:" + filename.filename().toString() + " extension:" + filename.extension());
				}
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
				int	 nIgnored = view.indexOf(ignored);
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

		public void sleep(int millisecs) throws InterruptedException
		{
			Thread.sleep(millisecs);
		}

		public void sleeps(int millisecs)
		{
			try {
				sleep(millisecs);
			} catch (InterruptedException e) {
				e.printStackTrace();
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

		public ToWebPluginPanel(Home home_,Path path_)
		{
			 super(new GridBagLayout());

			 home=home_;
			 path=path_;

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

			 String story			 = new Filename(path.toString()).readAsString();
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
				PrintWriter printWriter = new PrintWriter(path.toFile());
				printWriter.println(jsonText);
				printWriter.close();
			//	System.out.print("saved: " + jsonText);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}

		/**
		 * Invoked when the user presses the start button.
		 */
		public void actionPerformed(ActionEvent evt)
		{
			String cmd = evt.getActionCommand();

			if ( cmd.equals("save") ) {
				save();
			} else if ( cmd.equals("start") ) {
				startButton.setEnabled(false);
				progressMonitor = new ProgressMonitor
						(  ToWebPluginPanel.this
						,  "ToWebPlugin: Creating Images"
						,  "", 0, 100
						);
				story		  = storyArea.getText();
				task		  = new ToWebPluginTask();
				task.story	  = story;

				task.template = (String) templateList.getSelectedItem();
				storyEditable = storyArea.isEditable();
				storyArea.setEditable(false);
				storyArea.setText("");

				// find the views which are not ignored
				task.views	  = new Vector<String>();
				for ( int i = 0 ; i < viewModel.getSize() ; i++ ) {
					String value = viewModel.get(i);
					int nIgnored = value.indexOf(ignored);
					if ( nIgnored < 0 ) task.views.add(value);
				}

				task.home			 = home ;
				task.prefs			 = prefs;
				task.bDummy			 = true ;
				task.addPropertyChangeListener(this);
				task.execute();
			} else if ( cmd.equals("ignore") ) {
				String value = viewList.getSelectedValue();
				int nIgnored = value.indexOf(ignored);
				value = nIgnored > 0 ? value.substring(0,nIgnored) : value + ignored ;
				viewModel.set(viewList.getSelectedIndex(), value);
			} else if ( cmd.equals("remove") ) {
				int index = viewList.getSelectedIndex();
				viewModel.remove(index);
				viewList.setSelectedIndex(index-(index>0?1:0));
			} else if ( cmd.equals("up") || cmd.equals("down") ) {
				try {
					int	 up_down  = cmd.equals("down") ? +1 : -1 ;
					int	 selected = viewList.getSelectedIndex();
					String swap = viewModel.get(selected);
					viewModel.set(selected, viewModel.get(selected+up_down));
					viewModel.set(selected+up_down, swap);
					viewList.setSelectedIndex(selected+up_down);
					viewList.scrollRectToVisible(viewList.getCellBounds(selected-1, selected +1));
				} catch (Exception e) {}
			} else if ( cmd.equals("help") ) {
				try {
					new ProcessBuilder("open" ,"http://clanmills.com/files/ToWebPlugin/").start();
				} catch (IOException e) {
					e.printStackTrace();
				}
			} else {
				JOptionPane.showMessageDialog(null, cmd + " not implemented yet");
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

		@Override
		public void windowClosing(WindowEvent e) {
			save();
		}

		public void windowOpened	 (WindowEvent e) {}
		public void windowClosed	 (WindowEvent e) { this.frame.dispose(); }
		public void windowIconified	 (WindowEvent e) {}
		public void windowDeiconified(WindowEvent e) {}
		public void windowActivated	 (WindowEvent e) {}
		public void windowDeactivated(WindowEvent e) {}
	}


	/**
	 * Create the GUI and show it.
	 */
	void createAndShowGUI(ToWebPluginWorker worker,Home home,UserPreferences prefs)
	{
		//Create and set up the window.
		JFrame frame = new JFrame("ToWebPlugin Panel"); // ToWebPlugin Control Panel");
		frame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);

		Path path = FileSystems.getDefault().getPath(home.getName().substring(0, home.getName().lastIndexOf('.')) + ".toWebPlugin");

		//Create and set up the content pane.
		ToWebPluginPanel toWebPluginPanel = new ToWebPluginPanel(home,path);
		toWebPluginPanel.worker			  = worker;
		toWebPluginPanel.prefs			  = prefs;
		toWebPluginPanel.frame			  = frame;
		toWebPluginPanel.setOpaque(true);

		//Display the window.
		frame.setContentPane(toWebPluginPanel);
		frame.pack();
		frame.setVisible(true);
		frame.addWindowListener(toWebPluginPanel);
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
			setEnabled(true);
		}

		@Override
		public void execute()
		{
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
		boolean			 bDummy	 = false ;
		ArrayList<Photo> photos	 = new ArrayList<Photo>();
		String			 template;
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

		private boolean writeOut(String path,String s,boolean bClose)
		{
			boolean			 result = false;
			DataOutputStream out	= openStream(path);
			if ( out != null ) {
				result = writeOut(out,s,bClose);
			}
			return result;
		}

		private boolean writeOut(Path path,String s,boolean bClose)
		{
			return writeOut(path.toString(),s,bClose);
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
			boolean result = false ; // return value == true means HTML is ready

			// create directory for output
			// http://stackoverflow.com/questions/1503555/how-to-find-my-documents-folder
			String	docs		 = javax.swing.filechooser.FileSystemView.getFileSystemView().getDefaultDirectory().toString();
			Path	templatePath = Paths.get(docs).resolve("Documents").resolve("ToWebPlugin").resolve("templates").resolve(template + ".zip");
			String	title		 = home.getName().substring(0, home.getName().lastIndexOf('.'));
			Path	htmlPath	 = Paths.get(FileSystems.getDefault().getPath(home.getName().substring(0, home.getName().lastIndexOf('.'))).toString());
			Path	Images		 = htmlPath.resolve("Images"); (new File(Images.toString())).mkdirs();

			String	message		 = ""		;

			int		x			 = 1		; // multiply up the size of images
			int		y			 = 1		; // divide the size of images

			int		iwidth		 = 720*x/y	;
			int		iheight		 = 480*x/y	;

			String	type		 = "PNG"	;
			String	ext			 = ".png"	;

			if ( index == 0 ) photos = new ArrayList<Photo>();

			boolean bCameras  = true  ;
			boolean bLevels	  = false ;

			System.out.printf("ToWebPluginWorker.execute %d\n",index);

			String	htmlFileName = "index.html";
			try {
				String name = views.get(index);
				boolean bDone = false ;

				// paint camera
				if ( !bDone && bCameras && index >= 0 ) {
					for ( Camera camera : home.getStoredCameras() ) {
						if ( !bDone && camera.getName().equals(name) ) {
							if ( paintCamera(home,camera,iwidth,iheight,Images,ext,type)) {
								String file = name + ext;
								message	   += String.format("%s\n",unCamel(name));
								photos.add(new Photo(file,unCamel(name)));
								System.out.println("Camera " + message);
							}
						}
					}
				}

				// paint level
				if ( !bDone && bLevels && index >= 0 ) {
					for ( Level level : home.getLevels() ) {
						home.setSelectedLevel(level);
						PlanComponent plan	= new PlanComponent(home,prefs, null);
						if ( level.getName().equals(name) ) {
							Level level1 = home.getSelectedLevel();
							String file	 = name + ext;
							if ( paintPlan(plan,iwidth,iheight,name,Images,ext,type)) {
								message += String.format("%s\n", unCamel(name));
								photos.add(new Photo(file,unCamel(name)));
								System.out.println("Level " + message);
							}
							home.setSelectedLevel(level1);
						}
					}
				}
			} catch (Exception e){}

			if ( index == -1 ) {
				UnzipUtility unzip = new UnzipUtility();
				try {
					unzip.unzip(templatePath, htmlPath);
					templatePath = htmlPath.resolve(template);
					System.out.println("generating code");
					// use the template to generate the code
					STRawGroupDir ts = new STRawGroupDir(templatePath.toString(),'$','$');
					ST			  t2 = ts.getInstanceOf(htmlFileName); // load page.st
					// t2.add("title", title + "Generated by ToWebPlugin");
					t2.add("photos",photos);
					String PHOTOS = t2.render();

					// Read html from template as string
					// edit it
					// write to to the output directory
					Path htmlTemplatePath = templatePath.resolve(htmlFileName);
					String fileStr		  = new Filename(htmlTemplatePath).readAsString();
					fileStr = fileStr.replaceAll("__PHOTOS__", PHOTOS);
					fileStr = fileStr.replaceAll("__TITLE__" , title);
					Path htmlOutputPath = htmlPath.resolve(htmlFileName);
					result				  = writeOut(htmlOutputPath,fileStr,true);

					if ( result ) try {
						new ProcessBuilder("open" ,htmlPath.resolve(htmlFileName).toString()).start();
					} catch (Exception e) {
						System.out.println("unable to ditto and open");
					}
				} catch (Exception ex) {
					// some errors occurred
					ex.printStackTrace();
				}

				if ( !result ) message = "unable to write HTML " + htmlFileName;
			}

			return result;
		}
	}
}
