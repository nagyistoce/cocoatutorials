package com.clanmills.sh3d.ToWebPlugin;

import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.io.BufferedOutputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileFilter;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.URL;
import java.net.URLDecoder;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.security.CodeSource;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

import javax.imageio.ImageIO;
import javax.swing.JOptionPane;

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
public class ToWebPlugin extends Plugin {
    @Override
    public PluginAction[] getActions()
    {
        return new PluginAction [] {new ToWebAction()};
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

        public String pluginDir   ="";
        public String templateDir ="";

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

        private boolean writeOut(DataOutputStream out,String s)
        {
            boolean result=false;
            try {
                out.write(s.getBytes("US-ASCII"));
                result=true;
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
            return result;
        }

        @Override
        public void execute()
        {
            Home    home      = getHome();
            // String  directory = "/Users/rmills/Documents/SH3D_ToWebPlugin/";
            
            // create directory for output 
            Path    Directory = Paths.get(System.getProperty("user.home")).resolve("Documents").resolve("ToWebPlugin");
            String  directory = Directory.toString();
            (new File(directory)).mkdirs();
            
            String  message   = ""    ;

            int     width     = 400   ;
            int     height    = 300   ;

            String  type      = "PNG" ;
            String  ext       = ".png";
            String  file      = ""    ;

            boolean bCameras  = true  ;
            boolean bLevels   = true  ;

            String  name      = "index.html";
            String  path      = Directory.resolve(name).toString();
            
            System.out.println("directory = " + directory );

            // find templates in the jar
            // http://stackoverflow.com/questions/1429172/how-do-i-list-the-files-inside-a-jar-file
            // List<String> list = new ArrayList<String>();
            // File[] webimages =  
            // BufferedImage image = ImageIO.read(this.getClass().getResource(webimages[nextIndex].getName() ));
            
            CodeSource src = ToWebPlugin.class.getProtectionDomain().getCodeSource();
            if (src != null) {
	            URL jarCode = src.getLocation();
	            ZipInputStream zip = null;
				try {
					zip = new ZipInputStream(jarCode.openStream());
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
	            while( zip != null ) {
	                ZipEntry entry = null;
					try {
						entry = zip.getNextEntry();
					} catch (IOException e) {
						e.printStackTrace();
					}
	                if (entry != null) {
		                String entryName = entry.getName();
		                System.out.println(entryName);
	                } else {
	                	zip = null;
	                }
	                
	                // .read(this.getClass().getResource("CompanyLogo.png"))
	                
	                if (name.startsWith("templates/")  && name.endsWith(".st")) {
	                	
	                }
	            }
            } 
            System.out.println("------------------------");

            // enumerate files in templateDir (overwrite templates in the jar)
            File   textFolder = new File(templateDir);
            File[] stFiles    = textFolder.listFiles( new FileFilter() {
                   public boolean accept( File file ) {
                	   return file.getName().endsWith(".st");
                   }
            });
            for ( File stFile : stFiles ) System.out.println(stFile);
            System.out.println("------------------------");
            
            STRawGroupDir ts  = new STRawGroupDir(templateDir,'$','$');
            ST            t2  = ts.getInstanceOf("page"); // load page.st
            
            String[] users = { "Robin", "Mills" };

            t2.add("name", "World");
            t2.add("title", "I am the Hello World Title");
            t2.add("users",users);
            System.out.print(t2.render());

            try {
                DataOutputStream out = new DataOutputStream(new BufferedOutputStream(new FileOutputStream(path)));
                writeOut(out,"<html><body><table>\n");

                // paint with every camera
                if ( bCameras ) {
                    // save camera
                    Camera camera1    = home.getCamera();
                    for ( Camera camera : home.getStoredCameras() ) {
                        home.setCamera(camera);
                        HomeComponent3D comp  = new HomeComponent3D(home);
                        BufferedImage   image = comp.getOffScreenImage(width,height);
                        name  = camera.getName();
                        file  = name + ext      ;
                        path  = Directory.resolve(file).toString();
                        if ( saveImage(image,path,type) ) {
                            writeOut(out,String.format("<tr><td><center><img src=\"%s\"><br>%s</center></td></tr>\n",file, name));
                            message += String.format("%s\n", path);
                        }
                    }
                    // restore camera
                    home.setCamera(camera1);
                }

                // paint every level
                if ( bLevels ) {
                    UserPreferences prefs = getUserPreferences();
                    Level level1 = home.getSelectedLevel();
                    for ( Level level : home.getLevels() ) {
                        home.setSelectedLevel(level);
                        BufferedImage image = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);
                        Graphics2D    g     = image.createGraphics();
                        PlanComponent plan  = new PlanComponent(home,prefs, null);
                        plan.setSize(width, height);
                        plan.paint(g);
                        name  = level.getName() ;
                        file  = name + ext      ;
                        path  = Directory.resolve(file).toString();
                        if ( saveImage(image,path,type) ) {
                            writeOut(out,String.format("<tr><td><center><img src=\"%s\"><br>%s</center></td></tr>\n",file, name));
                            message += String.format("%s\n", path);
                        }
                    }
                    home.setSelectedLevel(level1);
                }
                writeOut(out,"</table></body></html>\n");
                out.close();
            } catch ( IOException e) {
                e.printStackTrace();
            }

            // inform user
            if ( message.length() > 0 ) {
                message = "images:\n" + message;
            } else {
                message = "no images have been written!";
            }
            JOptionPane.showMessageDialog(null, message);
        }
    }
}
