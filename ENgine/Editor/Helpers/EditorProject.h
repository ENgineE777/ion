
#pragma once

#include "Core/ICore.h"
#include "Core/Scene/SceneInstance.h"
#include "Core/Files/FileService.h"
#include "Editor/Helpers/AndroidXML.h"
#include "Editor/Helpers/SceneInfo.h"
#include "ProjectTree.h"
namespace tinyxml2
{
    class XMLElement;
}

namespace ProjectInfo
{

    enum TargetType
    {
        Application,
        IOS_Extention,
        Android_Widget
    };

    struct Target
    {
        TargetType type;


        char ex_name[128];
        char name[128];
        char icon_path[256];
        int start_scene;
        int cur_ed_scene;		
        int cur_ed_file;		

        ProjectTree                  rTree; 
        TScenes                      vScenesMap; 
        std::vector<Scene*>          vScenes;

        int  debug_info_type;
        int  force_ios_bitmap;
		int  ref_width;
        int  ref_height;

        Target():rTree(this)
        {
            name[0] = 0;
            cur_ed_scene = -1;
            start_scene = -1;

            String::Copy(icon_path, 256, "Editor/ion.bmp");

            force_ios_bitmap = 0;
            debug_info_type = 0;

			ref_width = 1024;
            ref_height = 768;				
        };
        ~Target()
        {
            for(unsigned i = 0;i < vScenes.size();++i)
                delete vScenes[i];
            vScenes.clear();
            vScenesMap.Clear();
        }
        inline void  SetTreeView(ISceneTreeView* pView){rTree.SetView(pView);}

        void DeleteScene(int nIndex);
        void Clear();
        void SetStartScene(Scene* pScene);
        void Rename(Scene* pScene,const char* sNewName);
        void LoadProjectInfo(const char* sProjectName,IStreamLoader::FileType data_type);
        void UpdateEngineTopEntities();
        void SelectScene(int index);
        void Save();
        void Save(IStreamSaver* pSaver);
        void Load(IStreamLoader* pLoader);

        Scene* AddScene(const string& sName,bool bScript,void* pParent = NULL);
        Scene* Find(const char* sName);
    };


    class EditorProject
    {
    public:		

        struct ProjectInfo
        {					
            int  orintation;
            char ios_theme[128];
            int  status_bar_is_white;
            char droid_theme[128];						

            int  additional_permissions;		

            char package_name[256];
            char package_version_name[256];
            char package_code_name[256];

            char bundle_name[256];
            char bundle_short_name[256];
            char bundle_version[256];

            char fb_id[256];
            char vk_id[256];

            int  sel_target;

            char hack_ios_ext[256];

            int  need_isi;

            ProjectInfo()
            {
                sel_target = -1;

                orintation = 0;
                String::Copy(ios_theme, 128, "No Title bar");
                status_bar_is_white = 1;
                String::Copy(droid_theme, 128, "Fullscreen");

                String::Copy(package_name, 256, "com.ion");
                String::Copy(package_version_name, 256, "1.0");
                String::Copy(package_code_name, 256, "1");

                String::Copy(bundle_name, 256, "com.ion");
                String::Copy(bundle_short_name, 256, "ion");	
                String::Copy(bundle_version, 256, "1.0");	

                fb_id[0] = 0;
                vk_id[0] = 0;			

                additional_permissions = 0;

                hack_ios_ext[0] = 0;
                need_isi = 1;
            };

            std::vector<Target> targets;

            Target* CurTarget()
            {
                if (sel_target == -1)
                {
                    return null;
                }

                return &targets[sel_target];
            }

            Scene* CurScene()
            {
                if (sel_target == -1)
                {
                    return null;
                }

                Target* target = &targets[sel_target];

                if (target->cur_ed_scene == -1)
                {
                    return null;
                }

                return target->vScenes[target->cur_ed_scene];
            };

        };

        char project_path[512];
        ProjectInfo project;

        class Module
        {		
        public:

            virtual void exportParams(tinyxml2::XMLElement* root, Platform platform) = 0;

            virtual const char* GetName() = 0;

        protected:
            void setKey(tinyxml2::XMLElement* root, Platform platform, const char* key, const char* value);
        };

