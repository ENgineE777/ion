
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include "Core/Core.h"
#include "Editor/Helpers/EditorProject.h"

int width = 640 * 0.75f;
int height = 960 * 0.75f;
	
char project[256];
char start_func[256];
char start_func_params[256];

int is_hidden = 0;
int net_cache_mode = 0;	
	
struct timespec lastTime;

inline long TimeDifference(const struct timespec& last, const struct timespec& current)
{    
    if (current.tv_sec > last.tv_sec)
        return (1000000000 - last.tv_nsec) + current.tv_nsec;
    else
        return current.tv_nsec - last.tv_nsec;
}

static void
make_x_window(Display *x_dpy, EGLDisplay egl_dpy,
              const char *name,
              int x, int y, int width, int height,
              Window *winRet,
              EGLContext *ctxRet,
              EGLSurface *surfRet)
{
   static const EGLint attribs[] = {
      EGL_RED_SIZE, 1,
      EGL_GREEN_SIZE, 1,
      EGL_BLUE_SIZE, 1,
      EGL_DEPTH_SIZE, 1,
      EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
      EGL_NONE
   };

   static const EGLint ctx_attribs[] = {
      EGL_CONTEXT_CLIENT_VERSION, 2,
      EGL_NONE
   };

   int scrnum;
   XSetWindowAttributes attr;
   unsigned long mask;
   Window root;
   Window win;
   XVisualInfo *visInfo, visTemplate;
   int num_visuals;
   EGLContext ctx;
   EGLConfig config;
   EGLint num_configs;
   EGLint vid;

   scrnum = DefaultScreen( x_dpy );
   root = RootWindow( x_dpy, scrnum );

	printf("eglChooseConfig\n");
   if (!eglChooseConfig( egl_dpy, attribs, &config, 1, &num_configs)) {
      printf("Error: couldn't get an EGL visual config\n");
      exit(1);
   }
   
   printf("eglGetConfigAttrib\n");
   if (!eglGetConfigAttrib(egl_dpy, config, EGL_NATIVE_VISUAL_ID, &vid)) {
      printf("Error: eglGetConfigAttrib() failed\n");
      exit(1);
   }

	printf("XGetVisualInfo\n");

   /* The X window visual must match the EGL config */
   visTemplate.visualid = vid;
   visInfo = XGetVisualInfo(x_dpy, VisualIDMask, &visTemplate, &num_visuals);
   if (!visInfo) {
      printf("Error: couldn't get X visual\n");
      exit(1);
   }

   /* window attributes */
   attr.background_pixel = 0;
   attr.border_pixel = 0;
   attr.colormap = XCreateColormap( x_dpy, root, visInfo->visual, AllocNone);
   attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;
   mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

	printf("XCreateWindow\n");

   win = XCreateWindow( x_dpy, root, 0, 0, width, height,
		        0, visInfo->depth, InputOutput,
		        visInfo->visual, mask, &attr );

   /* set hints and properties */
   {
      XSizeHints sizehints;
      sizehints.x = x;
      sizehints.y = y;
      sizehints.width  = width;
      sizehints.height = height;
      sizehints.flags = USSize | USPosition;
      XSetNormalHints(x_dpy, win, &sizehints);
      XSetStandardProperties(x_dpy, win, name, name,
                              None, (char **)NULL, 0, &sizehints);
   }

	printf("eglBindAPI\n");

   eglBindAPI(EGL_OPENGL_ES_API);

	printf("eglCreateContext\n");

   ctx = eglCreateContext(egl_dpy, config, EGL_NO_CONTEXT, ctx_attribs );
   if (!ctx) {
      printf("Error: eglCreateContext failed\n");
      exit(1);
   }

	printf("eglQueryContext\n");
	
   EGLint val;
   eglQueryContext(egl_dpy, ctx, EGL_CONTEXT_CLIENT_VERSION, &val);      

	printf("eglCreateWindowSurface\n");
	
   *surfRet = eglCreateWindowSurface(egl_dpy, config, win, NULL);
   if (!*surfRet) {
      printf("Error: eglCreateWindowSurface failed\n");
      exit(1);
   }

	printf("eglQuerySurface\n");

   /* sanity checks */
   {
      EGLint val;
      eglQuerySurface(egl_dpy, *surfRet, EGL_WIDTH, &val);
      eglQuerySurface(egl_dpy, *surfRet, EGL_HEIGHT, &val);            
   }

   XFree(visInfo);

   *winRet = win;
   *ctxRet = ctx;
}

