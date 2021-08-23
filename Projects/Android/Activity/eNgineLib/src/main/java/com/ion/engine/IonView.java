/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.ion.engine;

/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


import java.io.IOException;
import java.lang.reflect.Array;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;

import android.content.Context;
import android.graphics.PixelFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.media.MediaPlayer;
import android.net.Uri;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.os.Environment;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Surface;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;


import android.view.SurfaceHolder;
import android.view.inputmethod.InputConnection;
import android.view.inputmethod.EditorInfo;
import android.text.InputType;
import android.view.inputmethod.BaseInputConnection;

import android.view.WindowManager;

/**
 * A simple GLSurfaceView sub-class that demonstrate how to perform
 * OpenGL ES 2.0 rendering into a GL Surface. Note the following important
 * details:
 *
 * - The class must use a custom context factory to enable 2.0 rendering.
 *   See ContextFactory class definition below.
 *
 * - The class must use a custom EGLConfigChooser to be able to select
 *   an EGLConfig that supports 2.0. This is done by providing a config
 *   specification to eglChooseConfig() that has the attribute
 *   EGL10.ELG_RENDERABLE_TYPE containing the EGL_OPENGL_ES2_BIT flag
 *   set. See ConfigChooser class definition below.
 *
 * - The class must select the surface's format, then choose an EGLConfig
 *   that matches it exactly (with regards to red/green/blue/alpha channels
 *   bit depths). Failure to do so would result in an EGL_BAD_MATCH error.
 */


public class IonView extends GLSurfaceView
{
    private static String TAG = "IonView";

    public Renderer renderer;

