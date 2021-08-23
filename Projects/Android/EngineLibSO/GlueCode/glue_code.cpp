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

// OpenGL ES 2.0 code

#include <jni.h>

#include "core/core.h"
#include "Core/Network/NetLogger.h"


extern "C"
{
	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetApkName(JNIEnv * env, jobject obj, jstring apk_name);	
	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_ActivateResourcePack(JNIEnv * env, jobject obj, jstring pack_name);
	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetBasePath(JNIEnv * env, jobject obj, jstring base_path);
	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetLoadFromApk(JNIEnv * env, jobject obj, jboolean set_load);

	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetLocale(JNIEnv * env, jobject obj, jstring locale_name);	
    
	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetScreenDPI(JNIEnv * env, jobject obj, jint dpi);	

	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_Resize(JNIEnv * env, jobject obj,  jint width, jint height, jint disp_height);

	JNIEXPORT void JNICALL Java_com_ion_engine_IonActivity_IonInit(JNIEnv * env, jobject obj, jboolean need_activate_pack);
    JNIEXPORT void JNICALL Java_com_ion_engine_IonActivity_IonStep(JNIEnv * env, jobject obj);	

	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_TouchStart(JNIEnv * env, jobject obj, jint index, jint mx, jint my);
	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_TouchMove(JNIEnv * env, jobject obj, jint index, jint mx, jint my);
	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_TouchEnd(JNIEnv * env, jobject obj, jint index);

	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetLastChar(JNIEnv * env, jobject obj, jint chr);
	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetSoftKeyboardHeight(JNIEnv * env, jobject obj, jint hgt);
	
	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetInternetConnected(JNIEnv * env, jobject obj, int set);	

	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetGPSData(JNIEnv * env, jobject obj, int status, jfloat latitude, jfloat longitude);
	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetAllowCall(JNIEnv * env, jobject obj, bool set_allow);

	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_UserStatsSetFloat(JNIEnv* env, jobject obj,  jstring id, jfloat def_value, jfloat value, bool add);
	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_UserStatsSetInt(JNIEnv* env, jobject obj,  jstring id, jint def_value, jint value, bool add);
	JNIEXPORT float JNICALL Java_com_ion_engine_IonLib_UserStatsGetFloat(JNIEnv* env, jobject obj,  jstring id, jfloat def_value);
	JNIEXPORT int   JNICALL Java_com_ion_engine_IonLib_UserStatsGetInt(JNIEnv * env, jobject obj, jstring id, jint def_value);
	
	JNIEXPORT jstring Java_com_ion_engine_IonLib_SetBackBtnStateOn(JNIEnv * env, jobject obj);

	JNIEXPORT jstring Java_com_ion_engine_IonLib_GetRequestedItemName(JNIEnv * env, jobject obj);
	JNIEXPORT void Java_com_ion_engine_IonLib_ResetItemName(JNIEnv * env, jobject obj);
	JNIEXPORT void Java_com_ion_engine_IonLib_ResolveRequestedItem(JNIEnv * env, jobject obj, jboolean state);		

	JNIEXPORT void Java_com_ion_engine_IonLib_SetShareRes(JNIEnv * env, jobject obj, jstring data);

	JNIEXPORT void Java_com_ion_engine_IonLib_SetSignInStatusFB(JNIEnv * env, jobject obj, jint res, jstring data);
	JNIEXPORT void Java_com_ion_engine_IonLib_SetSignInStatusVK(JNIEnv * env, jobject obj, jint res, jstring data, jstring user);
	JNIEXPORT void Java_com_ion_engine_IonLib_SetSignInStatusInstagram(JNIEnv * env, jobject obj, jint res, jstring data);		

	JNIEXPORT void Java_com_ion_engine_IonLib_SetSelectImageResult(JNIEnv * env, jobject obj, jint res, jstring image);

	JNIEXPORT void Java_com_ion_engine_IonLib_ProcessPushNotificationUserData(JNIEnv * env, jobject obj, jstring userdata);
	JNIEXPORT void Java_com_ion_engine_IonLib_SchedulePushNotificationUserData(JNIEnv * env, jobject obj, jstring userdata);

	JNIEXPORT void Java_com_ion_engine_IonLib_ProcessAdsData(JNIEnv * env, jobject obj, jstring data);

	JNIEXPORT void Java_com_ion_engine_IonLib_SetGyro(JNIEnv * env, jobject obj, jfloat ax, jfloat ay, jfloat az);

	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetContactsCount(JNIEnv * env, jobject obj, jint num);
	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetContactData(JNIEnv * env, jobject obj, jint i, jstring name, jstring numbers);
	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_FinalizeReadContacts(JNIEnv * env, jobject obj);
	
	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetContactAvatar(JNIEnv * env, jobject obj, jstring name, jstring texture);
	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_FinalizeReadAvatars(JNIEnv * env, jobject obj);

	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetSendSmsResult(JNIEnv * env, jobject obj, jint res);
	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetPopupResult(JNIEnv * env, jobject obj, jint res);
	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetSimOperatorName(JNIEnv * env, jobject obj, jstring name);
	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetDeviceVendor(JNIEnv * env, jobject obj, jstring name);
	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetDeviceName(JNIEnv * env, jobject obj, jstring name);
	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetDeviceID(JNIEnv * env, jobject obj, jstring name);	

	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetBatteryLevel(JNIEnv * env, jobject obj, jfloat level);
	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetBatterySatus(JNIEnv * env, jobject obj, jint status);
	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetInstallationTime(JNIEnv * env, jobject obj, jlong status);


	JNIEXPORT int JNICALL Java_com_ion_engine_IonLib_GetProjectTheme(JNIEnv * env, jobject obj);
	JNIEXPORT int JNICALL Java_com_ion_engine_IonLib_GetProjectOrientation(JNIEnv * env, jobject obj);	

	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_MobileSetOnPause(JNIEnv * env, jobject obj, int set);

	JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_StartRequest(JNIEnv * env, jobject obj, jstring method, jstring request, jstring post_params, jstring response_file);

    JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_PasteFromClipboard(JNIEnv * env, jobject obj, jstring text);	
    JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_OnRequestIsClipboardEmpty(JNIEnv * env, jobject obj, jint bSet);	
};