void InitION()
{
	printf("InitION\n");
	
	core->Init();
    core->SetProjectPath(project);			
	/*if (net_cache_mode == 1)
	{
		core->Network()->SetWriteCache(true);
	}
	
	if (net_cache_mode == 2)
	{
		core->Network()->SetUseCache(true);
	}*/
	
    IStreamLoader* loader = core->Files()->CreateStreamLoader(project, "project", IStreamLoader::xml);						

	if (loader)
	{		
		int ref_height = 768;
		loader->Read("RefHeight", ref_height);
		
		en_core.Render()->Display()->SetReferenceHeight(ref_height);
		
		core->TraceTo("App", "project %s loaded", project);

		en_core.EN_Scene()->RegisterSceneName("editor/system_declations.srt");

		int start_scene = -1;
		loader->Read("start_scene", start_scene);		
			
		int index = 0;

		char scene[512];		

		while (loader->EnterBlock("Scene"))
		{					
			char path[512];					
			loader->Read("Path", path, 512);			
			
			en_core.EN_Scene()->RegisterSceneName(path);

			char name[128];
			loader->Read("Name", name, 512);			
			name[strlen(name) - 3] = 0;
			
			while (loader->EnterBlock("IncludedScene"))
			{										
				Str256 str;
				loader->Read("Path", str.data, 256);
				
				en_core.EN_Scene()->RegisterIncludeInScene(name, str.data);
				
				loader->LeaveBlock();
			}	
				
			if (index == start_scene)
			{
				loader->Read("Name", scene, 512);							
				scene[strlen(scene) - 3] = 0;
			}
			
			index++;

			loader->LeaveBlock();
		}			

		//en_core.SetEditorMode(true);
		en_core.EN_Scene()->SetSceneLoadType(IStreamLoader::xml);
		SceneInstance* scn = en_core.EN_Scene()->Load(scene);
		
		if (!scn)
		{
			core->TraceTo("App", "project %s not founded", project);
		}
		else
		{	
			if (start_func[0])
			{
				scn->SetStartFunc(start_func, start_func_params);

				core->TraceTo("App", "sf %s was callled with %s", start_func, start_func_params);
			}
		}

		loader->Release();		
	}
	else
	{
		core->TraceTo("App", "project %s not founded", project);
	}
	
	clock_gettime(CLOCK_MONOTONIC, &lastTime);
}

void ApplyMode(int width, int height)
{
	RenderService::VideoMode mode;
	
	mode.width = width;
	mode.height = height;
	
	en_core.Render()->Display()->ApplyVideoMode(mode);
	//core->TraceTo("app", "ApplyMode %i %i", mode.width, mode.height);
}

void IonLoop(Display *dpy, Window win, EGLDisplay egl_dpy, EGLSurface egl_surf)
{
   while (!en_core.IsExited())
	{
      int redraw = 0;
      XEvent event;

	  redraw = 1;

      if (XCheckWindowEvent(dpy, win, -1, &event))	  
	  {
		switch (event.type)
		{
		  case Expose:
			redraw = 1;
		  break;
		  case ConfigureNotify:
			ApplyMode(event.xconfigure.width, event.xconfigure.height);
          break;
		  case ButtonPress:
			core->Controls()->TouchStart(0, event.xbutton.x, event.xbutton.y);
			//core->TraceTo("app", "ButtonPress %i %i", event.xbutton.x, event.xbutton.y);						
		  break;	
		  case MotionNotify:
			core->Controls()->TouchMove(0, event.xbutton.x, event.xbutton.y);
			//core->TraceTo("app", "ButtonPress %i %i", event.xmotion.x_root, event.xmotion.y_root);						
		  break;	  
		  case ButtonRelease:
			core->Controls()->TouchEnd(0);
			//core->TraceTo("app", "ButtonRelease %i %i", event.xbutton.x_root, event.xbutton.y_root);						
		  break;		  
          case KeyPress:
		  {
			  /*char buffer[10];
			  int r, code;
			  code = XLookupKeysym(&event.xkey, 0);
				
			  if (code == XK_Left)
			  {
				  view_roty += 5.0;
			  }
			  else
			  if (code == XK_Right)
			  {
				  view_roty -= 5.0;
			  }
			  else
			  if (code == XK_Up)
			  {
				  view_rotx += 5.0;
			  }
			  else
			  if (code == XK_Down)
			  {
				  view_rotx -= 5.0;
			  }
			  else
			  {
				  r = XLookupString(&event.xkey, buffer, sizeof(buffer), NULL, NULL);
               
				  if (buffer[0] == 27)
				  {
					  return;
				  }
			  }*/
		  }
          redraw = 1;
          break;
		  default:
         ; /*no-op*/
		}		
	  }

      if (redraw)
	  {		  
		  struct timespec currTime;
		  clock_gettime(CLOCK_MONOTONIC, &currTime);
		  long iNanoSeconds = TimeDifference(lastTime, currTime);	
	      lastTime = currTime;

	      en_core.SetDeltaTime((float)iNanoSeconds/1000000000.0f);
		  core->CountDeltaTime(true);
		  
		  core->Work();         
		  eglSwapBuffers(egl_dpy, egl_surf);
      }
   }
}

