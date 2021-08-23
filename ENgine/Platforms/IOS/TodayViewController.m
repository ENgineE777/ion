//
//  TodayViewController.m
//  Ext
//
//  Created by Eugene on 7/21/15.
//
//

#import "TodayViewController.h"
#import <NotificationCenter/NotificationCenter.h>

#import "Core/Script/ScriptModule.h"
#import "Core/Core.h"

TodayViewController* tv_controller;

//GLKView* vw = 0;
//static EAGLContext* context = 0;

int  ref_hgt =  500;
bool in_drawing = false;
bool allow_draw = false;

@interface TodayViewController () <NCWidgetProviding>

@end

#include <sys/utsname.h>
NSString *machineName3()
{
    struct utsname systemInfo;
    if (uname(&systemInfo) < 0) {
        return nil;
    } else {
        return [NSString stringWithCString:systemInfo.machine
                                  encoding:NSUTF8StringEncoding];
    }
}


@implementation TodayViewController

- (float)GetScreenScale
{
    if (floor(NSFoundationVersionNumber) > NSFoundationVersionNumber_iOS_7_1)
    {
        return [UIScreen mainScreen].nativeScale;
    }
    
    return [UIScreen mainScreen].scale;
}

- (UIEdgeInsets)widgetMarginInsetsForProposedMarginInsets:(UIEdgeInsets)defaultMarginInsets
{
    return UIEdgeInsetsZero;
}

- (void)viewDidLoad
{
    tv_controller = self;
    
    allow_draw = false;
    
    while (in_drawing)
    {
        NSLog(@"in_drawing");
        ThreadWorker::Sleep(25);
    }
    
    int height = ref_hgt / [self GetScreenScale];
    int width = self.view.bounds.size.width;
    
    self.preferredContentSize = CGSizeMake(320, height);
    
  /*  if (context == 0)
    {
       context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        
        CADisplayLink* displaylink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawFrame)];
        displaylink.frameInterval = 2;
        [displaylink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        
        [EAGLContext setCurrentContext:context];
        
  
        //if (!inited)
        {
            en_core.EN_Scene()->RegisterSceneName("Editor/system_declations.srt");
            
            IFileBuffer* fb = core->Files()->LoadFile("project");
            
            {
                float scale = 1;
                
                if ([[UIScreen mainScreen] respondsToSelector:@selector(scale)])
                {
                    scale = [[UIScreen mainScreen] scale];
                }
                
                float dpi;
                
                if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
                {
                    bool isMini = false;
                    
                    NSString *machName = machineName3();
                    
                    if (machName != nil)
                    {
                        if ([machName isEqualToString:@"iPad2,5"] ||
                            [machName isEqualToString:@"iPad2,6"] ||
                            [machName isEqualToString:@"iPad2,7"])
                        {
                            isMini = true;
                        }
                    }
                    
                    if (isMini)
                    {
                        dpi = 163 * scale;
                    }
                    else
                    {
                        dpi = 132 * scale;
                    }
                }
                else
                    if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)
                    {
                        dpi = 163 * scale;
                    }
                    else
                    {
                        dpi = 160 * scale;
                    }
                
                en_core.Render()->Display()->SetScreenDPI(dpi);
            }
            
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
                
                //debug_info_type = *((int*)ptr);
                ptr += 4;
                
                //debug_info_type = 2;
                
                //sz = *((int*)ptr);
                ptr += 4;
                
                //en_core.Render()->SetTextureType(RenderServiceIOS::tex_pvrtc);
                
                //project_status_bar = *((int*)ptr);
                ptr += 4;
                
                //project_orintation = *((int*)ptr);
                ptr += 4;
                
                ref_hgt = *((int*)ptr);
                ptr += 4;
                
                //en_core.Render()->Display()->SetReferenceHeight(ref_hgt);
                //en_core.EN_Scene()->Load((const char*)ptr);
                //core.Scene()->Load("menu");
                
                fb->Release();
            }
        }
        
        [self initION];*/
    /*}
    else
    {
       //en_core.Render()->ReCreateResources();
    }
    
    bool finded = false;
    
    for (UIView* subview in [self.view subviews])
    {
        if (subview == vw)
        {
            finded = true;
        }
    }*/
    
    /*if (!finded)
    {
        if (!vw)
        {
            vw = [[GLKView alloc] initWithFrame: CGRectMake(0, 0, width, height)];
            vw.context = context;
            vw.opaque = NO;
            vw.backgroundColor = [UIColor clearColor];
    
            vw.delegate = self;
            self.view.opaque = NO;
            self.view.backgroundColor = [UIColor clearColor];
        }
    
        [self.view addSubview:vw];
        
        NSLog(@"[GLKView alloc]");
    }*/
    
    allow_draw = true;
}