    public IonView(Context context)
    {
        super(context);

        // Create an OpenGL ES 2.0 context.
        setEGLContextClientVersion(2);

        // Set the Renderer for drawing on the GLSurfaceView
        renderer = new Renderer();
        setRenderer(renderer);

        // Render the view only when there is a change in the drawing data
        //setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }

    class Renderer implements GLSurfaceView.Renderer, SurfaceTexture.OnFrameAvailableListener
	{
        private final String vss =
                "attribute vec2 vPosition;\n" +
                        "attribute vec2 vTexCoord;\n" +
                        "varying vec2 texCoord;\n" +
                        "void main() {\n" +
                        "  texCoord = vTexCoord;\n" +
                        "  gl_Position = vec4 ( vPosition.x, vPosition.y, 0.0, 1.0 );\n" +
                        "}";

        private final String fss =
                "#extension GL_OES_EGL_image_external : require\n" +
                        "precision mediump float;\n" +
                        "uniform samplerExternalOES sTexture;\n" +
                        "varying vec2 texCoord;\n" +
                        "void main() {\n" +
                        "  gl_FragColor = texture2D(sTexture,texCoord);\n" +
                        "}";

        private int[] hTex;
        private FloatBuffer pVertex;
        private FloatBuffer pTexCoord;
        private int hProgram;

        private Camera mCamera;
        private SurfaceTexture mSTexture;

        private boolean mUpdateST = false;

        boolean mUpdateST_inites = false;

        public void EnableCamera(int tex)
        {
            Log.e("GL", "EnableCamera " + tex);

            float[] vtmp = {0.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f};
            float[] ttmp = {1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f};
            pVertex = ByteBuffer.allocateDirect(8 * 4).order(ByteOrder.nativeOrder()).asFloatBuffer();
            pVertex.put(vtmp);
            pVertex.position(0);
            pTexCoord = ByteBuffer.allocateDirect(8 * 4).order(ByteOrder.nativeOrder()).asFloatBuffer();
            pTexCoord.put(ttmp);
            pTexCoord.position(0);

            initTex();

            hTex[0] = tex;

            mSTexture = new SurfaceTexture(hTex[0]);
            mSTexture.setOnFrameAvailableListener(this);

            mCamera = Camera.open();
            try
            {
                mCamera.setPreviewTexture(mSTexture);
            }
            catch (IOException ioe)
            {
            }

            hProgram = loadShader(vss, fss);

            Camera.Parameters param = mCamera.getParameters();
            List<Camera.Size> psize = param.getSupportedPreviewSizes();

            if (psize.size() > 0)
            {
                param.setPreviewSize(psize.get(0).width, psize.get(0).height);
            }

            param.set("orientation", "landscape");
            mCamera.setParameters(param);
            mCamera.startPreview();

            mUpdateST_inites = true;

            /*Log.e("GL", "EnableCamera " + tex);

            mSTexture = new SurfaceTexture ( tex );
            mSTexture.setOnFrameAvailableListener(this);

            mCamera = Camera.open();

            try
            {
                mCamera.setPreviewTexture(mSTexture);
            }
            catch ( IOException ioe )
            {
            }

            Camera.Parameters param = mCamera.getParameters();
            List<Camera.Size> psize = param.getSupportedPreviewSizes();

            if ( psize.size() > 0 )
            {
                int i = 0;

                param.setPreviewSize(psize.get(i).width, psize.get(i).height);
                //Log.i("mr","ssize: "+psize.get(i).width+", "+psize.get(i).height);
            }
            param.set("orientation", "landscape");
            mCamera.setParameters ( param );
            mCamera.startPreview();*/
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config)
        {
            //String extensions = GLES20.glGetString(GLES20.GL_EXTENSIONS);
            //Log.i("mr", "Gl extensions: " + extensions);
            //Assert.assertTrue(extensions.contains("OES_EGL_image_external"));

			IonActivity.activity.IonInit(false);
            //IonLib.SetCameraTexture(hTex[0]);
        }

        public synchronized void onFrameAvailable ( SurfaceTexture st )
        {
            Log.e("GL", "onFrameAvailable");
            mUpdateST = true;
        }

        private void initTex()
        {
            hTex = new int[1];
            /*GLES20.glGenTextures(1, hTex, 0);
            GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, hTex[0]);
            GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
            GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);
            GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_NEAREST);
            GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_NEAREST);*/
        }

        public void onDrawFrame(GL10 gl)
		{
            IonActivity.activity.IonStep();

            if (mUpdateST_inites)
            {
                synchronized (this) {
                    if (mUpdateST) {
                        mSTexture.updateTexImage();
                        mUpdateST = false;
                    }
                }

                GLES20.glUseProgram(hProgram);

                int ph = GLES20.glGetAttribLocation(hProgram, "vPosition");
                int tch = GLES20.glGetAttribLocation(hProgram, "vTexCoord");
                int th = GLES20.glGetUniformLocation(hProgram, "sTexture");

                GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
                GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, hTex[0]);
                GLES20.glUniform1i(th, 0);

                GLES20.glVertexAttribPointer(ph, 2, GLES20.GL_FLOAT, false, 4 * 2, pVertex);
                GLES20.glVertexAttribPointer(tch, 2, GLES20.GL_FLOAT, false, 4 * 2, pTexCoord);
                GLES20.glEnableVertexAttribArray(ph);
                GLES20.glEnableVertexAttribArray(tch);

                GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
                GLES20.glFlush();
            }
        }

        public void onSurfaceChanged(GL10 gl, int width, int height)
		{
            GLES20.glViewport(0, 0, width, height);

            if (mUpdateST_inites)
            {
                Camera.Parameters param = mCamera.getParameters();
                List<Camera.Size> psize = param.getSupportedPreviewSizes();
                if (psize.size() > 0) {
                    int i;
                    for (i = 0; i < psize.size(); i++) {
                        if (psize.get(i).width < width || psize.get(i).height < height)
                            break;
                    }

                    if (i > 0)
                        i--;
                    param.setPreviewSize(psize.get(i).width, psize.get(i).height);
                    //Log.i("mr","ssize: "+psize.get(i).width+", "+psize.get(i).height);
                }
                param.set("orientation", "portrait");
                mCamera.setParameters(param);
                mCamera.startPreview();
            }

			DisplayMetrics displaymetrics = new DisplayMetrics();
			IonActivity.activity.getWindowManager().getDefaultDisplay().getMetrics(displaymetrics);

			Log.e("GL", "onSurfaceChanged");
			IonLib.Resize(width, height, displaymetrics.heightPixels);
        }

        private int loadShader(String vss, String fss) {
            int vshader = GLES20.glCreateShader(GLES20.GL_VERTEX_SHADER);
            GLES20.glShaderSource(vshader, vss);
            GLES20.glCompileShader(vshader);
            int[] compiled = new int[1];
            GLES20.glGetShaderiv(vshader, GLES20.GL_COMPILE_STATUS, compiled, 0);
            if (compiled[0] == 0) {
                Log.e("Shader", "Could not compile vshader");
                Log.v("Shader", "Could not compile vshader:" + GLES20.glGetShaderInfoLog(vshader));
                GLES20.glDeleteShader(vshader);
                vshader = 0;
            }

            int fshader = GLES20.glCreateShader(GLES20.GL_FRAGMENT_SHADER);
            GLES20.glShaderSource(fshader, fss);
            GLES20.glCompileShader(fshader);
            GLES20.glGetShaderiv(fshader, GLES20.GL_COMPILE_STATUS, compiled, 0);
            if (compiled[0] == 0) {
                Log.e("Shader", "Could not compile fshader");
                Log.v("Shader", "Could not compile fshader:" + GLES20.glGetShaderInfoLog(fshader));
                GLES20.glDeleteShader(fshader);
                fshader = 0;
            }

            int program = GLES20.glCreateProgram();
            GLES20.glAttachShader(program, vshader);
            GLES20.glAttachShader(program, fshader);
            GLES20.glLinkProgram(program);

            return program;
        }
    }

