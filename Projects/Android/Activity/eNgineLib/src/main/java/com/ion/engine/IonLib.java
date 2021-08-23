/*
 * Copyright (C) 2007 The Android Open Source Project
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
// Wrapper for native library

public class IonLib
{     
     public static void Load()
     {
    	 System.loadLibrary("ENgine");
     }
     
     public static native void SetApkName(String apk_name);
	 public static native void ActivateResourcePack(String pack_name);
	 public static native void SetBasePath(String base_path);
	 public static native void SetLoadFromApk(boolean set_load);

	 public static native void SetLocale(String locale_name);	
	      
     public static native void SetScreenDPI(int dpi);
	 public static native void Resize(int width, int height, int disp_height);             
     
	 public static native void TouchStart(int index,int dx, int dy);
	 public static native void TouchMove(int index, int dx, int dy);
	 public static native void TouchEnd(int index);

	 public static native void SetLastChar(int chr);
	 public static native void SetSoftKeyboardHeight(int hgt);

	 public static native void SetInternetConnected(int set);

	 public static native void SetBackBtnStateOn();

	 public static native void SetGPSData(int status, float latitude, float longitude);
		
	 public static native void  SetAllowCall(Boolean set_allow);
	 
	 public static native void  UserStatsSetFloat(String id, float def_value, float value, Boolean add);
	 public static native void  UserStatsSetInt(String id, int def_value, int value, Boolean add);
	 public static native float UserStatsGetFloat(String id, float def_value);
	 public static native int   UserStatsGetInt(String id, int def_value);

	 public static native String GetRequestedItemName();  
	 public static native void ResetItemName();	
	 public static native void ResolveRequestedItem(Boolean state);	
	
	 public static native void   StartGetCall();
	 public static native String GetCallType();
	 public static native void   StopGetCall();
	
	 public static native String GetCall1Name(int index);

	 public static native String GetCall2Name(int index);
	 public static native String GetCall2Param1(int index);

	 public static native void SetShareRes(String data);

	 public static native void SetSignInStatusFB(int res, String data);
	 public static native void SetSignInStatusVK(int res, String data, String user);
	 public static native void SetSignInStatusInstagram(int res, String data); 
	 
	 public static native void SetSelectImageResult(int res, String image);
	 
	 public static native void ProcessPushNotificationUserData(String userdata);
	 public static native void SchedulePushNotificationUserData(String userdata);
	 
	 public static native void ProcessAdsData(String data);
	 
	 public static native void SetGyro(float ax, float ay, float az);
	 
	 public static native void SetContactsCount(int num);
	 public static native void SetContactData(int i, String name, String numbers);
	 public static native void FinalizeReadContacts();
	 public static native void SetContactAvatar(String name, String texture);
	 public static native void FinalizeReadAvatars();	 
	 
	 public static native void SetSendSmsResult(int res);
	 public static native void SetPopupResult(int res);

	 public static native void SetSimOperatorName(String name);
	 public static native void SetDeviceVendor(String name);
	 public static native void SetDeviceName(String name);
	 public static native void SetDeviceID(String name);
	 	 
	 public static native void MobileSetOnPause(int set);

	 public static native void SetBatteryLevel(float level);
	 public static native void SetBatterySatus(int status);
	 public static native void SetInstallationTime(long status);

	 public static native int GetProjectTheme();
	 public static native int GetProjectOrientation();

	 public static native void StartRequest(String method, String request, String post_params, String response_file);
	 
	 public static native void PasteFromClipboard(String text);	
	 public static native void OnRequestIsClipboardEmpty(int bSet);	
}
