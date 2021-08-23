
#include "EditorProject.h"
#include "Common/Utils.h"
#include "Core/Core.h"

#include "Common/libxml/tinyxml2.h"
#include "Editor/Helpers/SceneInfo.h"

#ifdef OSUNIX

#include <unistd.h>
#include <dirent.h>

#endif

namespace ProjectInfo
{

    int EditorProject::android_tex_format = 0;
    bool EditorProject::force_ios_bitmap = false;

    void EditorProject::ExporterNetListiner::OnRequestFinish(int res, void* data, const char* file_name)
    {
        EditorProject* ed_core = (EditorProject*)data;

        String::Copy(ed_core->exm_img_src, 256, file_name);
        ed_core->exp_img_ready = true;
    }

    void EditorProject::ConvertTexToAndroid(const char* in, const char* out, const char* ext, Platform platform)
    {		
        char command[1024];		

        char tmp[1024];
        String::Copy(tmp, 1024, out);
        String::RemoveExctention(tmp);
        String::Cat(tmp, 1024, ".bmp");

        RenderService::TextureData texture_data;
        String::Copy(texture_data.fl_name, 512, out);

        if (!texture_data.GetInfo())
        {	
            return;
        }	

        if (android_tex_format == 0 || String::IsEqual(ext, "tga") || force_ios_bitmap)
        {
            texture_data.width = GetPow(texture_data.width);
            texture_data.height = GetPow(texture_data.height);	

            DeleteFile(out);

            //sprintf(command, "\"\"%sBin\\nconvert.exe\" -out bmp -o \"%s\" -swap bgr -yflip -resize %i %i -32bits \"%s%s\"\"",
            //	en_core.EN_Files()->GetAppDirectory(), out, srcInfo.Width, srcInfo.Height, en_core.EN_Files()->GetAppDirectory(), in);

#ifdef PC
            sprintf(command, "\"\"%s/bin/nconvert.exe\" -out bmp -o \"%s\" -swap bgr -yflip -32bits \"%s%s\"\"", en_core.EN_Files()->GetAppDirectory(), out, en_core.EN_Files()->GetAppDirectory(), in);		
#endif

#ifdef OSUNIX
            sprintf(command, "'%sBin/nconvert' -out bmp -o '%s' -swap bgr -yflip -32bits '%s%s'", en_core.EN_Files()->GetAppDirectory(), out, en_core.EN_Files()->GetAppDirectory(), in);
#endif


            //int ret = system(command);
            RunSilent(command, "");				

            //rename(tmp, out);
        }
        else
            if (android_tex_format == 1)
            {
                sprintf(command, "\"\"%sBin\\Convert2ATITC.exe\" \"%s%s\" \"%s\"\"",
                    en_core.EN_Files()->GetAppDirectory(), en_core.EN_Files()->GetAppDirectory(), in, out);

                RunSilent(command, "");	
            }
            else
                if (android_tex_format == 2)
                {
                    DeleteFile(out);

                    if (texture_data.width >= 110 && texture_data.height >= 110)
                    {
                        texture_data.width = GetPow(texture_data.width);
                        texture_data.height = GetPow(texture_data.height);

                        if (texture_data.width > texture_data.height)
                        {
                            texture_data.height = texture_data.width;
                        }
                        else
                            if (texture_data.width < texture_data.height)
                            {
                                texture_data.width = texture_data.height;
                            }			

                            sprintf(command, "\"\"%sBin\\nconvert.exe\" -out bmp -o \"%s\" -yflip -resize %i %i -32bits \"%s%s\"\"",
                                en_core.EN_Files()->GetAppDirectory(), out, texture_data.width, texture_data.height, en_core.EN_Files()->GetAppDirectory(), in);		

                            RunSilent(command, "");

                            rename(out, tmp);

                            sprintf(command, "\"\"%sBin\\PVRTexTool.exe\" -fOGLPVRTC4 -i\"%s\" -o\"%s\"",
                                en_core.EN_Files()->GetAppDirectory(), tmp, out);

                            //sprintf(command, "\"\"%sBin\\PVRTexTool.exe\" -legacypvr -f PVRTC4 -p -square + -i \"%s%s\" -o \"%s\"\"",
                            //	core->Files()->GetAppDirectory(), core->Files()->GetAppDirectory(), in, out);

                            //int ret = system(command);
                            RunSilent(command, "");

                            DeleteFile(out);
                            DeleteFile(tmp);

                            String::Copy(tmp, 1024, out);
                            String::RemoveExctention(tmp);
                            String::Cat(tmp, 1024, ".pvr");

                            rename(tmp, out);
                    }
                    else
                    {
                        texture_data.width = GetPow(texture_data.width);
                        texture_data.height = GetPow(texture_data.height);			

                        sprintf(command, "\"\"%sBin\\nconvert.exe\" -out bmp -o \"%s\" -swap bgr -yflip -resize %i %i -32bits \"%s%s\"\"",
                            en_core.EN_Files()->GetAppDirectory(), out, texture_data.width, texture_data.height, en_core.EN_Files()->GetAppDirectory(), in);		

                        RunSilent(command, "");
                    }
                }
                else
                    if (android_tex_format == 3)
                    {
                        String::Copy(tmp, 1024, out);
                        String::RemoveExctention(tmp);
                        String::Cat(tmp, 1024, ".png");

                        char tmp2[1024];
                        String::Copy(tmp2, 1024, out);
                        String::RemoveExctention(tmp2);
                        String::Cat(tmp2, 1024, ".tmp");

                        texture_data.width = GetPow(texture_data.width);
                        texture_data.height = GetPow(texture_data.height);

                        DeleteFile(out);

                        sprintf(command, "\"\"%sBin\\nconvert.exe\" -out png -o \"%s\" -resize %i %i \"%s%s\"\"",
                            en_core.EN_Files()->GetAppDirectory(), tmp, texture_data.width, texture_data.height, en_core.EN_Files()->GetAppDirectory(), in);		

                        int k = RunSilent(command, "");

                        char tmp_file[1024];

                        String::Copy(tmp_file, 1024, in);
                        String::RemoveExctention(tmp_file);
                        String::Cat(tmp_file, 1024, ".bat");

                        int len = strlen(tmp_file);

                        for (int i=0;i<len;i++)
                        {
                            if (tmp_file[i] == '/' || tmp_file[i] == '\\')
                            {
                                tmp_file[i] = '_';
                            }
                        }

                        FILE* fl = fopen(tmp_file, "w");		

                        sprintf(command, "\"%sBin\\DDS\\nvdxt.exe\" -file \"%s\" -output \"%s\" -dxt1a",
                            en_core.EN_Files()->GetAppDirectory(), tmp, tmp2);		

                        fwrite(command, strlen(command), 1, fl);

                        fclose(fl);

                        //sprintf(command, "\"\"%sBin\\PVRTexTool.exe\" -dds -fDXT1 -i\"%s\" -o\"%s\"\"",
                        //    core->Files()->GetAppDirectory(), tmp, out);

                        //sprintf(command, "\"\"%sBin\\PVRTexTool.exe\" -legacypvr -f PVRTC4 -p -square + -i \"%s%s\" -o \"%s\"\"",
                        //	core->Files()->GetAppDirectory(), core->Files()->GetAppDirectory(), in, out);

                        //int ret = system("exe.bat");
                        k = RunSilent(tmp_file, "");

                        DeleteFile(tmp_file);

                        DeleteFile(tmp);
                        rename(tmp2, out);		
                    }	
                    else
                        if (android_tex_format == 4)
                        {
                            char tmp[1024];		
                            String::ExtractFileName(out, tmp);

                            int len = strlen(tmp);

                            for (int i=0;i<len;i++)
                            {
                                if (tmp[i] == '(' || tmp[i] == ')')
                                {
                                    tmp[i] = 'E';
                                }
                            }

                            char tmp2[1024];		
                            String::ExtractPath(out, tmp2, false);
                            String::Cat(tmp2, 1024, tmp);
                            String::FixSlashes(tmp2);
                            //String::RemoveExctention(tmp2);

                            //String::Cat(tmp, 1024, ext);

                            texture_data.width = GetPow(texture_data.width);
                            texture_data.height = GetPow(texture_data.height);		

                            DeleteFile(out);

                            sprintf(command, "\"\"%s/bin/nconvert\" -out png -o \"%s\" -resize %i %i \"%s%s\"\"",
                                en_core.EN_Files()->GetAppDirectory(), tmp2, texture_data.width, texture_data.height, en_core.EN_Files()->GetAppDirectory(), in);

                            //int ret = system(command);
                            int ret = RunSilent(command, "");				

                            bool has_alpha = false;

                            if (texture_data.bytesPerPixel == 4)
                            {			
                                has_alpha = true;			
                            }

                            char tmp3[1024];
                            String::Copy(tmp3, 1024, tmp2);
                            String::RemoveExctention(tmp3);
                            String::Cat(tmp3, 1024, ".pkm");

                            char sub_path[512];
                            String::ExtractPath(in, sub_path, false);
                            sub_path[strlen(sub_path)-1] = 0;

                            char tmp_file[1024];

                            String::Copy(tmp_file, 1024, tmp);
                            //String::Copy(tmp_file, 1024, sub_path);
                            //String::Cat(tmp_file, 1024, tmp);
                            String::RemoveExctention(tmp_file);
                            int k = rnd() * 1000.0f;
                            char str[8];
                            String::Printf(str, 8, "%i", k);
                            String::Cat(tmp_file, 1024, str);
                            String::Cat(tmp_file, 1024, ".bat");

                            String::ExtractPath(out, sub_path, false);
                            sub_path[strlen(sub_path)-1] = 0;		

                            char tmp_file2[1024];
                            String::Printf(tmp_file2, 1024,"%sBin\\etc\\%s", en_core.EN_Files()->GetAppDirectory(), tmp_file);

                            FILE* fl = fopen(tmp_file2, "w");		

                            sprintf(command, "chdir \"%sBin\\etc\" \n",
                                en_core.EN_Files()->GetAppDirectory());		

                            //sprintf(command, "%sBin\\atc", core->Files()->GetAppDirectory());		
                            //SetCurrentDirectory(command);

                            fwrite(command, strlen(command), 1, fl);

                            //fclose(fl);
                            //ret = RunSilent(tmp_file2, "");

                            //fl = fopen(tmp_file2, "w");		

                            if (has_alpha)
                            {
                                sprintf(command, "\"%s/Bin/etc/etcpack.exe\" \"%s\" \"%s\" -c etc1 -as\n",
                                    en_core.EN_Files()->GetAppDirectory(), tmp2, sub_path);
                            }
                            else
                            {
                                sprintf(command, "\"%s/Bin/etc/etcpack.exe\" \"%s\" \"%s\" -c etc1\n",
                                    en_core.EN_Files()->GetAppDirectory(), tmp2, sub_path);
                            }

                            fwrite(command, strlen(command), 1, fl);

                            //sprintf(command, "pause");
                            //fwrite(command, strlen(command), 1, fl);

                            fclose(fl);

                            ret = RunSilent(tmp_file2, "");
                            //ret = system(tmp_file2);		

                            DeleteFile(tmp_file2);

                            DeleteFile(out);

                            //rename(tmp3, out);

                            fl = fopen(out, "wb");

                            dword marker = 0;

                            if (has_alpha)  marker = 1;

                            fwrite(&marker, 4, 1, fl);

                            //AddFile(fl, tmp3);		

                            if (has_alpha)
                            {
                                String::RemoveExctention(tmp3);
                                String::Cat(tmp3, 1024, "_alpha.pkm");

                                //AddFile(fl, tmp3);		
                            }

                            fclose(fl);
                        }
    }