    /*MainRenderer mRenderer;

    IonView ( Context context ) {
        super ( context );
        mRenderer = new MainRenderer(this);
        setEGLContextClientVersion ( 2 );
        setRenderer ( mRenderer );
        setRenderMode ( GLSurfaceView.RENDERMODE_WHEN_DIRTY );
    }

    public void surfaceCreated ( SurfaceHolder holder )
    {
        super.surfaceCreated ( holder );
    }

    public void surfaceDestroyed ( SurfaceHolder holder )
    {
        mRenderer.close();
        super.surfaceDestroyed ( holder );
    }

    public void surfaceChanged ( SurfaceHolder holder, int format, int w, int h )
    {
        super.surfaceChanged ( holder, format, w, h );
    }

    // Renderer
    class MainRenderer implements GLSurfaceView.Renderer, SurfaceTexture.OnFrameAvailableListener
    {
        private final String vss =
                "attribute vec2 vPosition;\n" +
                        "attribute vec2 vTexCoord;\n" +
                        "varying vec2 texCoord;\n" +
                        "void main() {\n" +
                        "  texCoord = vTexCoord;\n" +
                        "  gl_Position = vec4 ( vPosition.x, vPosition.y, 0.0, 1.0 );\n" +
                        "}";

        private final String fss =
                "#extension GL_OES_EGL_image_external : require\n" +
                        "precision mediump float;\n" +
                        "uniform samplerExternalOES sTexture;\n" +
                        "varying vec2 texCoord;\n" +
                        "void main() {\n" +
                        "  gl_FragColor = texture2D(sTexture,texCoord);\n" +
                        "}";

        private int[] hTex;
        private FloatBuffer pVertex;
        private FloatBuffer pTexCoord;
        private int hProgram;

        private Camera mCamera;
        private SurfaceTexture mSTexture;

        private boolean mUpdateST = false;

        private IonView mView;

        MainRenderer(IonView view) {
            mView = view;
            float[] vtmp = {1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f};
            float[] ttmp = {1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f};
            pVertex = ByteBuffer.allocateDirect(8 * 4).order(ByteOrder.nativeOrder()).asFloatBuffer();
            pVertex.put(vtmp);
            pVertex.position(0);
            pTexCoord = ByteBuffer.allocateDirect(8 * 4).order(ByteOrder.nativeOrder()).asFloatBuffer();
            pTexCoord.put(ttmp);
            pTexCoord.position(0);
        }

        public void close() {
            mUpdateST = false;
            mSTexture.release();
            mCamera.stopPreview();
            mCamera = null;
            deleteTex();
        }

        public void onSurfaceCreated(GL10 unused, EGLConfig config) {
            //String extensions = GLES20.glGetString(GLES20.GL_EXTENSIONS);
            //Log.i("mr", "Gl extensions: " + extensions);
            //Assert.assertTrue(extensions.contains("OES_EGL_image_external"));

            initTex();
            mSTexture = new SurfaceTexture(hTex[0]);
            mSTexture.setOnFrameAvailableListener(this);

            mCamera = Camera.open();
            try {
                mCamera.setPreviewTexture(mSTexture);
            } catch (IOException ioe) {
            }

            GLES20.glClearColor(1.0f, 1.0f, 0.0f, 1.0f);

            hProgram = loadShader(vss, fss);
        }

        public void onDrawFrame(GL10 unused) {
            GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);

            synchronized (this) {
                if (mUpdateST) {
                    mSTexture.updateTexImage();
                    mUpdateST = false;
                }
            }

            GLES20.glUseProgram(hProgram);

            int ph = GLES20.glGetAttribLocation(hProgram, "vPosition");
            int tch = GLES20.glGetAttribLocation(hProgram, "vTexCoord");
            int th = GLES20.glGetUniformLocation(hProgram, "sTexture");

            GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
            GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, hTex[0]);
            GLES20.glUniform1i(th, 0);

            GLES20.glVertexAttribPointer(ph, 2, GLES20.GL_FLOAT, false, 4 * 2, pVertex);
            GLES20.glVertexAttribPointer(tch, 2, GLES20.GL_FLOAT, false, 4 * 2, pTexCoord);
            GLES20.glEnableVertexAttribArray(ph);
            GLES20.glEnableVertexAttribArray(tch);

            GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
            GLES20.glFlush();
        }

        public void onSurfaceChanged(GL10 unused, int width, int height) {
            GLES20.glViewport(0, 0, width, height);
            Camera.Parameters param = mCamera.getParameters();
            List<Camera.Size> psize = param.getSupportedPreviewSizes();
            if (psize.size() > 0) {
                int i;
                for (i = 0; i < psize.size(); i++) {
                    if (psize.get(i).width < width || psize.get(i).height < height)
                        break;
                }
                if (i > 0)
                    i--;
                param.setPreviewSize(psize.get(i).width, psize.get(i).height);
                //Log.i("mr","ssize: "+psize.get(i).width+", "+psize.get(i).height);
            }
            param.set("orientation", "landscape");
            mCamera.setParameters(param);
            mCamera.startPreview();
        }

        private void initTex() {
            hTex = new int[1];
            GLES20.glGenTextures(1, hTex, 0);
            GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, hTex[0]);
            GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
            GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);
            GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_NEAREST);
            GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_NEAREST);
        }

        private void deleteTex() {
            GLES20.glDeleteTextures(1, hTex, 0);
        }

        public synchronized void onFrameAvailable(SurfaceTexture st) {
            mUpdateST = true;
            mView.requestRender();
        }

        private int loadShader(String vss, String fss) {
            int vshader = GLES20.glCreateShader(GLES20.GL_VERTEX_SHADER);
            GLES20.glShaderSource(vshader, vss);
            GLES20.glCompileShader(vshader);
            int[] compiled = new int[1];
            GLES20.glGetShaderiv(vshader, GLES20.GL_COMPILE_STATUS, compiled, 0);
            if (compiled[0] == 0) {
                Log.e("Shader", "Could not compile vshader");
                Log.v("Shader", "Could not compile vshader:" + GLES20.glGetShaderInfoLog(vshader));
                GLES20.glDeleteShader(vshader);
                vshader = 0;
            }

            int fshader = GLES20.glCreateShader(GLES20.GL_FRAGMENT_SHADER);
            GLES20.glShaderSource(fshader, fss);
            GLES20.glCompileShader(fshader);
            GLES20.glGetShaderiv(fshader, GLES20.GL_COMPILE_STATUS, compiled, 0);
            if (compiled[0] == 0) {
                Log.e("Shader", "Could not compile fshader");
                Log.v("Shader", "Could not compile fshader:" + GLES20.glGetShaderInfoLog(fshader));
                GLES20.glDeleteShader(fshader);
                fshader = 0;
            }

            int program = GLES20.glCreateProgram();
            GLES20.glAttachShader(program, vshader);
            GLES20.glAttachShader(program, fshader);
            GLES20.glLinkProgram(program);

            return program;
        }
    }*/
}