pthread_mutex_t mutex_caller;
bool mutex_inited_caller = false;

bool was_inited = false;

int delta_touch_y = 0;

struct timespec lastTime;

ThreadLocker locker;

float gyros[3];

pthread_mutex_t mutex;
bool mutex_inited = false;

bool wrong_texture_format = false;
int debug_info_type = 0;
int project_orient_mode = 0;
int project_theme = 0;

JNIEnv* cur_env = null;
jobject cur_obj;

float android_extern_batterylevel = 0.0f;
int android_extern_batterystatus = 0;
int64 android_extern_installationtime = 0;

void CallFromJava(const char* function)
{	    
	jclass cls = cur_env->GetObjectClass(cur_obj);	

	//core->TraceTo("app", "calling %s", function);

	if (cls == null)
	{
		core->TraceTo("app", "class not found");		
	}
	else
	{			
		jmethodID mid = cur_env->GetMethodID(cls, function, "()V");

		if (mid == null)
		{
			core->TraceTo("app", "func not found");			
		}
		else
		{
			cur_env->CallVoidMethod(cur_obj, mid);			
		}
	}	
}

void CallFromJava(const char* function, const char* param)
{
	jstring jstr = cur_env->NewStringUTF(param);

	jclass cls = cur_env->GetObjectClass(cur_obj);

	if (cls == null)
	{
		core->TraceTo("app", "class not found");		
	}
	else
	{	
		jmethodID mid = cur_env->GetMethodID(cls, function, "(Ljava/lang/String;)V");

		if (mid == null)
		{
			core->TraceTo("app", "func not found");			
		}
		else
		{
			cur_env->CallVoidMethod(cur_obj, mid, jstr);
		}
	}	
}