    void EditorProject::ConvertTexToIOS(const char* in, const char* out, const char* ext, Platform platform)
    {			
        if (force_ios_bitmap > 0)
        {
            ConvertTexToAndroid(in, out, ext, platform);

            return;
        }

        char tmp[1024];
        String::Copy(tmp, 1024, out);
        String::RemoveExctention(tmp);
        String::Cat(tmp, 1024, ".bmp");	

        char command[1024];

        RenderService::TextureData texture_data;
        String::Copy(texture_data.fl_name, 512, out);

        if (!texture_data.GetInfo())
        {	
            return;
        }		

        DeleteFile(out);

        if (texture_data.width >= 110 && texture_data.height >= 110)
        {	
            texture_data.width = GetPow(texture_data.width);
            texture_data.height = GetPow(texture_data.height);

            if (texture_data.width > texture_data.height)
            {
                texture_data.height = texture_data.width;
            }
            else
                if (texture_data.width < texture_data.height)
                {
                    texture_data.width = texture_data.height;
                }

                sprintf(command, "\"\"%s/bin/nconvert.exe\" -out bmp -o \"%s\" -yflip -resize %i %i -32bits \"%s%s\"\"",
                    en_core.EN_Files()->GetAppDirectory(), out, texture_data.width, texture_data.height, en_core.EN_Files()->GetAppDirectory(), in);		

                RunSilent(command, "");

                rename(out, tmp);

                sprintf(command, "\"\"%s/bin/PVRTexTool.exe\" -fOGLPVRTC4 -i\"%s\" -o\"%s\"",
                    en_core.EN_Files()->GetAppDirectory(), tmp, out);

                //sprintf(command, "\"\"%sBin\\PVRTexTool.exe\" -legacypvr -f PVRTC4 -p -square + -i \"%s%s\" -o \"%s\"\"",
                //	core->Files()->GetAppDirectory(), core->Files()->GetAppDirectory(), in, out);

                //int ret = system(command);
                RunSilent(command, "");

                DeleteFile(out);
                DeleteFile(tmp);

                String::Copy(tmp, 1024, out);
                String::RemoveExctention(tmp);
                String::Cat(tmp, 1024, ".pvr");

                rename(tmp, out);
        }
        else
        {
            int k = texture_data.width - (int)((float)texture_data.width / 2) * 2;

            if (k == 1) texture_data.width += 1;

            k = texture_data.height - (int)((float)texture_data.height / 2) * 2;
            if (k == 1) texture_data.height += 1;		

            sprintf(command, "\"\"%s/bin/nconvert.exe\" -out bmp -o \"%s\" -swap bgr -yflip -resize %i %i \"%s%s\"\"",
                en_core.EN_Files()->GetAppDirectory(), out, texture_data.width, texture_data.height, en_core.EN_Files()->GetAppDirectory(), in);		

            RunSilent(command, "");
        }
    }

    void EditorProject::ConvertFBX(const char* in, const char* out, const char* ext, Platform platform)
    {
        //en_core.Models()->Save(in, out, platform);		
    }

    void EditorProject::ConvertAnimFBX(const char* in, const char* out, const char* ext, Platform platform)
    {
        //((AnimationServiceImp*)core->Animation())->Save(in, out, platform);	
    }

    void EditorProject::ConvertXML(const char* in, const char* out, const char* ext, Platform platform)
    {
        char tmp[1024];
        String::Copy(tmp, 1024, in);

        tmp[strlen(tmp) - 1] = 'b';

        CopyFileEX(tmp, out);
        //((AnimationServiceImp*)core->Animation())->Save(in, out, platform);	
    }

    EditorProject::EditorProject()
    {			
        project_path[0] = 0;	

        exp_img_ready = false;
        exm_img_src[0] = 0;	

        exp_func_registered = false;

        progress_callback = null;
    }

    void EditorProject::RegisterExportFunc()
    {			
        if (exp_func_registered)
        {
            return;
        }

        //en_core.EN_Files()->RegisterPostConverter(&RegisterExportedFile);

        en_core.EN_Files()->RegisterConverter(platform_pc, "fbx", &ConvertFBX);
        en_core.EN_Files()->RegisterConverter(platform_pc, "fbb", &ConvertAnimFBX);
        en_core.EN_Files()->RegisterConverter(platform_pc, "xml", &ConvertXML);

        //core->Files()->RegisterConverter(platform_ios, "png", &ConvertTexToAndroid);		
        //core->Files()->RegisterConverter(platform_ios, "dds", &ConvertTexToAndroid);		
        //core->Files()->RegisterConverter(platform_ios, "jpg", &ConvertTexToAndroid);		

        en_core.EN_Files()->RegisterConverter(platform_ios, "png", &ConvertTexToIOS);		
        en_core.EN_Files()->RegisterConverter(platform_ios, "dds", &ConvertTexToIOS);		
        en_core.EN_Files()->RegisterConverter(platform_ios, "jpg", &ConvertTexToIOS);		
        en_core.EN_Files()->RegisterConverter(platform_ios, "bmp", &ConvertTexToIOS);
        en_core.EN_Files()->RegisterConverter(platform_ios, "xml", &ConvertXML);

        en_core.EN_Files()->RegisterConverter(platform_ios, "tga", &ConvertTexToAndroid);		
        en_core.EN_Files()->RegisterConverter(platform_ios, "fbx", &ConvertFBX);
        en_core.EN_Files()->RegisterConverter(platform_ios, "fbb", &ConvertAnimFBX);

        en_core.EN_Files()->RegisterConverter(platform_android, "png", &ConvertTexToAndroid);		
        en_core.EN_Files()->RegisterConverter(platform_android, "dds", &ConvertTexToAndroid);		
        en_core.EN_Files()->RegisterConverter(platform_android, "jpg", &ConvertTexToAndroid);		
        en_core.EN_Files()->RegisterConverter(platform_android, "tga", &ConvertTexToAndroid);	
        en_core.EN_Files()->RegisterConverter(platform_android, "bmp", &ConvertTexToAndroid);	
        en_core.EN_Files()->RegisterConverter(platform_android, "fbx", &ConvertFBX);
        en_core.EN_Files()->RegisterConverter(platform_android, "fbb", &ConvertAnimFBX);
        en_core.EN_Files()->RegisterConverter(platform_android, "xml", &ConvertXML);

        exp_func_registered = true;
    }

    void EditorProject::ResetProject()
    {
        if (!project.CurTarget())
        {
            return;
        }
        Target* pTarget = project.CurTarget();
        pTarget->name[0] = 0;
        pTarget->cur_ed_scene = -1;
        pTarget->cur_ed_file = -1;
        pTarget->start_scene = -1;
        project.orintation = 0;
        String::Copy(project.ios_theme, 128, "No Title bar");
        project.status_bar_is_white = 1;
        String::Copy(project.droid_theme, 128, "Fullscreen");

        String::Copy(pTarget->icon_path, 256, "Editor/mobileapp.png");

        String::Copy(project.package_name, 256, "com.mobileapp");
        String::Copy(project.package_version_name, 256, "1.0");
        String::Copy(project.package_code_name, 256, "1");

        String::Copy(project.bundle_name, 256, "com.mobileapp");
        String::Copy(project.bundle_short_name, 256, "mobileapp");	
        String::Copy(project.bundle_version, 256, "1.0");	

        project.need_isi = 1;

        project.fb_id[0] = 0;
        project.vk_id[0] = 0;	

        project.additional_permissions = 0;
        pTarget->force_ios_bitmap = 0;
        pTarget->debug_info_type = 0;

		pTarget->ref_width = 1024;
        pTarget->ref_height = 768;

        project.hack_ios_ext[0] = 0;
    }

    void EditorProject::SetProjectPath(const char* name)
    {
        String::Copy(project_path, 512, name);	
        core->SetProjectPath(name);
    }

