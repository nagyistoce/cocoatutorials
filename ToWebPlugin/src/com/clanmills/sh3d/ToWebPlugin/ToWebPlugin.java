package com.clanmills.sh3d.ToWebPlugin;

import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;
import javax.swing.JOptionPane;

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
    public PluginAction[] getActions() {
        return new PluginAction [] {new ToWebAction()};
    }

    public class ToWebAction extends PluginAction {
        public ToWebAction() {
           putPropertyValue(Property.NAME, "ToWebPlugin...");
           putPropertyValue(Property.MENU, "Tools");
           // Enables the action by default
           setEnabled(true);
        }
        
        public boolean saveImage(BufferedImage image,String pathName,String fileType)
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
        
        @Override
        public void execute()
        {
            Home    home      = getHome();
            String  directory = "/Users/rmills/Documents/SH3D_ToWebPlugin/";
            String  message   = ""   ;
            int     width     = 800  ;
            int     height    = 600  ;
            String  type      = "PNG";
            boolean bLevels   = true ;
            boolean bCameras  = true ;
            
            // paint with every camera
            if ( bCameras ) {
                // save camera
                Camera camera1    = home.getCamera();
            	for ( Camera camera : home.getStoredCameras() ) {
                    home.setCamera(camera);
                    HomeComponent3D comp  = new HomeComponent3D(home);
                    BufferedImage   image = comp.getOffScreenImage(width,height);
                    String          path  = directory + camera.getName() + ".png";
                    if ( saveImage(image,path,type) ) {
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
                	String          path  = directory + level.getName() + ".png";
                    BufferedImage   image = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);
                    Graphics2D      g     = image.createGraphics();
                    PlanComponent   plan  = new PlanComponent(home,prefs, null);
                    plan.setSize(width, height);
                    plan.paint(g);
                    if ( saveImage(image,path,type) ) {
                        message += String.format("%s\n", path);
                    }
                }
                home.setSelectedLevel(level1);
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
