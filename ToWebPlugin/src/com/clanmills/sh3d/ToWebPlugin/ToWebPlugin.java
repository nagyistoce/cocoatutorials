package com.clanmills.sh3d.ToWebPlugin;

import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.List;

import javax.imageio.ImageIO;
import javax.swing.JOptionPane;

import com.eteks.sweethome3d.model.Camera;
import com.eteks.sweethome3d.model.Home;
import com.eteks.sweethome3d.plugin.Plugin;
import com.eteks.sweethome3d.plugin.PluginAction;
import com.eteks.sweethome3d.swing.HomeComponent3D;

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
        
        @Override
        public void execute() {
            
            Home   home           = getHome();
            String directory      = "/Users/rmills/Documents/SW3dWeb/";
            String message        = "images:\n";
        	int    imageWidth     = 800;
        	int    imageHeight    = 600;
        	Camera camera1        = home.getCamera();
        	int    count          = 0;
        	List <Camera> cameras = home.getStoredCameras();
        	
            for ( Camera  camera  : cameras) {
            	home.setCamera(camera);
            	HomeComponent3D       homeComponent3D = new HomeComponent3D(home); // , getHome().preferences, quality == 1);
            	BufferedImage image = homeComponent3D.getOffScreenImage(imageWidth , imageHeight);
	            try {
	                String pngFile = directory + camera.getName() + ".png"; 
					ImageIO.write(image, "PNG", new File(pngFile));
					message += String.format("%s\n", pngFile);
					count++;
				} catch (IOException e) {
					System.err.println("Bad News: Exception in ToWebPlugin.java");
					e.printStackTrace();
				}
            }
            home.setCamera(camera1);
            message += String.format("image written = %d",count);
            JOptionPane.showMessageDialog(null, message);
        }
    }
}