    void EditorProject::OpenProject(ProjectInfo& project, const char* name)
    {	
        IStreamLoader* loader = core->Files()->CreateStreamLoader(name, "project", IStreamLoader::xml);	
        if (loader)
        {		
            if (!loader->EnterBlock("Target"))
            {
                project.targets.resize(1);
                project.sel_target = 0;

                Target* pTarget = project.CurTarget();
                loader->Read("Name", pTarget->name, 128);
                loader->Read("Icon", pTarget->icon_path, 256);

                String::ExtractFileName(name, pTarget->ex_name);
                String::RemoveExctention(pTarget->ex_name);

				loader->Read("RefWidth", pTarget->ref_width);
                loader->Read("RefHeight", pTarget->ref_height);
                en_core.Render()->Display()->SetReferenceHeight(pTarget->ref_height);

                loader->Read("Package_name", project.package_name, 256);
                loader->Read("Package_version_name", project.package_version_name, 256);
                loader->Read("Package_code_name", project.package_code_name, 256);		

                loader->Read("bundle_name", project.bundle_name, 256);
                loader->Read("bundle_short_name", project.bundle_short_name, 256);
                loader->Read("bundle_version", project.bundle_version, 256);				

                loader->Read("need_isi", project.need_isi);							

                loader->Read("fb_id", project.fb_id, 256);
                loader->Read("vk_id", project.vk_id, 256);							

                loader->Read("hack_ios_ext", project.hack_ios_ext, 256);										

                loader->Read("additional_permissions", project.additional_permissions);

                loader->Read("force_ios_bitmap", pTarget->force_ios_bitmap);		

                loader->Read("debug_info_type", pTarget->debug_info_type);		

                loader->Read("Orientation", project.orintation);
                loader->Read("IOSTheme", project.ios_theme, 128);
                loader->Read("IOSIsWhiteStatusBar", project.status_bar_is_white);		
                loader->Read("DROIDTheme", project.droid_theme, 128);
                loader->Read("start_scene", pTarget->start_scene);
                loader->Read("cur_ed_scene", pTarget->cur_ed_scene);
                loader->Read("cur_ed_file", pTarget->cur_ed_file);

                pTarget->Load(loader);
            }				

            loader->Release();
        }
        else 
            project_path[0] = 0;
    }

    void EditorProject::SaveProject()
    {
        if (!project_path[0])
        {
            return;
        }
        Target* pTarget = project.CurTarget();
        pTarget->Save();
        IStreamSaver* saver = core->Files()->CreateStreamSaver(project_path, IStreamSaver::xml);	
        String::ExtractFileName(project_path, pTarget->ex_name);
        String::RemoveExctention(pTarget->ex_name);

        if (saver)
        {
            saver->MarkBeginBlock("project");

            saver->Write("Name", pTarget->name);
            saver->Write("Icon", pTarget->icon_path);

			saver->Write("RefWidth", pTarget->ref_width);
            saver->Write("RefHeight", pTarget->ref_height);

            saver->Write("Package_name", project.package_name);
            saver->Write("Package_version_name", project.package_version_name);
            saver->Write("Package_code_name", project.package_code_name);		

            saver->Write("bundle_name", project.bundle_name);
            saver->Write("bundle_short_name", project.bundle_short_name);
            saver->Write("bundle_version", project.bundle_version);				

            saver->Write("need_isi", project.need_isi);							

            saver->Write("fb_id", project.fb_id);
            saver->Write("vk_id", project.vk_id);					

            saver->Write("hack_ios_ext", project.hack_ios_ext);										

            saver->Write("additional_permissions", project.additional_permissions);

            saver->Write("force_ios_bitmap", pTarget->force_ios_bitmap);		

            saver->Write("debug_info_type", pTarget->debug_info_type);		

            saver->Write("Orientation", project.orintation);
            saver->Write("IOSTheme", project.ios_theme);
            saver->Write("IOSIsWhiteStatusBar", project.status_bar_is_white);		
            saver->Write("DROIDTheme", project.droid_theme);
            saver->Write("start_scene", pTarget->start_scene);
            saver->Write("cur_ed_scene", pTarget->cur_ed_scene);
            saver->Write("cur_ed_file", pTarget->cur_ed_file);

            pTarget->Save(saver);

            saver->MarkEndBlock("project");

            saver->Release();
        }
    }

    void EditorProject::CopyDir(const char* dir, const char* local_dir, const char* out_dir)
    {
#ifdef PC
        char cur_dir[521];
        String::Printf(cur_dir, 512, "%s/%s/*.*", dir, local_dir);

        WIN32_FIND_DATA data;
        HANDLE h = FindFirstFile(cur_dir, &data);

        if( h!=INVALID_HANDLE_VALUE ) 
        {
            do
            {
                if (!String::IsEqual(data.cFileName, ".") && !String::IsEqual(data.cFileName, ".."))
                {
                    char sub_dir[521];
                    String::Printf(sub_dir, 512, "%s/%s/%s", dir, local_dir, data.cFileName);	

                    if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    {					
                        char sub_local_dir[521];
                        String::Printf(sub_local_dir, 512, "%s/%s", local_dir, data.cFileName);	

                        CopyDir(dir, sub_local_dir, out_dir);
                    }
                    else
                    {
                        char out_file[521];
                        String::Printf(out_file, 512, "%s/%s/%s", out_dir, local_dir, data.cFileName);

                        CopyFileEX(sub_dir, out_file);	
                    }
                }			
            }

            while(FindNextFile(h,&data));
        } 

        FindClose(h);
#endif

#ifdef OSUNIX

        char cur_dir[521];

        if (local_dir[0])
        {
            String::Printf(cur_dir, 512, "%s/%s", dir, local_dir);
        }
        else
        {
            String::Printf(cur_dir, 512, "%s", dir);		
        }

        struct dirent *de=NULL;
        DIR *d=NULL;

        d=opendir(cur_dir);

        if(d == NULL)
        {
            return;
        }

        while(de = readdir(d))
        {
            char sub_dir[521];

            if (local_dir[0])
            {
                String::Printf(sub_dir, 512, "%s/%s/%s", dir, local_dir, de->d_name);				
            }
            else
            {
                String::Printf(sub_dir, 512, "%s/%s", dir, de->d_name);	
            }						

            if (!String::IsEqual(".", de->d_name) && !String::IsEqual("..", de->d_name) && !String::IsEqual(".svn", de->d_name))
            {		
                if (de->d_type == DT_DIR)
                {					
                    char sub_local_dir[521];

                    if (local_dir[0])
                    {
                        String::Printf(sub_local_dir, 512, "%s/%s", local_dir, de->d_name);	
                    }
                    else
                    {
                        String::Printf(sub_local_dir, 512, "%s", de->d_name);	
                    }

                    CopyDir(dir, sub_local_dir, out_dir);
                }
                else
                {
                    char out_file[521];

                    if (local_dir[0])
                    {
                        String::Printf(out_file, 512, "%s/%s/%s", out_dir, local_dir, de->d_name);
                    }
                    else
                    {
                        String::Printf(out_file, 512, "%s/%s", out_dir, de->d_name);
                    }

                    CopyFileEX(sub_dir, out_file);	
                }
            }
        }

        closedir(d);

#endif
    }

    void EditorProject::SetProgressExportText( const char *format, ... )
    {
        char buf[1024];

        buf[0] = 0;

        va_list	args;

        va_start( args, format );
        vsnprintf( buf, sizeof( buf ), format, args );	
        va_end( args );

        if (progress_callback)
        {
            progress_callback->SetText(buf);	
        }
    }

    void EditorProject::CopyImage(const char* ApplicationDir, const char* platform_str, const char* exm_img_src, const char* img, int w, int h)
    {
        char command[2048];	

#ifdef PC
        sprintf(command, "\"\"%sBin\\nconvert.exe\" -out png -o \"%s/Export/%s/Pack/%s\" -resize %i %i -overwrite \"%s%s\"\"",
            ApplicationDir, ApplicationDir, platform_str, img, w, h, ApplicationDir, exm_img_src);	
#endif

#ifdef OSUNIX
        sprintf(command, "%s/Bin/nconvert -out png -o '%s/export/%s/pack/%s' -resize %i %i -overwrite '%s/%s'",
            ApplicationDir, ApplicationDir, platform_str, img, w, h, ApplicationDir, exm_img_src);		
#endif

        RunSilent(command, "");		
    }

    void EditorProject::MakeZip(const char* ApplicationDir, const char* platform_str, const char* out_name)
    {
        char zip_cmd[2048];		

#ifdef PC
        String::Printf(zip_cmd, 2048, "\"\"%s//Bin//7-Zip\\7z.exe\" -r a -tzip -y \"%s//Export//%s//%s\" \"%s//Export//%s//Pack/*\" \"", ApplicationDir, ApplicationDir, platform_str, out_name, ApplicationDir, platform_str);
#endif

#ifdef OSUNIX
        String::Printf(zip_cmd, 2048, "/usr/bin/7z a -r -tzip '%s/export/%s/%s' '%s/export/%s/pack/*'", ApplicationDir, platform_str, out_name, ApplicationDir, platform_str);			
#endif

        core->TraceTo("Export", zip_cmd);
        core->TraceTo("Export", "zip %i", RunSilent(zip_cmd, ""));
    }

