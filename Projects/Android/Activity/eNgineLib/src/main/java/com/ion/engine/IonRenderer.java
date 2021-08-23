
package com.ion.engine;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;

import android.content.SharedPreferences;
import android.content.res.AssetManager;
import android.opengl.GLU;
import android.preference.CheckBoxPreference;
import android.util.Log;
import net.rbgrn.android.glwallpaperservice.GLWallpaperService;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.StringReader;
import java.lang.Thread;
import java.util.ArrayList;
import java.util.List;

import android.util.Log;
import android.view.MotionEvent;

import android.view.GestureDetector;
import android.view.MotionEvent;

public class IonRenderer implements GLWallpaperService.Renderer
{	
	long startTime;

	class OptionGlue
	{		
		int type;
		int def_value;
		int value;		
		String scriptFunc;	
	};
	
	String scene_name;
	List<OptionGlue> options = null;
	static public int double_clink_val = 1;
	AssetManager mngr;
	SharedPreferences settings;
	
	public Document XMLfromString(String v)
	{
        Document doc = null;

        DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
        
        try
        {
            DocumentBuilder db = dbf.newDocumentBuilder();

            InputSource is = new InputSource();
            is.setCharacterStream(new StringReader(v));
            doc = db.parse(is); 

        }
        catch (ParserConfigurationException e)
        {
            e.printStackTrace();
        }
        catch (SAXException e)
        {
            System.out.println("Wrong XML file structure: " + e.getMessage());
            return null;
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }

        return doc;
    }
	
	private String convertStreamToString(InputStream is)
	{
	    BufferedReader reader = new BufferedReader(new InputStreamReader(is));
	    StringBuilder sb = new StringBuilder();

	    String line = null;
	    try
	    {
	        while ((line = reader.readLine()) != null)
	        {
	            sb.append(line + "\n");
	        }
	    }
	    catch (IOException e)
	    {
	        e.printStackTrace();
	    }
	    finally
	    {
	        try
	        {
	            is.close();
	        }
	        catch (IOException e)
	        {
	            e.printStackTrace();
	        }
	    }
	    return sb.toString();
	}
	
	public void LoadPreference(AssetManager set_mngr, SharedPreferences set_settings)
	{
		mngr = set_mngr;
		settings = set_settings;
		
		String imagePath = "preferences.xml";	    
	    		
	    InputStream is=null;
	    
	    options = new ArrayList<OptionGlue>();
	    
	    try
	    {
	        is = mngr.open(imagePath);
	        
	        String str = convertStreamToString(is).substring(1);	        	       
	        
	        Document doc = XMLfromString(str);
	        
	        Node root = doc.getFirstChild();
	        
	        Node nd = root.getFirstChild();
	        
	        while (nd != null)
	        {	        		        	
	        	String nodeName = nd.getNodeName();	        	
	        	
	        	Log.e("ION", nodeName);
	        	
	        	if (nodeName.equals("option"))
	        	{
	        		Element eElement = (Element)nd;	        			        		
	        		 	    	
	        		OptionGlue option = new OptionGlue();	        			  	        		
	        		
	        		option.type = Integer.parseInt(eElement.getElementsByTagName("type").item(0).getTextContent());
	        		option.def_value = Integer.parseInt(eElement.getElementsByTagName("def_value").item(0).getTextContent());
	        		option.value = option.def_value;	        		
	        		option.scriptFunc = eElement.getElementsByTagName("scriptFunc").item(0).getTextContent();
	    				    			
	    			options.add(option);
	        	}	        	
	        	
	        	if (nodeName.equals("sceneName"))
	        	{
	        		scene_name = nd.getTextContent();
	        	}
	        	
	        	nd = nd.getNextSibling();	        	
	        }
	    }
	    catch (IOException e1)
	    {
	    	e1.printStackTrace();
	    }	    	    
	       
		double_clink_val = settings.getInt("double_clink_val", 1);					
		
		for (int i=0;i<options.size();i++)
		{
			if (options.get(i).type == 0)
			{
				options.get(i).value = settings.getInt("options_val" + i, options.get(i).def_value);
			}			
		}	
	}
	
	public void onDrawFrame(GL10 gl)
	{
		long endTime = System.currentTimeMillis();
		long dt = endTime - startTime;
    
		if (dt < 33)
		{
			try
			{
				Thread.sleep(33 - dt);
            }
			catch (InterruptedException e)
			{
				e.printStackTrace();
            }
		}	
    
		startTime = System.currentTimeMillis();

		if (options != null)
		{
			for (int i=0;i<options.size();i++)
			{
				if (options.get(i).type == 0 || options.get(i).type == 2)
				{
					//Log.e("TAG", scene_name, options.get(i).scriptFunc + " " + options.get(i).value);
					//IonLib.CallFuncInScene(scene_name, options.get(i).scriptFunc, options.get(i).value, -1.0f, -1.0f, -1.0f, -1.0f);
				}
			}
		}
		
		IonActivity.activity.IonStep();		
	}

	public void onSurfaceChanged(GL10 gl, int width, int height)
	{		
		IonLib.Resize(width, height, height);
		LoadPreference(mngr, settings);
	}

	public void onSurfaceCreated(GL10 gl, EGLConfig config)
	{	
		IonActivity.activity.IonInit(false);
		startTime = System.currentTimeMillis();
	}
}
