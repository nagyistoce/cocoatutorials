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
            HomeComponent3D comp  = new HomeComponent3D(home);
            BufferedImage   image = comp.getOffScreenImage(width,height);
            String          file  = camera.getName() + ext;
            String          path  = directory.resolve(file).toString();
            home.setCamera (camera);
            boolean     result = saveImage(image,path,type);
            return          result;
        }


        @Override
        public void execute()
        {
            Home    home      = getHome();

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
                    UserPreferences prefs = getUserPreferences();
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
        }
    }
}
