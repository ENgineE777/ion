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
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.graphics.PixelFormat;
import android.graphics.SurfaceTexture;
import android.media.MediaPlayer;
import android.net.Uri;
import android.opengl.GLSurfaceView;
import android.text.InputType;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Surface;
import android.view.View;
import android.view.WindowManager;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputConnection;
import android.view.inputmethod.BaseInputConnection;

public class IonInputView extends View
{
    private static String TAG = "IonInputView";              
    
    public IonInputView(Context context)
    {
        super(context);        
    }
    public IonInputView(Context context, AttributeSet attrs)
    {
        super(context, attrs);       
    }         

    /*@Override
    public InputConnection onCreateInputConnection(EditorInfo outAttrs)
    {
    	Log.e(TAG, "onCreateInputConnection IonInputView " + IonActivity.activity.cur_kb_type + " " + IonActivity.activity.cur_rk_type);

		/*BaseInputConnection fic = new BaseInputConnection(this, false);
		outAttrs.actionLabel = null;
		outAttrs.inputType = EditorInfo.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD | EditorInfo.TYPE_TEXT_FLAG_NO_SUGGESTIONS | EditorInfo.TYPE_TEXT_VARIATION_FILTER;
		outAttrs.imeOptions = EditorInfo.IME_ACTION_NEXT;

		return fic;*/

    	/*InputConnection connection = super.onCreateInputConnection(outAttrs);
    	outAttrs.imeOptions |= EditorInfo.IME_FLAG_NO_FULLSCREEN;
      
    	if (IonActivity.activity.cur_kb_type.equals("URL"))
    	{
    		outAttrs.inputType = EditorInfo.TYPE_TEXT_VARIATION_URI;
    	}
    	else
    	if (IonActivity.activity.cur_kb_type.equals("Number"))
        {
        	outAttrs.inputType = EditorInfo.TYPE_CLASS_NUMBER;
        }
        else
        if (IonActivity.activity.cur_kb_type.equals("Phone"))
        {
          	outAttrs.inputType = EditorInfo.TYPE_CLASS_PHONE;
        }
        else
        if (IonActivity.activity.cur_kb_type.equals("Email"))
        {
        	outAttrs.inputType = EditorInfo.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD;
        }               
        else
        {
        	outAttrs.inputType = EditorInfo.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD;        
        }
    	
    	    	
    	if (IonActivity.activity.cur_rk_type.equals("Next"))
    	{
    		outAttrs.imeOptions |= EditorInfo.IME_ACTION_NEXT;
    	}
    	else
    	if (IonActivity.activity.cur_rk_type.equals("Done"))
        {
    		outAttrs.imeOptions |= EditorInfo.IME_ACTION_DONE;
        }
        else
        if (IonActivity.activity.cur_rk_type.equals("Go"))
        {
        	outAttrs.imeOptions |= EditorInfo.IME_ACTION_GO;
        }
        else
        if (IonActivity.activity.cur_rk_type.equals("Search"))
        {
        	outAttrs.imeOptions |= EditorInfo.IME_ACTION_SEARCH;
        }    	
    	
    	return connection;
    }*/