- (void)initION
{
    //return;
    
    NSArray *arr = [NSLocale preferredLanguages];
    
    for (NSString *lan in arr)
    {
        NSLog(@"%@: %@ %@",lan, [NSLocale canonicalLanguageIdentifierFromString:lan], [[[NSLocale alloc] initWithLocaleIdentifier:lan] displayNameForKey:NSLocaleIdentifier value:lan]);
        
        en_core.SetLocale([lan UTF8String]);
        
        break;
    }
    //NSLog(language);
    
    self.view.multipleTouchEnabled = true;
    
    core->Init();
    
    en_core.EN_Scene()->RegisterSceneName("Editor/system_declations.srt");
    
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
        
        //debug_info_type = *((int*)ptr);
        ptr += 4;
        
        //debug_info_type = 2;
        
        sz = *((int*)ptr);
        ptr += 4;
        
        //en_core.Render()->SetTextureType(RenderServiceIOS::tex_pvrtc);
        
        //project_status_bar = *((int*)ptr);
        ptr += 4;
        
        //project_orintation = *((int*)ptr);
        ptr += 4;
        
        ref_hgt = *((int*)ptr);
        ptr += 4;
        
        en_core.Render()->Display()->SetReferenceHeight(ref_hgt);
        //en_core.EN_Scene()->Load((const char*)ptr);
        //core.Scene()->Load("menu");
        
        fb->Release();
    }

    if (debug_info_type > 0)
        core->CreateNetLogger();
    
    //last_time = GetCurrentTime();
}
- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)viewDidLayoutSubviews
{
    int height = ref_hgt / [self GetScreenScale];
    int width = [[UIScreen mainScreen] bounds].size.width;
    
    //core->TraceTo("app", "sz - %i %i", width, height);
    
    
    width = self.view.bounds.size.width;
    //[vw setFrame:CGRectMake(0, 0, width, height)];
    
   // core->TraceTo("app", "bounds - %4.3f %4.3f", self.view.bounds.size.width, self.view.bounds.size.height);
    
    NSLog(@"viewDidLayoutSubviews");
}

- (void)drawFrame
{
    //[vw setNeedsDisplay];
}

/*- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    if (!allow_draw)
    {
        return;
    }
    
    in_drawing = true;
    
    //NSLog(@"drawInRect");
    
    /*if (need_init_tiuches)
    {
        for (int i=0; i<10; i++)
        {
            act_touches[i].used = false;
            core->Controls()->TouchEnd(i);
        }
        
        need_init_tiuches = false;
    }*/
    
    /*RenderService::VideoMode mode;
    
    mode.width = rect.size.width * [self GetScreenScale];
    mode.height = rect.size.height * [self GetScreenScale];
    
    en_core.Render()->Display()->ApplyVideoMode(mode);
    
    /*double cur_time = GetCurrentTime();
    
    en_core.SetDeltaTime(cur_time - last_time);
    last_time = cur_time;*/
    
    /*en_core.CountDeltaTime(true);
    
    core->Work();
    
    /*for (int k=0; k<10; k++)
    {
        if (!act_touches[k].used) continue;
        act_touches[k].counter++;
        
        if (act_touches[k].need_end && act_touches[k].counter == 2)
        {
            act_touches[k].used = false;
            core->Controls()->TouchEnd(k);
            core->TraceTo("app", "hack on");
        }
    }*/
    
    //in_drawing = false;
//}

- (void)widgetPerformUpdateWithCompletionHandler:(void (^)(NCUpdateResult))completionHandler {
    // Perform any setup necessary in order to update the view.
    
    // If an error is encountered, use NCUpdateResultFailed
    // If there's no update required, use NCUpdateResultNoData
    // If there's an update, use NCUpdateResultNewData

    completionHandler(NCUpdateResultNewData);
}

@end