inline long TimeDifference(const struct timespec& last, const struct timespec& current)
{    
    if (current.tv_sec > last.tv_sec)
        return (1000000000 - last.tv_nsec) + current.tv_nsec;
    else
        return current.tv_nsec - last.tv_nsec;
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetApkName(JNIEnv * env, jobject obj, jstring apk_name)
{
	jboolean isCopy;
	const char* str_id = (const char*)env->GetStringUTFChars(apk_name, &isCopy);
		
	core->Files()->SetApkName(str_id);

	env->ReleaseStringUTFChars(apk_name, str_id);
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_PasteFromClipboard(JNIEnv * env, jobject obj, jstring text)
{
    jboolean isCopy;
    const char* str = (const char*)env->GetStringUTFChars(text, &isCopy);
    core->Mobile()->PasteFromClipboard(str);

	env->ReleaseStringUTFChars(text, str);
}

 JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_OnRequestIsClipboardEmpty(JNIEnv * env, jobject obj, jint bSet)
{
   core->Mobile()->OnRequestIsClipboardEmpty(bSet);
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetBasePath(JNIEnv * env, jobject obj, jstring base_path)
{
	jboolean isCopy;
	const char* str_id = (const char*)env->GetStringUTFChars(base_path, &isCopy);

	core->Files()->SetBasePath(str_id);

	env->ReleaseStringUTFChars(base_path, str_id);
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetLoadFromApk(JNIEnv * env, jobject obj, jboolean set_load)
{
	core->Files()->SetLoadFromApk(set_load);
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_ActivateResourcePack(JNIEnv * env, jobject obj, jstring pack_name)
{
	jboolean isCopy;
	const char* str_id = (const char*)env->GetStringUTFChars(pack_name, &isCopy);

	core->Files()->ActivateResourcePack(str_id, true);

	env->ReleaseStringUTFChars(pack_name, str_id);
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetLocale(JNIEnv * env, jobject obj, jstring locale_name)
{
	jboolean isCopy;
	const char* locale = (const char*)env->GetStringUTFChars(locale_name, &isCopy);
	
	en_core.SetLocale(locale);	

	env->ReleaseStringUTFChars(locale_name, locale);
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonActivity_IonInit(JNIEnv * env, jobject obj, jboolean need_activate_pack)
{				
	gyros[0] = gyros[1] = gyros[2] = 0.0f;

	if (!mutex_inited)
	{
		pthread_mutex_init(&mutex, NULL);
		mutex_inited = true;
	}
		
	if (!mutex_inited_caller)
	{
		pthread_mutex_init(&mutex_caller, NULL);
		mutex_inited_caller = true;
	}

	pthread_mutex_lock(&mutex);	
	//locker.Init();

	cur_env = env;
	cur_obj = obj;

	if (was_inited)
	{
		core->TraceTo("App","Recreate resources");
		//en_core.Render()->ReCreateResources();

		pthread_mutex_unlock(&mutex);		

		return;
	}
		
	if (need_activate_pack) core->Files()->ActivateResourcePack("Resource.pack", true);
		
	core->TraceTo("App", "core.Init()");

	core->Init();

	en_core.EN_Scene()->RegisterSceneName("editor/system_declations.srt");

	//core.Render()->SetScreenDPI(dpi_hight);

	IFileBuffer* fb = core->Files()->LoadFile("project");

	if (fb)
	{
		byte* ptr = fb->GetBuffer();

		int sz = 0;
		sz = *((int*)ptr);
		ptr += 4;

		for (int i=0;i<sz;i++)
		{					
			en_core.EN_Scene()->RegisterSceneName((const char*)ptr);
			ptr += 256;
		}

		debug_info_type = *((dword*)ptr);
		ptr += 4;		

		int textures_mode = *((dword*)ptr);
		ptr += 4;

		project_theme = *((dword*)ptr);
		ptr += 4;

		project_orient_mode = *((dword*)ptr);
		ptr += 4;	

		int ref_height = *((dword*)ptr);
		ptr += 4;

		en_core.Render()->Display()->SetReferenceHeight(ref_height);

		//if (en_core.Render()->Display()->SetTextureType((RenderServiceIOS::TextureType)textures_mode))
		{
			wrong_texture_format = false;
			en_core.EN_Scene()->Load((const char*)ptr);	
		}
		//else
		{
			//wrong_texture_format = true;			
		}

		fb->Release();
	}

    if (debug_info_type > 0)
        core->CreateNetLogger();

	clock_gettime(CLOCK_MONOTONIC, &lastTime);

	was_inited = true;

	pthread_mutex_unlock(&mutex);	
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetScreenDPI(JNIEnv * env, jobject obj, jint dpi)
{
	en_core.Render()->Display()->SetScreenDPI(dpi);
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_Resize(JNIEnv * env, jobject obj,  jint width, jint height, jint disp_height)
{
	delta_touch_y = height - disp_height;

	core->TraceTo("App", "ApplyVideoMode %i %i %i", width, height, delta_touch_y);

	RenderService::VideoMode mode;
	mode.width = width;
	mode.height = height;

	en_core.Render()->Display()->ApplyVideoMode(mode);
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonActivity_IonStep(JNIEnv * env, jobject obj)
{			
	if (!was_inited) return;

	cur_env = env;
	cur_obj = obj;
	
	//locker.Lock();
	pthread_mutex_lock(&mutex);

	struct timespec currTime;
	clock_gettime(CLOCK_MONOTONIC, &currTime);
	long iNanoSeconds = TimeDifference(lastTime, currTime);	
	lastTime = currTime;

	en_core.SetDeltaTime((float)iNanoSeconds/1000000000.0f);
	core->CountDeltaTime(true);

	/*if (wrong_texture_format)
	{
		if (supported_tex_type == RenderServiceIOS::tex_bitmap)
		{
			en_core.Render()->PrintText(10,10,up_left, COLOR_GREEN, "Resurces exported with wrong format. Needed in Bitmap.");
		}
		else
		if (supported_tex_type == RenderServiceIOS::tex_atc)
		{
			en_core.Render()->PrintText(10,10,up_left, COLOR_GREEN, "Resurces exported with wrong format. Needed in ATC or Bitmap.");
		}
		else
		if (supported_tex_type == RenderServiceIOS::tex_pvrtc)
		{
			en_core.Render()->PrintText(10,10,up_left, COLOR_GREEN, "Resurces exported with wrong format. Needed in PVRTC or Bitmap.");
		}  
		else
		if (supported_tex_type == RenderServiceIOS::tex_dds)
		{
			en_core.Render()->Debug()->PrintText(10,10,up_left, COLOR_GREEN, "Resurces exported with wrong format. Needed in DDS or Bitmap.");
		}
	}*/
	
	//core.Render()->PrintText(10,10,up_right, COLOR_GREEN, "%i", core.GetFPS());

	//en_core.Render()->PrintText(10,10,up_right, COLOR_GREEN, "%4.3f %4.3f %4.3f", gyros[0], gyros[1], gyros[2]);

	core->Controls()->SetGyro(0, gyros[1]);
	core->Controls()->SetGyro(1, gyros[0]);
	core->Controls()->SetGyro(2, gyros[2]);

	//struct mallinfo mi;
    //mi = mallinfo();
	//core.Render()->PrintText(10,10,up_left, COLOR_GREEN, "%i %i", mi.fordblks, mi.uordblks);	

	if (debug_info_type > 0)
		en_core.Render()->Debug()->PrintText(10, 10, up_right, COLOR_GREEN, "%i", core->GetFPS());	

	if (debug_info_type == 2)
	{
		int tex_total = 0;
		int tex_loading = 0;
		int tex_memory = 0;

		en_core.Render()->GetTextureInfo(tex_total, tex_loading, tex_memory);

		float flt_tex_memory = tex_memory / (1024 * 1024);
		en_core.Render()->Debug()->PrintText(10, 35, up_right, COLOR_GREEN, "C: %i L: %i M: %4.1f Mb", tex_total, tex_loading, flt_tex_memory);		
	}

	pthread_mutex_lock(&mutex_caller);

	en_core.Work(); 		

	pthread_mutex_unlock(&mutex_caller);	
	
	//locker.UnLock();
	pthread_mutex_unlock(&mutex);
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_TouchStart(JNIEnv * env, jobject obj, jint index, jint mx, jint my)
{	
	core->Controls()->TouchStart(index, mx, my + delta_touch_y);
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_TouchMove(JNIEnv * env, jobject obj, jint index, jint mx, jint my)
{
	core->Controls()->TouchMove(index, mx, my + delta_touch_y);	
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_TouchEnd(JNIEnv * env, jobject obj, jint index)
{
	core->Controls()->TouchEnd(index);
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetLastChar(JNIEnv * env, jobject obj, jint chr)
{
	core->Controls()->SetLastChar((wchar_t)chr);
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetSoftKeyboardHeight(JNIEnv * env, jobject obj, jint hgt)
{	
	core->Controls()->SetSoftKeyboardHeight(en_core.Render()->Display()->GetHeight() - hgt);
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetInternetConnected(JNIEnv * env, jobject obj, int set)
{
	en_core.Network()->SetInternetConnected(set);
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetGPSData(JNIEnv * env, jobject obj, int status, jfloat latitude, jfloat longitude)
{
	en_core.Mobile()->SetGPSData(status, latitude, longitude);	
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetAllowCall(JNIEnv * env, jobject obj, bool set_allow)
{
	en_core.Mobile()->SetAllowCall(set_allow);
}

JNIEXPORT void Java_com_ion_engine_IonLib_UserStatsSetFloat(JNIEnv* env, jobject obj,  jstring id, jfloat def_value, jfloat value, bool add)
{
	jboolean isCopy;
	const char* str_id = (const char*)env->GetStringUTFChars(id, &isCopy);
	
	core->UserStats()->SetFloat(str_id, def_value, value, add);

	env->ReleaseStringUTFChars(id, str_id);
}

JNIEXPORT void Java_com_ion_engine_IonLib_UserStatsSetInt(JNIEnv* env, jobject obj,  jstring id, jint def_value, jint value, bool add)
{
	jboolean isCopy;
	const char* str_id = (const char*)env->GetStringUTFChars(id, &isCopy);

	core->UserStats()->SetInt(str_id, def_value, value, add);

	env->ReleaseStringUTFChars(id, str_id);
}

JNIEXPORT float Java_com_ion_engine_IonLib_UserStatsGetFloat(JNIEnv* env, jobject obj,  jstring id, jfloat def_value)
{
	jboolean isCopy;
	const char* str_id = (const char*)env->GetStringUTFChars(id, &isCopy);

	float val = core->UserStats()->GetFloat(str_id, def_value);

	env->ReleaseStringUTFChars(id, str_id);

	return val;
}

JNIEXPORT int Java_com_ion_engine_IonLib_UserStatsGetInt(JNIEnv * env, jobject obj, jstring id, jint def_value)
{
	jboolean isCopy;
	const char* str_id = (const char*)env->GetStringUTFChars(id, &isCopy);

	int val = core->UserStats()->GetInt(str_id, def_value);

	env->ReleaseStringUTFChars(id, str_id);

	return val;
}

JNIEXPORT jstring Java_com_ion_engine_IonLib_SetBackBtnStateOn(JNIEnv * env, jobject obj)
{
	core->Controls()->SetBackBtnStateOn();
}

JNIEXPORT jstring Java_com_ion_engine_IonLib_GetRequestedItemName(JNIEnv * env, jobject obj)
{					
	if (!was_inited)
	{
		return env->NewStringUTF("");
	}

	//locker.Lock();
	pthread_mutex_lock(&mutex);

	jstring str;
	
	if (was_inited && en_core.OnlineUserStats()->GetRequestedItemName())
	{
		str = env->NewStringUTF(en_core.OnlineUserStats()->GetRequestedItemName());
	}
	else
	{
		str = env->NewStringUTF("");
	}
	
	//locker.UnLock();
	pthread_mutex_unlock(&mutex);

	return str;
}

JNIEXPORT void Java_com_ion_engine_IonLib_ResetItemName(JNIEnv * env, jobject obj)
{					
	if (!was_inited) return;	

	//locker.Lock();
	pthread_mutex_lock(&mutex);

	en_core.OnlineUserStats()->ResetItemName();	

	//locker.UnLock();
	pthread_mutex_unlock(&mutex);
}

JNIEXPORT void Java_com_ion_engine_IonLib_ResolveRequestedItem(JNIEnv * env, jobject obj, jboolean state)
{
	if (!was_inited) return;
	
	//locker.Lock();
	pthread_mutex_lock(&mutex);

	if (was_inited) en_core.OnlineUserStats()->ResolveRequestedItem(state);	

	//locker.UnLock();	
	pthread_mutex_unlock(&mutex);
}

JNIEXPORT void Java_com_ion_engine_IonLib_SetShareRes(JNIEnv * env, jobject obj, jstring data)
{
	jboolean isCopy;
	const char* str_result = (const char*)env->GetStringUTFChars(data, &isCopy);

	en_core.Mobile()->SetShareRes(str_result);

	env->ReleaseStringUTFChars(data, str_result);
}

JNIEXPORT void Java_com_ion_engine_IonLib_SetSignInStatusFB(JNIEnv * env, jobject obj, jint res, jstring data)
{
	jboolean isCopy;
	const char* str_result = (const char*)env->GetStringUTFChars(data, &isCopy);

	en_core.OnlineUserStats()->Facebook()->SetSignInStatus(res, str_result);

	env->ReleaseStringUTFChars(data, str_result);
}

JNIEXPORT void Java_com_ion_engine_IonLib_SetSignInStatusVK(JNIEnv * env, jobject obj, jint res, jstring data, jstring user)
{
	jboolean isCopy;
	const char* str_result = (const char*)env->GetStringUTFChars(data, &isCopy);
	const char* str_user = (const char*)env->GetStringUTFChars(user, &isCopy);

	en_core.OnlineUserStats()->VK()->SetSignInStatus(res, str_result, str_user);

	env->ReleaseStringUTFChars(data, str_result);
	env->ReleaseStringUTFChars(user, str_user);
}

JNIEXPORT void Java_com_ion_engine_IonLib_SetSignInStatusInstagram(JNIEnv * env, jobject obj, jint res, jstring data)
{
	jboolean isCopy;
	const char* str_result = (const char*)env->GetStringUTFChars(data, &isCopy);	

	en_core.OnlineUserStats()->Instagram()->SetSignInStatus(res, str_result);

	env->ReleaseStringUTFChars(data, str_result);
}

JNIEXPORT void Java_com_ion_engine_IonLib_SetSelectImageResult(JNIEnv * env, jobject obj, jint res, jstring image)
{
	jboolean isCopy;
	const char* str_image = (const char*)env->GetStringUTFChars(image, &isCopy);	

	en_core.Mobile()->SetSelectImageResult(res, str_image);

	env->ReleaseStringUTFChars(image, str_image);
}

JNIEXPORT void Java_com_ion_engine_IonLib_ProcessPushNotificationUserData(JNIEnv * env, jobject obj, jstring userdata)
{
	jboolean isCopy;
	const char* str_userdata = (const char*)env->GetStringUTFChars(userdata, &isCopy);

	en_core.Mobile()->ProcessData(str_userdata);

	env->ReleaseStringUTFChars(userdata, str_userdata);
}

JNIEXPORT void Java_com_ion_engine_IonLib_SchedulePushNotificationUserData(JNIEnv * env, jobject obj, jstring userdata)
{
	jboolean isCopy;
	const char* str_userdata = (const char*)env->GetStringUTFChars(userdata, &isCopy);

	en_core.Mobile()->ScheduleData(str_userdata);

	env->ReleaseStringUTFChars(userdata, str_userdata);
}

JNIEXPORT void Java_com_ion_engine_IonLib_ProcessAdsData(JNIEnv * env, jobject obj, jstring data)
{
	jboolean isCopy;
	const char* str_data = (const char*)env->GetStringUTFChars(data, &isCopy);

	//en_core.Ads()->ProcessData(str_data);

	env->ReleaseStringUTFChars(data, str_data);
}

JNIEXPORT void Java_com_ion_engine_IonLib_SetGyro(JNIEnv * env, jobject obj, jfloat ax, jfloat ay, jfloat az)
{	
	gyros[0] = ax;
	if (fabs(gyros[0])<0.075f) gyros[0] = 0.00f;

	gyros[1] = ay;
	if (fabs(gyros[1])<0.075f) gyros[1] = 0.00f;
	
	gyros[2] = az;
	if (fabs(gyros[2])<0.075f) gyros[2] = 0.00f;
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetContactsCount(JNIEnv * env, jobject obj, jint num)
{
	en_core.Mobile()->SetContactsCount(num);
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetContactData(JNIEnv * env, jobject obj, jint i, jstring name, jstring numbers)
{
	jboolean isCopy;
	const char* str_name = (const char*)env->GetStringUTFChars(name, &isCopy);	
	const char* str_numbers = (const char*)env->GetStringUTFChars(numbers, &isCopy);

	en_core.Mobile()->SetContactData( i, str_name, str_numbers );

	env->ReleaseStringUTFChars(name, str_name);	
	env->ReleaseStringUTFChars(numbers, str_numbers);
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_FinalizeReadContacts(JNIEnv * env, jobject obj)
{
	en_core.Mobile()->FinalizeReadContatcs();
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetContactAvatar(JNIEnv * env, jobject obj, jstring name, jstring texture)
{
	jboolean isCopy;
	const char* str_name = (const char*)env->GetStringUTFChars(name, &isCopy);
	const char* str_texture = (const char*)env->GetStringUTFChars(texture, &isCopy);	

	en_core.Mobile()->SetContactAvatar(str_name, str_texture);

	env->ReleaseStringUTFChars(name, str_name);
	env->ReleaseStringUTFChars(texture, str_texture);	
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_FinalizeReadAvatars(JNIEnv * env, jobject obj)
{
	en_core.Mobile()->FinalizeReadAvatars();
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetSendSmsResult(JNIEnv * env, jobject obj, jint res)
{
	en_core.Mobile()->SetSendSmsResult(res);
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetPopupResult(JNIEnv * env, jobject obj, jint res)
{
	en_core.Mobile()->SetPopupResult(res);
}


JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetSimOperatorName(JNIEnv * env, jobject obj, jstring name)
{
	jboolean isCopy;
	const char* str_name = (const char*)env->GetStringUTFChars(name, &isCopy);

	en_core.Mobile()->SetSimOperatorName( str_name );

	env->ReleaseStringUTFChars(name, str_name);
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetDeviceVendor(JNIEnv * env, jobject obj, jstring name)
{
	jboolean isCopy;
	const char* str_name = (const char*)env->GetStringUTFChars(name, &isCopy);

	en_core.SetDeviceVendor( str_name );

	env->ReleaseStringUTFChars(name, str_name);
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetDeviceName(JNIEnv * env, jobject obj, jstring name)
{
	jboolean isCopy;
	const char* str_name = (const char*)env->GetStringUTFChars(name, &isCopy);

	en_core.SetDeviceName( str_name );

	env->ReleaseStringUTFChars(name, str_name);
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetDeviceID(JNIEnv * env, jobject obj, jstring name)
{
	jboolean isCopy;
	const char* str_name = (const char*)env->GetStringUTFChars(name, &isCopy);

	en_core.SetDeviceID( str_name );

	env->ReleaseStringUTFChars(name, str_name);
}

void LoadProject()
{
	core->Files()->Init();

	IFileBuffer* fb = core->Files()->LoadFile("project");

	if (fb)
	{
		byte* ptr = fb->GetBuffer();

		int sz = 0;
		sz = *((int*)ptr);
		ptr += 4;

		for (int i=0;i<sz;i++)
		{			
			ptr += 256;
		}
		
		ptr += 4;		
		ptr += 4;

		project_theme = *((dword*)ptr);
		ptr += 4;

		project_orient_mode = *((dword*)ptr);
		ptr += 4;		

		fb->Release();
	}
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetBatteryLevel(JNIEnv * env, jobject obj, jfloat level)
{
	android_extern_batterylevel = level;
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetBatterySatus(JNIEnv * env, jobject obj, jint status)
{
	android_extern_batterystatus = status;
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_SetInstallationTime(JNIEnv * env, jobject obj, jlong inst_time)
{
	android_extern_installationtime = inst_time;
}

JNIEXPORT int JNICALL Java_com_ion_engine_IonLib_GetProjectTheme(JNIEnv * env, jobject obj)
{
	LoadProject();

	return project_theme;
}

JNIEXPORT int JNICALL Java_com_ion_engine_IonLib_GetProjectOrientation(JNIEnv * env, jobject obj)
{
	LoadProject();

	return project_orient_mode;
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_MobileSetOnPause(JNIEnv * env, jobject obj, int set)
{
	en_core.Mobile()->SetOnPause(set > 0);
	en_core.Mobile()->ExecutePauseCallbacks();
}

JNIEXPORT void JNICALL Java_com_ion_engine_IonLib_StartRequest(JNIEnv * env, jobject obj, jstring method, jstring request, jstring post_params, jstring response_file)
{
	jboolean isCopy;
	const char* str_method = (const char*)env->GetStringUTFChars(method, &isCopy);
	const char* str_request = (const char*)env->GetStringUTFChars(request, &isCopy);
	const char* str_post_params = (const char*)env->GetStringUTFChars(post_params, &isCopy);
	const char* str_response_file = (const char*)env->GetStringUTFChars(response_file, &isCopy);

	core->Network()->StartRequest(null, null, str_method, str_request, str_post_params, str_response_file);

	env->ReleaseStringUTFChars(response_file, str_response_file);
	env->ReleaseStringUTFChars(post_params, str_post_params);
	env->ReleaseStringUTFChars(request, str_request);
	env->ReleaseStringUTFChars(method, str_method);
}