        class ModuleInfo : public Module
        {
        public:
            virtual void exportParams(tinyxml2::XMLElement* root, Platform platform)
            {
                setKey(root, platform, "app_id", app_id);
                setKey(root, platform, "app_secret", app_secret);
            }

            virtual const char* GetName()
            {
                return "info";
            }

            char app_id[256];
            char app_secret[256];
        };

        struct ExportParams
        {
            char name[256];
            char image[1024];

            char package_name[256];
            char package_version_name[256];
            char package_code_name[256];

            char bundle_name[256];
            char bundle_short_name[256];
            char bundle_version[256];

            char fb_id[256];
            char vk_id[256];		

            char droid_google_anlt[256];
            char ios_google_anlt[256];		

            char droid_flurry[256];
            char ios_flurry[256];		
            char fb_id_url[256];
            char fbappidsuffix[256];

            std::vector<Module*> modules;

            ExportParams()
            {
                name[0] = 0;
                image[0] = 0;

                package_name[0] = 0;
                package_version_name[0] = 0;
                package_code_name[0] = 0;

                bundle_name[0] = 0;
                bundle_short_name[0] = 0;
                bundle_version[0] = 0;

                fb_id[0] = 0;
                vk_id[0] = 0;		

                droid_google_anlt[0] = 0;
                ios_google_anlt[0] = 0;		

                droid_flurry[0] = 0;
                ios_flurry[0] = 0;		

                fb_id_url[0] = 0;
                fbappidsuffix[0] = 0;
            };

            ~ExportParams()
            {
                RemoveModules();
            }

            void AddModule(Module* module)
            {
                modules.push_back(module);
            }

            void RemoveModules()
            {
                for (int i = 0, size = modules.size(); i < size; ++i)
                {
                    delete modules[i];
                }

                modules.clear();
            }
        };

        ExportParams exp_params;

        bool exp_img_ready;
        char exm_img_src[256];	

        class ExporterNetListiner : public INetworkService::RequestListiner
        {	
            virtual void OnRequestFinish(int res, void* data, const char* file_name);
        };

        ExporterNetListiner exp_net_listiner;

        AndroidXML binary_xml;

        static int android_tex_format;
        static bool force_ios_bitmap;

        static void ConvertTexToAndroid(const char* in, const char* out, const char* ext, Platform platform);
        static void ConvertTexToIOS(const char* in, const char* out, const char* ext, Platform platform);
        static void ConvertFBX(const char* in, const char* out, const char* ext, Platform platform);
        static void ConvertAnimFBX(const char* in, const char* out, const char* ext, Platform platform);
        static void ConvertXML(const char* in, const char* out, const char* ext, Platform platform);

        bool exp_func_registered;

        void CopyImage(const char* ApplicationDir, const char* platform_str, const char* exm_img_src, const char* img, int w, int h);
        void MakeZip(const char* ApplicationDir, const char* platform_str, const char* out_name);
        void SignIPA(const char* ApplicationDir, const char* platform_str, const char* out_name);
        void SignAPK(const char* ApplicationDir, const char* platform_str,
            const char* droid_cert_name, const char* droid_cert_alias, const char* droid_cert_keypass, const char* droid_cert_storepass);
        void AlignAPK(const char* ApplicationDir, const char* platform_str);

    public:	

        class IProgressExportCallback
        {
        public:
            virtual void SetText(const char *text) = 0;
        };

        IProgressExportCallback* progress_callback;

        EditorProject();

        void ResetProject();	
        void SetProjectPath(const char* name);
        void OpenProject(ProjectInfo& project, const char* name);    
        void SaveProject();

        bool ExportResources(Platform platform, const char* ApplicationDir, const char* OutDataPath, const char* project_name);
        void ExportResources(Platform platform, bool gen_package);

        void ReadModulesParams(Platform cur_platform, IDataFile* data, ExportParams* params);

        void RegisterExportFunc();	

        void PrepereExpParametrs(Platform platform);
        void ReplaceKey(const char* file_name, const char* key, const char* value);
        bool Export(const char* exp_name);
        void CopyDir(const char* dir, const char* local_dir, const char* out_dir);

        void SetProgressExportText( const char *format, ... );
    };

}