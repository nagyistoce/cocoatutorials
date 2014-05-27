package com.clanmills.sh3d.ToWebPlugin;

import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.List;
import java.util.prefs.Preferences;

import javax.imageio.ImageIO;
import javax.swing.JOptionPane;

import com.eteks.sweethome3d.model.Camera;
import com.eteks.sweethome3d.model.Home;
import com.eteks.sweethome3d.model.UserPreferences;
import com.eteks.sweethome3d.plugin.Plugin;
import com.eteks.sweethome3d.plugin.PluginAction;
import com.eteks.sweethome3d.swing.HomeComponent3D;
import com.eteks.sweethome3d.swing.PlanComponent;
// import com.eteks.sweethome3d.viewcontroller.HomeController3D;

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
            Home    home         = getHome();
            String  directory    = "/Users/rmills/Documents/SH3D_ToWebPlugin/";
            String  message      = "" ;
        	int     imageWidth   = 800;
        	int     imageHeight  = 600;
        	String  fileType     = "PNG";
        	boolean bPlan        = true ;
        	
        	// save camera
        	Camera camera1        = home.getCamera();
        	
            // paint with every camera
        	for ( Camera      camera   : home.getStoredCameras() ) {
            	home.setCamera(camera);
            	HomeComponent3D       homeComponent3D = new HomeComponent3D(home);
            	BufferedImage image = homeComponent3D.getOffScreenImage(imageWidth , imageHeight);
                String        pathName = directory + camera.getName() + ".png";
                if ( saveImage(image,pathName,fileType) ) {
					message += String.format("%s\n", pathName);
                }
        	}
            
            // paint plan
        	if ( bPlan ) {
	        	String pathName = directory + "Plan.png";
	        	UserPreferences userPreferences = getUserPreferences();
	        	BufferedImage   image = new BufferedImage(imageWidth, imageHeight, BufferedImage.TYPE_INT_ARGB);
	        	Graphics2D      g     = image.createGraphics();
	        	PlanComponent   plan  = new PlanComponent(home,userPreferences, null);
	        	plan.setSize(imageWidth, imageHeight);
	        	plan.paint(g);
	            if ( saveImage(image,pathName,fileType) ) {
					message += String.format("%s\n", pathName);
	            }
        	}
            
            // restore camera
            home.setCamera(camera1);
            
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