int main(int argc, char *argv[])
{		
	ProjectInfo::EditorProject core_ed;
	
	if (argc < 1)
	{
		return 0;
	}
		
	char exp_str[256];
	exp_str[0] = 0;
		
	if (argc>1 && ReadArg(argc, argv, "export", exp_str, 256))
	{
		core_ed.Export(exp_str);	

		return 0;		
	}	
	
	project[0] = 0;
	start_func[0] = 0;	
	start_func_params[0] = 0;	

	ReadArg(argc, argv, "scene", project, 256);
	ReadArg(argc, argv, "width", width);
	ReadArg(argc, argv, "height", height);
	ReadArg(argc, argv, "is_hidden", is_hidden);
	ReadArg(argc, argv, "net_cache_mode", net_cache_mode);
	ReadArg(argc, argv, "sfunc", start_func, 256);
	ReadArg(argc, argv, "sfuncp", start_func_params, 256);
		
    const int winWidth = width, winHeight = height;
   
    Display *x_dpy;
    Window win;
    EGLSurface egl_surf;
    EGLContext egl_ctx;
    EGLDisplay egl_dpy;
    char *dpyName = NULL;
    GLboolean printInfo = GL_FALSE;
    EGLint egl_major, egl_minor;
    	
    const char *s;   

    x_dpy = XOpenDisplay(dpyName);
    if (!x_dpy)
	{
		printf("Error: couldn't open display %s\n", dpyName ? dpyName : getenv("DISPLAY"));
        return -1;
    }

    egl_dpy = eglGetDisplay(x_dpy);
	
    if (!egl_dpy)
	{
		printf("Error: eglGetDisplay() failed\n");
        return -1;
    }

    if (!eglInitialize(egl_dpy, &egl_major, &egl_minor))
	{
		printf("Error: eglInitialize() failed\n");
        return -1;
    }

    s = eglQueryString(egl_dpy, EGL_VERSION);
    printf("EGL_VERSION = %s\n", s);

    s = eglQueryString(egl_dpy, EGL_VENDOR);
    printf("EGL_VENDOR = %s\n", s);

    s = eglQueryString(egl_dpy, EGL_EXTENSIONS);
    printf("EGL_EXTENSIONS = %s\n", s);

    s = eglQueryString(egl_dpy, EGL_CLIENT_APIS);
    printf("EGL_CLIENT_APIS = %s\n", s);
	
    make_x_window(x_dpy, egl_dpy,"OpenGL ES 2.x tri", 0, 0, winWidth, winHeight, &win, &egl_ctx, &egl_surf);

    XMapWindow(x_dpy, win);
    
	if (!eglMakeCurrent(egl_dpy, egl_surf, egl_surf, egl_ctx))
	{
		printf("Error: eglMakeCurrent() failed\n");
        return -1;
    }

    if (printInfo)
	{
		printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
        printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
        printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
        printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
    }
   
	InitION();
	
	ApplyMode(winWidth, winHeight);	
	
	IonLoop(x_dpy, win, egl_dpy, egl_surf);
	
    eglDestroyContext(egl_dpy, egl_ctx);
    eglDestroySurface(egl_dpy, egl_surf);
    eglTerminate(egl_dpy);


    XDestroyWindow(x_dpy, win);
    XCloseDisplay(x_dpy);

    return 0;
}