	@Override
	public InputConnection onCreateInputConnection(EditorInfo outAttrs)
	{
		/*outAttrs.actionLabel = "";
		outAttrs.hintText = "";
		outAttrs.initialCapsMode = 0;
		outAttrs.initialSelEnd = outAttrs.initialSelStart = -1;
		outAttrs.label = "";
		outAttrs.imeOptions = EditorInfo.IME_ACTION_UNSPECIFIED | EditorInfo.IME_FLAG_NO_EXTRACT_UI;
		outAttrs.inputType = InputType.TYPE_CLASS_TEXT;*/

		outAttrs.imeOptions |= EditorInfo.IME_FLAG_NO_FULLSCREEN;

		if (IonActivity.activity.cur_kb_type.equals("URL"))
		{
			outAttrs.inputType = EditorInfo.TYPE_TEXT_VARIATION_URI;
		}
		else
		if (IonActivity.activity.cur_kb_type.equals("Number"))
		{
			outAttrs.inputType = EditorInfo.TYPE_CLASS_NUMBER;
		}
		else
		if (IonActivity.activity.cur_kb_type.equals("Phone"))
		{
			outAttrs.inputType = EditorInfo.TYPE_CLASS_PHONE;
		}
		else
		if (IonActivity.activity.cur_kb_type.equals("Email"))
		{
			outAttrs.inputType = EditorInfo.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD;
		}
		else
		{
			outAttrs.inputType = EditorInfo.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD;
		}


		if (IonActivity.activity.cur_rk_type.equals("Next"))
		{
			outAttrs.imeOptions |= EditorInfo.IME_ACTION_NEXT;
		}
		else
		if (IonActivity.activity.cur_rk_type.equals("Done"))
		{
			outAttrs.imeOptions |= EditorInfo.IME_ACTION_DONE;
		}
		else
		if (IonActivity.activity.cur_rk_type.equals("Go"))
		{
			outAttrs.imeOptions |= EditorInfo.IME_ACTION_GO;
		}
		else
		if (IonActivity.activity.cur_rk_type.equals("Search"))
		{
			outAttrs.imeOptions |= EditorInfo.IME_ACTION_SEARCH;
		}

		final InputConnection in = new BaseInputConnection(this, false)
		{
			int prev_len = 0;

			/*private HudInputElement getInputElement()
			{
				...
			}*/

			@Override
			public boolean setComposingText(CharSequence text,
											int newCursorPosition)
			{
				Log.d("ion", "composing text: " + text + "," + newCursorPosition);

				String str = text.toString();

				if (prev_len != str.length())
				{
					if (str.length() > 0)
					{
						int keyCode = text.toString().charAt(str.length() - 1);

						if (prev_len > str.length())
						{
							keyCode = 8;
						}

						Log.d("ion", "setComposingText ACTION_DOWN " + keyCode);

						IonLib.SetLastChar(keyCode);
					}

					prev_len = str.length();
				}

				/*HudInputElement input = getInputElement();

				if(input!=null)
				{
					input.setComposingText(text.toString());
				}*/

				return super.setComposingText(text, newCursorPosition);
			}


			@Override
			public boolean finishComposingText()
			{
				/*HudInputElement input = getInputElement();
				if(input!=null){
					input.doneComposing();
				}*/

				return super.finishComposingText();
			}

			@Override
			public boolean commitText(CharSequence text, int newCursorPosition)
			{
				Log.d("ion", "commit:" + text.toString() + "," + this.getEditable().toString());

				prev_len = 0;

				/*HudInputElement input = getInputElement();
				if(input!=null)
				{
					input.doneComposing();
				}*/

				return super.commitText(text, newCursorPosition);
			}
		};

		return in;
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event)
	{
		Log.d("ion", "Fired onKeyDown " + keyCode);

		if (keyCode == KeyEvent.KEYCODE_UNKNOWN && event.getAction() == KeyEvent.ACTION_MULTIPLE)
		{
			Log.d("ion", "ACTION_DOWN " + keyCode);
		}

		char ct = event.getDisplayLabel();
	    
		int chr = event.getUnicodeChar();

		if (keyCode == 67)
		{
			chr = 8;
		}

		if (keyCode == 66)
		{
			chr = 9;
		}		

		IonLib.SetLastChar(chr);
		Log.d("ion", "ACTION_DOWN " + keyCode + " " + ct);        
		Log.d("ion", Character.toString(Character.toChars(event.getUnicodeChar())[0]));

		
	    return super.onKeyDown(keyCode, event);
	}

	@Override
	public boolean onKeyMultiple (int keyCode, int count, KeyEvent event)
	{
		Log.d("ion", "Fired onKeyMultiple " + keyCode);

		if (keyCode == KeyEvent.KEYCODE_UNKNOWN && event.getAction() == KeyEvent.ACTION_MULTIPLE)
		{			
			String str = event.getCharacters();
			Log.d("ion", "onKeyMultiple " + event.getCharacters());  
			IonLib.SetLastChar(str.charAt(0));
		}
		
	    return super.onKeyDown(keyCode, event);
	}
}