    void EditorProject::SignIPA(const char* ApplicationDir, const char* platform_str, const char* out_name)
    {
        char zip_cmd[2048];		

		char ios_cert_name[256];
		String::Copy(ios_cert_name, 511, ReadINI("settings.ini", "", "iOSKey", "ios_cert_name"));

		char ios_cert_key[256];
		String::Copy(ios_cert_name, 511, ReadINI("settings.ini", "", "iOSKey", "ios_cert_key"));

		char ios_cert_keypass[256];
		String::Copy(ios_cert_keypass, 511, ReadINI("settings.ini", "", "iOSKey", "ios_cert_keypass"));

#ifdef PC	
        String::Printf(zip_cmd, 2048, "\"\"%s//Bin//codesign\\ldid.exe\" -k\"%s//Bin//cert//ios\" -S%s:AppleIncRootCertificate.cer:AppleWWDRCA.cer:%s:%s -C -E -I \"%s//Export//%s//Pack//%s\" \"", ApplicationDir, ApplicationDir, ios_cert_name, ios_cert_key, ios_cert_keypass, ApplicationDir, platform_str, out_name);
#endif

#ifdef OSUNIX
        String::Printf(zip_cmd, 2048, "%s/Bin/codesignunix/ldid -k'%s/Bin/cert/ios' -SAN_air.cer:AppleIncRootCertificate.cer:AppleWWDRCA.cer:AN_key.key:123 -C -E -I '%s/export/%s/pack/%s'", ApplicationDir, ApplicationDir, ApplicationDir, platform_str, out_name);
#endif

        core->TraceTo("Export", zip_cmd);
        core->TraceTo("Export", "sign ipa %i", RunSilent(zip_cmd, ""));
    }


