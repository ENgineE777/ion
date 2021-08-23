
package com.ion.engine;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.opengl.GLU;

import java.lang.Thread;

public class IonRenderer
{	
	long startTime;

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

		//IonLib.Step();		
	}

	public void onSurfaceChanged(GL10 gl, int width, int height)
	{
		//IonLib.Resize(width, height);
	}

	public void onSurfaceCreated(GL10 gl, EGLConfig config)
	{
		//IonLib.Init(false);		

		startTime = System.currentTimeMillis();
	}
	
	public void release()
	{

	}
}