    void EditorProject::SignAPK(const char* ApplicationDir, const char* platform_str,
        const char* droid_cert_name, const char* droid_cert_alias, const char* droid_cert_keypass, const char* droid_cert_storepass)							
    {
        char command[2048];	

#ifdef PC	
        HKEY key = 0;
        LSTATUS status = 0;
        if ((status = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\JavaSoft\\Java Development Kit", 0, KEY_WOW64_64KEY | KEY_QUERY_VALUE, &key)) != ERROR_SUCCESS)
        {
            if ((status = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Wow6432Node\\JavaSoft\\Java Development Kit", 0, KEY_WOW64_64KEY | KEY_QUERY_VALUE, &key)) != ERROR_SUCCESS)
            {
                core->TraceTo("Export", "JDK is not installed!");			

                return;
            }
        }

        char java_version[128] = { '\0' };
        char java_home[2048] = { '\0' };

        DWORD key_len = 0;
        DWORD key_type = 0;

        status = ::RegGetValue(key, "", "CurrentVersion", RRF_RT_ANY, &key_type, java_version, &key_len);
        if (status == ERROR_MORE_DATA)
        {
            status = ::RegGetValue(key, "", "CurrentVersion", RRF_RT_ANY, &key_type, java_version, &key_len);
        }

        if (status != ERROR_SUCCESS)
        {
            core->TraceTo("Export", "JDK cannot dedect version!");
            ::RegCloseKey(key);		

            return;
        }

        core->TraceTo("Export", "JDK version: %s", java_version);

        status = ::RegGetValue(key, java_version, "JavaHome", RRF_RT_ANY, &key_type, java_home, &key_len);
        if (status == ERROR_MORE_DATA)
        {
            status = ::RegGetValue(key, java_version, "JavaHome", RRF_RT_ANY, &key_type, java_home, &key_len);
        }

        if (status != ERROR_SUCCESS)
        {
            core->TraceTo("Export", "JDK cannot decect Home!");
            ::RegCloseKey(key);		

            return;
        }

        ::RegCloseKey(key);

        core->TraceTo("Export", "JDK home: %s", java_home);

        String::Printf(command, 2048, "\"\"%s/bin/jarsigner.exe\" -sigalg MD5withRSA -digestalg SHA1 -keystore \"%sBin/cert/android/%s\" \"%sExport//%s//app.apk\" %s %s %s\"", java_home, ApplicationDir, droid_cert_name, ApplicationDir, platform_str, droid_cert_alias, droid_cert_keypass, droid_cert_storepass);
#endif

#ifdef OSUNIX
        String::Printf(command, 2048, "jarsigner -sigalg MD5withRSA -digestalg SHA1 -keystore '%s/Bin/cert/android/%s' '%s/export/%s/app.apk' %s %s %s", ApplicationDir, droid_cert_name, ApplicationDir, platform_str, droid_cert_alias, droid_cert_keypass, droid_cert_storepass);
#endif

        core->TraceTo("Export", "apk %s", command);
        core->TraceTo("Export", "APK sign %i", RunSilent(command, ""));
    }


    void EditorProject::AlignAPK(const char* ApplicationDir, const char* platform_str)
    {
        char command[2048];	

#ifdef PC
        String::Printf(command, 2048, "\"\"%sBin\\zipalign.exe\" -v 4 \"%sExport//%s//app.apk\" \"%sExport//%s//app-release.apk\"\"", 
            ApplicationDir, ApplicationDir, platform_str, ApplicationDir, platform_str);			
#endif

#ifdef OSUNIX
        String::Printf(command, 2048, "%s/Bin/zipalign -v 4 '%s/export/%s/app.apk' '%s/export/%s/app-release.apk'", 
            ApplicationDir, ApplicationDir, platform_str, ApplicationDir, platform_str);			
#endif

        core->TraceTo("Export", "APK align %i", RunSilent(command, ""));	
    }

    bool EditorProject::ExportResources(Platform platform, const char* ApplicationDir, const char* OutDataPath, const char* project_name)
    {
        ProjectInfo project;

        OpenProject(project, project_name);

        en_core.EN_Files()->ClearPool("Target");
        en_core.EN_Files()->ActivatePool("Target");

        en_core.EN_Files()->RegFile("editor/system_declations.srt");
        en_core.EN_Files()->RegFile("Editor/fonts/helveticaneuecur");	
        en_core.EN_Scene()->RegisterSceneName("editor/system_declations.srt");

        //en_core.EN_Files()->RegFile("Loc/localize.loc");

        core->Script()->SetAllowExecuteScript(false);

        for (size_t i=0;i<project.CurTarget()->vScenes.size();i++)
        {
            en_core.EN_Scene()->RegisterSceneName(project.CurTarget()->vScenes[i]->Path());
        }

        for (size_t i=0;i<project.CurTarget()->vScenes.size();i++)
        {
            if (project.CurTarget()->vScenes[i]->IsScript()) continue;

            en_core.EN_Files()->RegFile(project.CurTarget()->vScenes[i]->Path());

            SetProgressExportText("Loading %s", project.CurTarget()->vScenes[i]->Name());

            project.CurTarget()->vScenes[i]->SetExportedScene(en_core.EN_Scene()->Load(project.CurTarget()->vScenes[i]->Path()));			

            char name[128];
            String::Copy(name, 128, project.CurTarget()->vScenes[i]->Name());
            name[strlen(name) - 3] = 0;

            project.CurTarget()->vScenes[i]->ExportedScene()->SetName(name);

            for (size_t j=0;j<project.CurTarget()->vScenes[i]->vIncludes.size();j++)
            {
                project.CurTarget()->vScenes[i]->ExportedScene()->IncludeScene(project.CurTarget()->vScenes[i]->vIncludes[j].c_str());
            }

            en_core.EN_Scene()->Work(0.1f, 0);

            while (project.CurTarget()->vScenes[i]->ExportedScene()->IsLoading())
            {
                en_core.EN_Scene()->Work(0.1f, 0);

                ThreadWorker::Sleep(100);
            }		
        }			

        SetProgressExportText("Starting scenes...");

        for (size_t i=0;i<project.CurTarget()->vScenes.size();i++)
        {		
            if (project.CurTarget()->vScenes[i]->IsScript()) continue;

            project.CurTarget()->vScenes[i]->ExportedScene()->PreStart();		
        }

        for (size_t i=0;i<project.CurTarget()->vScenes.size();i++)
        {		
            if (project.CurTarget()->vScenes[i]->IsScript()) continue;

            if (!project.CurTarget()->vScenes[i]->ExportedScene()->PostStart())
            {
                for (size_t j=0;j<i;j++)
                {
                    if (project.CurTarget()->vScenes[j]->IsScript()) continue;

                    core->Network()->Reset();

                    project.CurTarget()->vScenes[j]->ExportedScene()->Finish();
                }			

                core->TraceTo("Export", "Script contain errors");
                core->TraceTo("Export", "Export Failed");

                core->Script()->SetAllowExecuteScript(true);

                return false;
            }
        }

        en_core.EN_Files()->ActivatePool("");	

        //	records.clear();

        force_ios_bitmap = project.CurTarget()->force_ios_bitmap;
        force_ios_bitmap = 1;

        en_core.EN_Files()->SavePool("System", OutDataPath, platform);	
        en_core.EN_Files()->SavePool("Target", OutDataPath, platform);	

        SetProgressExportText("Save script...");

        for (size_t i=0;i<project.CurTarget()->vScenes.size();i++)
        {
            if (project.CurTarget()->vScenes[i]->IsScript()) continue;

            char scene_path[512];			
            String::Copy(scene_path, 512, OutDataPath);

            project.CurTarget()->vScenes[i]->LowerCasePath();

            String::Cat(scene_path, 512, project.CurTarget()->vScenes[i]->Path());
            //String::LowerCase(scene_path);

            CreateDirectories(scene_path);

            String::Copy(scene_path, 512, ApplicationDir);
            String::Cat(scene_path, 512, OutDataPath);
            String::Cat(scene_path, 512, project.CurTarget()->vScenes[i]->Path());
            //String::LowerCase(scene_path);		

            project.CurTarget()->vScenes[i]->ExportedScene()->Save(scene_path, IStreamSaver::binary);

            scene_path[strlen(scene_path) - 3] = 's';
            scene_path[strlen(scene_path) - 2] = 'r';
            scene_path[strlen(scene_path) - 1] = 't';

            if (core->Script()->ExportPrecompiledByteCode()) project.CurTarget()->vScenes[i]->ExportedScene()->SaveScriptByteCode(scene_path);				
        }

        SetProgressExportText("Clean scenes...");

        for (size_t i=0;i<project.CurTarget()->vScenes.size();i++)
        {
            if (project.CurTarget()->vScenes[i]->IsScript()) continue;

            core->Network()->Reset();

            project.CurTarget()->vScenes[i]->ExportedScene()->Finish();				

            project.CurTarget()->vScenes[i]->ExportedScene()->SetTaskHoldersOwnerActive(project.CurTarget()->cur_ed_scene == i);

            char name[128];
            String::Copy(name, 128, project.CurTarget()->vScenes[i]->Name());
            name[strlen(name) - 3] = 0;

            en_core.EN_Scene()->Unload(name);	
            en_core.EN_Scene()->Work(0.1f, 0);

            project.CurTarget()->vScenes[i]->SetExportedScene(null);		
        }

        SetProgressExportText("Copy byte code...");

        core->Script()->SetAllowExecuteScript(true);

        if (!core->Script()->ExportPrecompiledByteCode())
        {
            for (size_t i=0;i<project.CurTarget()->vScenes.size();i++)
            {
                char out_scene_path[512];			
                String::Copy(out_scene_path, 512, OutDataPath);
                String::Cat(out_scene_path, 512, project.CurTarget()->vScenes[i]->Path());
                String::LowerCase(out_scene_path);

                out_scene_path[strlen(out_scene_path) - 3] = 's';
                out_scene_path[strlen(out_scene_path) - 2] = 'r';
                out_scene_path[strlen(out_scene_path) - 1] = 't';

                char scene_path[512];
                String::Copy(scene_path, 512, project.CurTarget()->vScenes[i]->Path());	

                scene_path[strlen(scene_path) - 3] = 's';
                scene_path[strlen(scene_path) - 2] = 'r';
                scene_path[strlen(scene_path) - 1] = 't';

                CopyFileEX(scene_path, out_scene_path);	
            }
        }	

        en_core.EN_Scene()->UnregisterSceneNames();

        if (platform == platform_undefined)
        {		
            return true;
        }

        SetProgressExportText("Generating project...");

        char StartScene[256];

        String::Copy(StartScene,255,ApplicationDir);
        String::Cat(StartScene,255,"/");
        String::Cat(StartScene,255,OutDataPath);
        String::Cat(StartScene,255,"project");

        FILE* file = fopen(StartScene, "wb");

        char StartSceneName[256];	

        String::GetCropPath(ApplicationDir, project.CurTarget()->vScenes[project.CurTarget()->start_scene]->Path(), StartSceneName, 256);	

        int sz = project.CurTarget()->vScenes.size();
        fwrite(&sz, sizeof(sz), 1, file);	

        for (int i=0;i<sz;i++)
        {
            fwrite(project.CurTarget()->vScenes[i]->Path(), 256, 1, file);
        }	

        fwrite(&project.CurTarget()->debug_info_type, 1, 4, file);

        fwrite(&android_tex_format, 1, 4, file);

        if (platform == platform_android)
        {
            int prj_theme = 0;

            if (String::IsEqual(project.droid_theme, "Fullscreen"))
            {
                prj_theme = 0;
            }
            else
                if (String::IsEqual(project.droid_theme, "NoTitleBar"))
                {
                    prj_theme = 1;
                }
                else
                {
                    prj_theme = 2;
                }

                fwrite(&prj_theme, 1, 4, file);
        }
        else
            if (platform == platform_ios)
            {		
                int prj_theme = 0;

                if (String::IsEqual(project.droid_theme, "No Title bar"))
                {
                    prj_theme = 0;
                }
                else
                    if (String::IsEqual(project.droid_theme, "Title bar ios 6"))
                    {
                        prj_theme = 1;
                    }
                    else
                    {
                        prj_theme = 2;
                    }

                    fwrite(&prj_theme, 1, 4, file);
            }

            fwrite(&project.orintation, 1, 4, file);

            fwrite(&project.CurTarget()->ref_height, 1, 4, file);

            char name[64];
            String::Copy(name, 64, project.CurTarget()->vScenes[project.CurTarget()->start_scene]->Name());
            name[strlen(name) - 3] = 0;

            fwrite(name, 64, 1, file);

            fclose(file);

            return true;
    }

    void EditorProject::ExportResources(Platform platform, bool gen_package)
    {			
        RegisterExportFunc();

        if (project.CurTarget()->vScenes.size() == 0) return;	

        SetProgressExportText("Starting Export...");

        core->Script()->ReInit();

        char platform_str[128];

        switch (platform)
        {
        case platform_pc:
            String::Printf(platform_str, 128, "pc/%s", project.CurTarget()->ex_name);			
            break;
        case platform_ios:
            String::Printf(platform_str, 128, "ios/%s", project.CurTarget()->ex_name);
            break;
        case platform_android:
            String::Printf(platform_str, 128, "android/%s", project.CurTarget()->ex_name);
            break;
        case platform_osx:
            String::Printf(platform_str, 128, "osx/%s", project.CurTarget()->ex_name);			
            break;
        }

        String::LowerCase(platform_str);

        char ApplicationDir[512];

        GetCurDirectory(ApplicationDir, 512);			

        char OutDataPath[512];
        char cachePath[512];

        String::Copy(OutDataPath,511,ApplicationDir);
        String::Cat(OutDataPath,511,"/export/");	
        String::Cat(OutDataPath,511,platform_str);

        char droid_cert_name[256];
        String::Copy(droid_cert_name,511,ReadINI("settings.ini", "", "AndroidKey", "droid_cert_name"));

        char droid_cert_alias[256];
        String::Copy(droid_cert_alias,511,ReadINI("settings.ini", "", "AndroidKey", "droid_cert_alias"));

        String::Copy(cachePath,511,ReadINI("settings.ini", "", "AndroidKey", "droid_cert_keypass"));

        char droid_cert_keypass[256];

        if (cachePath[0])
        {
            String::Printf(droid_cert_keypass, 255, "-keypass %s", cachePath);
        }
        else
        {
            droid_cert_keypass[0] = 0;
        }

        String::Copy(cachePath,511,ReadINI("settings.ini", "", "AndroidKey", "droid_cert_storepass"));

        char droid_cert_storepass[256];

        if (cachePath[0])
        {
            String::Printf(droid_cert_storepass, 255, "-storepass %s", cachePath);
        }
        else
        {
            droid_cert_storepass[0] = 0;
        }

        char app_name[128];
        String::Copy(app_name,511,"/ion.app");

        if (gen_package)
        {
            if (platform == platform_android)
            {
                String::Cat(OutDataPath,511,"/pack/assets");
            }
            else
                if (platform == platform_ios)
                {
                    String::Cat(OutDataPath,511,"/pack/Payload");
                    String::Cat(OutDataPath,511,app_name);		
                }
        }
        else
        {
            String::Cat(OutDataPath,511,"/data");
        }

        DeleteDirectory(OutDataPath);	

        String::Copy(cachePath,511,ApplicationDir);
        String::Cat(cachePath,511,"/export/");	
        String::Cat(cachePath,511,platform_str);		
        String::Cat(cachePath,511,"/cache");

        CreateDirectories(cachePath);

        String::Cat(cachePath,511,"/cache");

        en_core.EN_Files()->LoadCached(cachePath);

        String::Copy(OutDataPath,511,"/export/");	
        String::Cat(OutDataPath,511,platform_str);	

        if (gen_package)
        {
            if (platform == platform_android)
            {
                String::Cat(OutDataPath,511,"/pack/assets/");
            }
            else
                if (platform == platform_ios)
                {
                    String::Cat(OutDataPath,511,"/pack/Payload");
                    String::Cat(OutDataPath,511,app_name);
                    String::Cat(OutDataPath,511,"/");			
                }
        }
        else
        {
            String::Cat(OutDataPath,511,"/Data/");
        }

        //String::LowerCase(OutDataPath);

        //en_core.SetTraceCallback(0);

        String::Cat(ApplicationDir,512,"/");

        if (!ExportResources(platform, ApplicationDir, OutDataPath, project_path))
        {
            return;
        }

        //	RegisterExportedFile("project", StartScene, "", platform);		

        en_core.EN_Files()->SaveCached(cachePath);

        if (gen_package)
        {		
            if (platform == platform_android)
            {
                SetProgressExportText("Generating apk...");

                char image_src[2048];		
                String::Printf(image_src, 2048, "Bin/images/droid");			

                char image_dest[2048];		
                String::Printf(image_dest, 2048, "export/%s/pack", platform_str);

                CopyDir(image_src, "", image_dest);		

                if (project.additional_permissions == 1)
                {
                    char image_src[2048];		
                    String::Printf(image_src, 2048, "Bin/images/AndroidManifest.xml", platform_str, ApplicationDir);			

                    char image_dest[2048];		
                    String::Printf(image_dest, 2048, "export/%s/pack/AndroidManifest.xml", platform_str, ApplicationDir);

                    CopyFileEX(image_src, image_dest);
                }

                String::LowerCase(exm_img_src);

                if (exp_params.image[0])
                {
                    exp_img_ready = false;

                    core->Network()->StartRequest(&exp_net_listiner, this, null, exp_params.image, "", "app_image");				

                    while(!exp_img_ready)
                    {
                        ThreadWorker::Sleep(100);
                        ((NetworkService*)en_core.Network())->Work(0, 0);
                    }
                }

                char command[2048];					

                sprintf(command, "res/drawable-ldpi-v4/ic_launcher.png", app_name);			
                CopyImage(ApplicationDir, platform_str, exm_img_src, command, 36, 36);

                sprintf(command, "res/drawable-mdpi-v4/ic_launcher.png", app_name);			
                CopyImage(ApplicationDir, platform_str, exm_img_src, command, 48, 48);

                sprintf(command, "res/drawable-hdpi-v4/ic_launcher.png", app_name);			
                CopyImage(ApplicationDir, platform_str, exm_img_src, command, 72, 72);

                sprintf(command, "res/drawable-xhdpi-v4/ic_launcher.png", app_name);			
                CopyImage(ApplicationDir, platform_str, exm_img_src, command, 96, 96);

                sprintf(command, "res/drawable-xxhdpi-v4/ic_launcher.png", app_name);			
                CopyImage(ApplicationDir, platform_str, exm_img_src, command, 144, 144);

                sprintf(command, "res/drawable-xxxhdpi-v4/ic_launcher.png", app_name);			
                CopyImage(ApplicationDir, platform_str, exm_img_src, command, 192, 192);

                String::Printf(command, 2048, "%s/export/%s/app.apk", ApplicationDir, platform_str);
                DeleteFile(command);

                char file_nm[2048];
                String::Printf(file_nm, 2048, "export/%s/pack/resources.arsc", platform_str);					

                binary_xml.Open(file_nm);

                binary_xml.Modify("AppNow", exp_params.name);
                binary_xml.Modify("ION ENgine", exp_params.name);

                if (exp_params.droid_google_anlt[0]) binary_xml.Modify("UA-XXXX-Y", exp_params.droid_google_anlt);

                //binary_xml.ModifyPackageName("com.ion.packagename", exp_params.package_name);

                binary_xml.Write(file_nm);

                //TraceXML(file_nm);

                String::Printf(file_nm, 2048, "%s/export/%s/pack/AndroidManifest.xml", ApplicationDir, platform_str);			

                binary_xml.Open(file_nm);

                binary_xml.ModifyVersionCode(atoi(exp_params.package_code_name));
                binary_xml.Modify("1.0.0", exp_params.package_version_name);

                char str[128];

                binary_xml.Modify("com.ion.packagename", exp_params.package_name);

                sprintf(str, "%s.MESSAGE", exp_params.package_name);
                binary_xml.Modify("com.ion.packagename.MESSAGE", str);

                sprintf(str, "%s.permission.C2D_MESSAGE", exp_params.package_name);
                binary_xml.Modify("com.ion.packagename.permission.C2D_MESSAGE", str);

                sprintf(str, "val:%s", exp_params.fb_id);
                binary_xml.Modify("val:1395007780739839", str);

                sprintf(str, "val:%s", exp_params.vk_id);
                binary_xml.Modify("val:3673620", str);

                sprintf(str, "val:%s", exp_params.droid_flurry);
                binary_xml.Modify("val:flurry_app_id", str);

                for (int i = exp_params.modules.size(); --i >= 0;)
                {
                    char act_name[256];

                    Module* module = exp_params.modules[i];

                    if (String::IsEqual(module->GetName(), "info"))
                    {
                        ModuleInfo* module_info = (ModuleInfo*)module;

                        sprintf(str, "val:%s", module_info->app_id);
                        binary_xml.Modify("val:448", str);

                        sprintf(str, "val:%s", module_info->app_secret);
                        binary_xml.Modify("val:b3a55dd742780b2fbb03b667ce8d37bc", str);
                    }					
                }

                binary_xml.Write(file_nm);

                MakeZip(ApplicationDir, platform_str, "app.apk");

                SignAPK(ApplicationDir, platform_str, droid_cert_name, droid_cert_alias, droid_cert_keypass, droid_cert_storepass);			

                String::Printf(command, 2048, "%s/export/%s/app-release.apk", ApplicationDir, platform_str);
                DeleteFile(command);

                AlignAPK(ApplicationDir, platform_str);			

                String::Printf(command, 2048, "%s/export/%s/app.apk", ApplicationDir, platform_str);
                DeleteFile(command);

                DeleteDirectory(image_dest);

				String::Printf(command, 2048, "%s/export/%s/", ApplicationDir, platform_str);
				ShellExecute(NULL, "open", command, NULL, NULL, SW_SHOWDEFAULT);
            }
            else
                if (platform == platform_ios)
                {
                    SetProgressExportText("Generating ipa...");

                    char image_src[2048];		
                    String::Printf(image_src, 2048, "Bin/images/ios");		

                    char image_dest[2048];		
                    String::Printf(image_dest, 2048, "export/%s/pack", platform_str);

                    CopyDir(image_src, "", image_dest);

                    char plist_src[2048];
                    String::Printf(plist_src, 2048, "%s/Bin/images/Info.plist", ApplicationDir);				

                    char plist_dest[2048];
                    String::Printf(plist_dest, 2048, "%s/Bin/images/Info2.plist", ApplicationDir);				

                    tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();

                    if (!doc->LoadFile(plist_src))
                    {
                        const char* orinet_str1 = "UIInterfaceOrientationPortrait";
                        const char* orinet_str2 = "UIInterfaceOrientationPortrait";
                        const char* orinet_str3 = "UIInterfaceOrientationPortrait";

                        if (project.orintation == 0)
                        {
                            orinet_str1 = "UIInterfaceOrientationLandscapeLeft";
                            orinet_str2 = "UIInterfaceOrientationLandscapeLeft";
                            orinet_str3 = "UIInterfaceOrientationLandscapeLeft";
                        }				

                        {
                            tinyxml2::XMLElement* nd = doc->FirstChildElement("plist");
                            tinyxml2::XMLElement* nd2 = nd->FirstChildElement("dict");

                            tinyxml2::XMLElement* key_nd = nd2->FirstChildElement("key");

                            while (!String::IsEqual(key_nd->GetText(), "UISupportedInterfaceOrientations"))
                            {
                                key_nd = key_nd->NextSiblingElement();
                            }

                            key_nd = key_nd->NextSiblingElement();

                            tinyxml2::XMLElement* key_nd_array = key_nd->FirstChildElement("string");
                            key_nd_array->FirstChild()->SetValue(orinet_str2);

                            //key_nd_array = key_nd_array->NextSiblingElement();
                            //key_nd_array->FirstChild()->SetValue(orinet_str3);
                        }

                        {
                            tinyxml2::XMLElement* nd = doc->FirstChildElement("plist");
                            tinyxml2::XMLElement* nd2 = nd->FirstChildElement("dict");

                            tinyxml2::XMLElement* key_nd = nd2->FirstChildElement("key");

                            while (!String::IsEqual(key_nd->GetText(), "UISupportedInterfaceOrientations~ipad"))
                            {
                                key_nd = key_nd->NextSiblingElement();
                            }

                            key_nd = key_nd->NextSiblingElement();

                            tinyxml2::XMLElement* key_nd_array = key_nd->FirstChildElement("string");
                            key_nd_array->FirstChild()->SetValue(orinet_str2);

                            //key_nd_array = key_nd_array->NextSiblingElement();
                            //key_nd_array->FirstChild()->SetValue(orinet_str3);
                        }

                        {
                            tinyxml2::XMLElement* nd = doc->FirstChildElement("plist");
                            tinyxml2::XMLElement* nd2 = nd->FirstChildElement("dict");

                            tinyxml2::XMLElement* key_nd = nd2->FirstChildElement("key");

                            while (!String::IsEqual(key_nd->GetText(), "UIInterfaceOrientation"))
                            {
                                key_nd = key_nd->NextSiblingElement();
                            }

                            key_nd = key_nd->NextSiblingElement();
                            key_nd->FirstChild()->SetValue(orinet_str2);
                        }

                        {
                            tinyxml2::XMLElement* nd = doc->FirstChildElement("plist");
                            tinyxml2::XMLElement* nd2 = nd->FirstChildElement("dict");

                            tinyxml2::XMLElement* key_nd = nd2->FirstChildElement("key");

                            while (!String::IsEqual(key_nd->GetText(), "CFBundleDisplayName"))
                            {
                                key_nd = key_nd->NextSiblingElement();
                            }

                            key_nd = key_nd->NextSiblingElement();
                            key_nd->FirstChild()->SetValue(exp_params.name);
                        }

                        {
                            tinyxml2::XMLElement* nd = doc->FirstChildElement("plist");
                            tinyxml2::XMLElement* nd2 = nd->FirstChildElement("dict");

                            tinyxml2::XMLElement* key_nd = nd2->FirstChildElement("key");

                            while (!String::IsEqual(key_nd->GetText(), "UIStatusBarHidden"))
                            {
                                key_nd = key_nd->NextSiblingElement();
                            }

                            key_nd = key_nd->NextSiblingElement();

                            if (!String::IsEqual(project.ios_theme, "No Title bar"))					
                            {
                                key_nd->SetName("false");
                            }					
                        }

                        {
                            tinyxml2::XMLElement* nd = doc->FirstChildElement("plist");
                            tinyxml2::XMLElement* nd2 = nd->FirstChildElement("dict");

                            tinyxml2::XMLElement* key_nd = nd2->FirstChildElement("key");

                            while (!String::IsEqual(key_nd->GetText(), "UIStatusBarHidden~ipad"))
                            {
                                key_nd = key_nd->NextSiblingElement();
                            }

                            key_nd = key_nd->NextSiblingElement();

                            if (!String::IsEqual(project.ios_theme, "No Title bar"))					
                            {
                                key_nd->SetName("false");
                            }					
                        }

                        {
                            tinyxml2::XMLElement* nd = doc->FirstChildElement("plist");
                            tinyxml2::XMLElement* nd2 = nd->FirstChildElement("dict");

                            tinyxml2::XMLElement* key_nd = nd2->FirstChildElement("key");

                            while (!String::IsEqual(key_nd->GetText(), "UIStatusBarStyle"))
                            {
                                key_nd = key_nd->NextSiblingElement();
                            }

                            key_nd = key_nd->NextSiblingElement();

                            if (project.status_bar_is_white == 0)					
                            {
                                key_nd->FirstChild()->SetValue("UIStatusBarStyleDefault");
                            }					
                        }

                        {
                            tinyxml2::XMLElement* nd = doc->FirstChildElement("plist");
                            tinyxml2::XMLElement* nd2 = nd->FirstChildElement("dict");

                            for (int i = exp_params.modules.size(); --i >= 0;)
                            {
                                char act_name[256];

                                Module* module = exp_params.modules[i];

                                core->TraceTo("Export", "Module: %s", module->GetName());

                                String::Printf(act_name, 256, "ModuleData_%s", module->GetName());

                                tinyxml2::XMLElement* module_node = nd2->InsertEndChild(doc->NewElement("key"))->ToElement();
                                module_node->InsertEndChild(doc->NewText(act_name));

                                module_node = nd2->InsertEndChild(doc->NewElement("dict"))->ToElement();

                                module->exportParams(module_node, platform);					
                            }
                        }

                        doc->SaveFile(plist_dest);
                    }			

                    ReplaceKey(plist_dest, "bundlename", exp_params.bundle_name);
                    ReplaceKey(plist_dest, "bundleshortname", exp_params.bundle_short_name);
                    ReplaceKey(plist_dest, "bundle_version", exp_params.bundle_version);
                    ReplaceKey(plist_dest, "fbappid", exp_params.fb_id);			

                    ReplaceKey(plist_dest, "Flurry_app_id", exp_params.ios_flurry);			
                    ReplaceKey(plist_dest, "GA_app_id", exp_params.ios_google_anlt);			

                    if (exp_params.fb_id_url[0])
                    {
                        ReplaceKey(plist_dest, "fbidurl", exp_params.fb_id_url);			
                    }
                    else
                    {
                        ReplaceKey(plist_dest, "fbidurl", "fb812384188857172");			
                    }


                    if (exp_params.fbappidsuffix[0])
                    {
                        ReplaceKey(plist_dest, "fbsuffix", exp_params.fbappidsuffix);			
                    }
                    else
                    {
                        ReplaceKey(plist_dest, "fbsuffix", "fa3623");							
                    }

                    delete doc;

                    char command[2048];				

                    String::Printf(command, 2048, "export/%s/pack/Payload/%s/Info.plist", platform_str, app_name);			
                    CopyFileEX("Bin/images/Info2.plist", command);			

                    if (project.hack_ios_ext[0])
                    {
                        String::Cat(OutDataPath,511,"/PlugIns/Ext2.appex/");

                        if (!ExportResources(platform, ApplicationDir, OutDataPath, project.hack_ios_ext))
                        {
                            return;
                        }

                        char image_src[2048];
                        String::Printf(image_src, 2048, "%s/Bin/images/ios_ext_today", ApplicationDir);			

                        char image_dest[2048];		
                        String::Printf(image_dest, 2048, "%s/export/%s/pack/Payload/%s", ApplicationDir, platform_str, app_name);

                        CopyDir(image_src, "", image_dest);

                        String::Printf(plist_src, 2048, "%sBin/images/Info_ext.plist", ApplicationDir);		
                        String::Printf(plist_dest, 2048, "%sBin/images/Info2.plist", ApplicationDir);

                        CopyFileEX(plist_src, plist_dest);

                        {
                            char bundle_name[128];
                            String::Printf(bundle_name, 128, "%s.Ext2", exp_params.bundle_name);

                            ReplaceKey(plist_dest, "bundlename", bundle_name);
                        }

                        ReplaceKey(plist_dest, "bundledisplayanme", exp_params.name);

                        char plist_cmd[512];
                        String::Printf(plist_cmd, 2048, "%s/export/%s/pack/Payload/%s/PlugIns/Ext2.appex/Info.plist", ApplicationDir, platform_str, app_name);

                        CopyFileEX(plist_dest, plist_cmd);	

                        SignIPA(ApplicationDir, platform_str, "Payload/ion.app/PlugIns/Ext2.appex/Ext2");				
                    }

                    if (exp_params.image[0])
                    {
                        exp_img_ready = false;

                        core->Network()->StartRequest(&exp_net_listiner, this, null, exp_params.image, "", "app_image");

                        while(!exp_img_ready)
                        {
                            ThreadWorker::Sleep(100);
                            ((NetworkService*)en_core.Network())->Work(0, 0);
                        }
                    }
                    else
                    {
                        String::LowerCase(exm_img_src);
                    }

                    char img[256];

                    sprintf(img, "Payload/%s/icon-72@2x.png", app_name);			
                    CopyImage(ApplicationDir, platform_str, exm_img_src, img, 144, 144);

                    sprintf(img, "Payload/%s/icon@2x.png", app_name);			
                    CopyImage(ApplicationDir, platform_str, exm_img_src, img, 114, 114);

                    sprintf(img, "Payload/%s/icon-72.png", app_name);
                    CopyImage(ApplicationDir, platform_str, exm_img_src, img, 72, 72);

                    sprintf(img, "Payload/%s/icon.png", app_name);
                    CopyImage(ApplicationDir, platform_str, exm_img_src, img, 57, 57);

                    sprintf(img, "Payload/%s/icon-60.png", app_name);
                    CopyImage(ApplicationDir, platform_str, exm_img_src, img, 60, 60);

                    sprintf(img, "Payload/%s/icon-60@2x.png", app_name);
                    CopyImage(ApplicationDir, platform_str, exm_img_src, img, 120, 120);

                    sprintf(img, "Payload/%s/icon-76.png", app_name);
                    CopyImage(ApplicationDir, platform_str, exm_img_src, img, 76, 76);

                    sprintf(img, "Payload/%s/icon-76@2x.png", app_name);
                    CopyImage(ApplicationDir, platform_str, exm_img_src, img, 152, 152);									

                    char zip_cmd[2048];		
                    String::Printf(zip_cmd, 2048, "%s/export/%s/app.ipa", platform_str, ApplicationDir);
                    DeleteFile(zip_cmd);			

                    SignIPA(ApplicationDir, platform_str, "Payload/ion.app/ion");

                    MakeZip(ApplicationDir, platform_str, "app.ipa");

                    DeleteDirectory(image_dest);

					String::Printf(command, 2048, "%s/export/%s/", ApplicationDir, platform_str);
					ShellExecute(NULL, "open", command, NULL, NULL, SW_SHOWDEFAULT);
                }
        }	

        core->TraceTo("Export", "Export Finished");
    }

    void EditorProject::ReadModulesParams(Platform cur_platform, IDataFile* data, EditorProject::ExportParams* params)
    {
        while (data->EnterBlock("modules"))
        {
            char name[256];

            data->ReadString("name", name, 256);

            if (String::Compare("info", name) == 0)
            {
                EditorProject::ModuleInfo* module = new EditorProject::ModuleInfo;
                params->AddModule(module);			

                if (data->EnterBlock("cfg"))
                {
                    data->ReadString("app_id", module->app_id, 256);
                    data->ReadString("app_secret", module->app_secret, 256);

                    data->LeaveBlock();
                }
            }

            data->LeaveBlock();
        }
    }

    void EditorProject::PrepereExpParametrs(Platform platform)
    {
        exp_params.RemoveModules();

        String::Copy(exp_params.name, 256, project.CurTarget()->name);
        String::Copy(exm_img_src, 256, project.CurTarget()->icon_path);

        String::Copy(exp_params.package_name, 256, project.package_name);
        String::Copy(exp_params.package_version_name, 256, project.package_version_name);
        String::Copy(exp_params.package_code_name, 256, project.package_code_name);		

        String::Copy(exp_params.bundle_name, 256, project.bundle_name);
        String::Copy(exp_params.bundle_short_name, 256, project.bundle_short_name);
        String::Copy(exp_params.bundle_version, 256, project.bundle_version);	

        String::Copy(exp_params.fb_id, 256, project.fb_id);
        String::Copy(exp_params.vk_id, 256, project.vk_id);	
    }

    void EditorProject::ReplaceKey(const char* file_name, const char* key, const char* value)
    {
        //very, very lame but it works

        IFileBuffer* buffer = core->Files()->LoadFile(file_name);	

        if (buffer)
        {
            int key_len = strlen(key);
            int val_len = strlen(value);

            int readed_buffer_len = buffer->GetSize();
            byte* readed_buffer = (byte*)malloc(readed_buffer_len);

            memcpy(readed_buffer, buffer->GetBuffer(), buffer->GetSize());

            buffer->Release();

            for (int i=0;i<readed_buffer_len - key_len;i++)
            {
                bool finded = true;

                for (int j=0;j<key_len;j++)
                {
                    if (readed_buffer[i + j] != key[j])
                    {
                        finded = false;
                        break;
                    }
                }

                if (finded)
                {
                    byte* tmp_buffer = (byte*)malloc(readed_buffer_len + val_len - key_len);

                    if (i!=0)
                    {
                        memcpy(tmp_buffer, readed_buffer, i);
                    }

                    if (val_len > 0)
                    {
                        memcpy(&tmp_buffer[i], value, val_len);
                    }

                    memcpy(&tmp_buffer[i+val_len], &readed_buffer[i+key_len], readed_buffer_len - i - key_len);

                    readed_buffer_len = readed_buffer_len + val_len - key_len;

                    free(readed_buffer);

                    readed_buffer = tmp_buffer;
                }
            }

            IFile* fl = core->Files()->OpenFile(file_name, IFile::write);	

            if (fl)
            {
                fl->Write(readed_buffer, readed_buffer_len);
                fl->Release();
            }

            free(readed_buffer);
        }
    }

    bool EditorProject::Export(const char* exp_name)
    {	
        core->TraceTo("Export", "parse exp_name");

        ResetProject();

        en_core.Render()->SetUseDummyRender(true);

        core->TraceTo("Export", "start core");

        en_core.EN_Files()->AddPool("System");
        en_core.EN_Files()->AddPool("Target");

        en_core.EN_Files()->ActivatePool("System");

        if (!core->Init()) return 0;

        en_core.EN_Files()->ActivatePool("");

        RegisterExportFunc();	

        core->TraceTo("Export", "load loader");

        en_core.SetEditorMode(true);	
        core->Scene()->SetSceneLoadType(IStreamLoader::xml);

        core->TraceTo("Export", "open json");

        IDataFile* data = core->Files()->OpenJSONFile(exp_name);

        if (!data) return 0;

        char platform[256];	

        data->ReadString("src", project_path, 256);
        data->ReadString("platform", platform, 256);
        data->ReadString("name", exp_params.name, 256);			

        data->ReadString("fb_id", exp_params.fb_id, 256);
        data->ReadString("vk_id", exp_params.vk_id, 256);

        data->ReadString("ga_ios", exp_params.ios_google_anlt, 256);
        data->ReadString("ga_android", exp_params.droid_google_anlt, 256);
        data->ReadString("flurry_ios", exp_params.ios_flurry, 256);
        data->ReadString("flurry_android", exp_params.droid_flurry, 256);

        data->ReadString("fb_id_url", exp_params.fb_id_url, 256);
        data->ReadString("fbappidsuffix", exp_params.fbappidsuffix, 256);


        //data->ReadString("image", exp_params.image, 1024);

        Platform cur_platform = platform_undefined;

        if (String::IsEqual(platform, "ios"))
        {
            android_tex_format = 0;
            cur_platform = platform_ios;

            data->ReadString("bundle_id", exp_params.bundle_name, 256);
            data->ReadString("bundle_short_name", exp_params.bundle_short_name, 256);
            data->ReadString("bundle_version", exp_params.bundle_version, 256);
        }
        else
            if (String::IsEqual(platform, "android"))
            {
                android_tex_format = 4;
                cur_platform = platform_android;

                data->ReadString("package_name", exp_params.package_name, 256);
                data->ReadString("package_version_name", exp_params.package_version_name, 256);
                data->ReadString("package_code_name", exp_params.package_code_name, 256);
            }

            ReadModulesParams(cur_platform, data, &exp_params);

            data->Release();

            core->TraceTo("Export", "open project");

            OpenProject(project, project_path);

            String::Copy(exm_img_src, 256, project.CurTarget()->icon_path);	

            core->TraceTo("Export", "ExportResources");

            //temporary
            /*String::Copy(exp_params.droid_google_anlt, 256, project.droid_google_anlt);
            String::Copy(exp_params.ios_google_anlt, 256, project.ios_google_anlt);
            String::Copy(exp_params.droid_flurry, 256, project.droid_flurry);
            String::Copy(exp_params.ios_flurry, 256, project.ios_flurry);*/	

            ExportResources(cur_platform, true);

            return 0;
    }

    void EditorProject::Module::setKey(tinyxml2::XMLElement* root, Platform platform, const char* key, const char* value)
    {
        if (platform == platform_android)
        {
            tinyxml2::XMLElement* meta = root->InsertEndChild(root->GetDocument()->NewElement("meta-data"))->ToElement();
            meta->SetAttribute("android:name", key);

            char buffer[256];
            String::Printf(buffer, 256, "val:%s", value);

            meta->SetAttribute("android:value", buffer);
        }
        else if (platform == platform_ios)
        {
            tinyxml2::XMLElement* node = root->InsertEndChild(root->GetDocument()->NewElement("key"))->ToElement();
            node->InsertEndChild(root->GetDocument()->NewText(key));

            node = root->InsertEndChild(root->GetDocument()->NewElement("string"))->ToElement();
            node->InsertEndChild(root->GetDocument()->NewText(value));
        }
    }


    void Target::SetStartScene(Scene* pScene)
    {
        int nOldStartScene = start_scene;
        if (!pScene->IsScript())
        {
            start_scene = pScene->Index();
            rTree.SetStartImage(pScene->Handle(),true);
            if(nOldStartScene != -1)
            {
                rTree.SetStartImage(vScenes[nOldStartScene]->Handle(),false);
            }
        }
    }

    void Target::LoadProjectInfo(const char* sProjectName,IStreamLoader::FileType data_type)
    {
        IStreamLoader* loader = core->Files()->CreateStreamLoader(sProjectName, "project",data_type);	
        if (loader)
        {
            if (!loader->EnterBlock("Target"))
            {
                while (loader->EnterBlock("Scene"))
                {			
                    bool bFlag;
                    loader->Read("OnlySrt", bFlag);
                    char sName[128],sPath[512];
                    loader->Read("Name",sName,128);	
                    ProjectInfo::Scene* pScene = NULL;
                    ProjectInfo::Scene** ppScene = vScenesMap.Find(sName);
                    if(ppScene)
                        pScene = *ppScene;
                    else
                    {
                        pScene = new ProjectInfo::Scene(sName);
                        vScenesMap[sName] = pScene;
                        while (loader->EnterBlock("IncludedScene"))
                        {										
                            loader->Read("Path",sPath, 256);
                            pScene->vIncludes.push_back(sPath);
                            loader->LeaveBlock();
                        }
                    }
                    loader->Read("Path", sPath, 512);
                    if(!bFlag)
                        pScene->Load(sPath,data_type);
                    else
                        pScene->SetPath(sPath);
                    loader->LeaveBlock();
                }	
            }
            loader->Release();
        }
    }

    void Target::UpdateEngineTopEntities()
    {
        for(unsigned i = 0;i < vScenes.size();++i)
        {
            Scene* pScene  = vScenes[i];
            if(pScene->GetEngineScene())
                pScene->UpdateEngineTopEntities();
        }
    }

    void Target::Clear()
    {
        for(unsigned i = 0;i < vScenes.size();++i)
        {
            if (vScenes[i]->Instance())
            {
                en_core.EN_Scene()->Unload(vScenes[i]->Name());
            }
            delete vScenes[i];
        }
        vScenes.clear();
    }

    void Target::DeleteScene(int nIndex)
    {
        if (nIndex != -1)
        {
            if(rTree.View())
                rTree.View()->DeleteItem(vScenes[nIndex]->Handle());
            vScenesMap.Delete(vScenes[nIndex]->Name());
            delete vScenes[nIndex];
            vScenes.erase(vScenes.begin() + nIndex);		
            for(unsigned i = nIndex;i < vScenes.size();++i)
                vScenes[i]->SetIndex(vScenes[i]->Index() - 1);
        }
    }

    void Target::SelectScene(int index)
    {
        if(index != -1 && index < vScenes.size())
        {
            if(vScenes[index]->IsScript())
              cur_ed_file = index;
            else
              cur_ed_scene = index;
            if(rTree.View())
                rTree.View()->SelectItem(vScenes[index]->Handle());
        }
        else
            if(rTree.View())
                rTree.View()->SelectItem(NULL);
    }

    void Target::Save()
    {
        rTree.GrabTree();
        for (size_t i=0; i<vScenes.size(); i++)
        {
            ProjectInfo::Scene* pScene = vScenes[i];
            if (pScene->Instance())
                pScene->Instance()->Save(pScene->Path(), IStreamSaver::xml);	
            if (!pScene->IsScript())
                pScene->Save();
        }
    }

    void Target::Save(IStreamSaver* pSaver)
    {
        for (size_t  i=0;i<vScenes.size();i++)
        {			
            pSaver->MarkBeginBlock("Scene");
            vScenes[i]->Save(pSaver);
            pSaver->MarkEndBlock("Scene");
        }	
        rTree.Save(pSaver);
    }

    Scene* Target::AddScene(const string& sName,bool bScript,void* pParent)
    {
        vScenes.push_back(new ProjectInfo::Scene(sName,vScenes.size(),bScript));
        ProjectInfo::Scene* pScene = vScenes.back();
        vScenesMap[sName] = pScene;
		if (start_scene == -1 && !bScript)
		{
			start_scene = 0;
			rTree.AddItem(pScene, pParent,true);
		}
		else
          rTree.AddItem(pScene,pParent);
        return pScene;
    }

    void Target::Load(IStreamLoader* pLoader)
    {
        while (pLoader->EnterBlock("Scene"))
        {			
            vScenes.push_back(new Scene(vScenes.size()));
            Scene* scene_src = vScenes.back();
            scene_src->Load(pLoader);
            vScenesMap[scene_src->Name()] = scene_src;
            pLoader->LeaveBlock();
        }	
        rTree.Load(pLoader);
    }

    void Target::Rename(Scene* pScene,const char* sNewName)
    {
        string sPath = pScene->Path();
        int i = sPath.size() - 1;
        for(;i >= 0;--i)
        {
            if(sPath[i] =='/')
                break;
        }
        if(i >= 0)
        {
            sPath.erase(i + 1,sPath.size() - i - 1);
            sPath += sNewName;
            if(pScene->IsScript())
                sPath += ".srt";
            else
                sPath += ".isx";
            vScenesMap.Delete(pScene->Name());
            rTree.Rename(pScene->Handle(),sNewName);
            string sName = string(sNewName) + "_ed";
            pScene->SetName(sName.c_str());
            pScene->SetPath(sPath.c_str());
            vScenesMap[pScene->Name()] = pScene;
        }

    }

    Scene* Target::Find(const char* sName)
    {
        string sBuff = sName;
        sBuff += "_ed";
        Scene** ppScene  = vScenesMap.Find(sBuff);
        if(ppScene)
            return *ppScene;
        return NULL;
    }
}