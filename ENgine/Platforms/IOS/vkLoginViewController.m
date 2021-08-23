
#include "core/core.h"
#import "vkLoginViewController.h"

@implementation vkLoginViewController
@synthesize vkWebView, appID, delegate;

//int allow_keyboard_show = 3;
extern bool allow_first_responder;

bool view_was_showed = false;
bool vk_need_hide_view = false;
bool vk_login_view_logined = false;
int  vk_login_view_status = 0;
bool vk_login_need_call_ion = true;

string vk_login_view_token;
string vk_login_view_user_id;

extern int project_orintation;

- (void) dealloc
{
    [delegate release];
    [appID release];
    vkWebView.delegate = nil;
    [vkWebView release];
    [super dealloc];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    
}

-(void)viewDidAppear:(BOOL)animated
{
    if (vk_need_hide_view)
    {
        [self dismissModalViewControllerAnimated:YES];
    }
    
    view_was_showed = true;
}

#pragma mark - View lifecycle
- (void)viewDidLoad
{
    vk_login_view_status = 0;
    
    vk_need_hide_view = false;
    view_was_showed = false;
    
    [super viewDidLoad];
    
    if(!vkWebView)
    {
        self.vkWebView = [[UIWebView alloc] initWithFrame:self.view.bounds];
        vkWebView.delegate = self;
        vkWebView.scalesPageToFit = YES;
        [self.view addSubview:vkWebView];
    }
    
    UIScreen* mainscr = [UIScreen mainScreen];
    
    int offset = 0;
    
    if (![UIApplication sharedApplication].statusBarHidden)
    {
        if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 7.0f)
        {
            offset = [UIApplication sharedApplication].statusBarFrame.size.height;
        }
    }
    
    vkWebView.frame = CGRectMake(0, offset, mainscr.bounds.size.width, mainscr.bounds.size.height - offset);
    
    //allow_keyboard_show = 3;
    
   // [[NSNotificationCenter defaultCenter] addObserver:self
   //                                          selector:@selector(keyboardWillShow:)
   //                                              name:UIKeyboardWillShowNotification object:nil];
    
    //NSString *authLink = [NSString stringWithFormat:@"http://api.vk.com/oauth/authorize?client_id=%@&scope=wall,photos&redirect_uri=http://api.vk.com/blank.html&display=touch&response_type=token", appID];
    
    const char* client_id = "3673620";
    //const char* client_id = "3748102";
    
    int scope = 1 * 0 + 2 + 4 + 8 * 0 + 16 + 32 * 0 + 64 * 0 + 128 * 0 + 1024 * 0 + 2048 * 0 + 4096 * 0 + 8192 + 65536 + 131072 * 0 + 262144 * 0 + 524288 * 0 + 134217728;
    const char* redirect_url = "http://oauth.vk.com/blank.html";
    
    NSString* authLink = [[NSString alloc] initWithFormat:@"http://oauth.vk.com/authorize?client_id=%s&display=touch&scope=%i&redirect_uri=%s&response_type=token", client_id, scope, redirect_url];
    
    NSURL *url = [NSURL URLWithString:authLink];
    
    [vkWebView loadRequest:[NSURLRequest requestWithURL:url]];
}

- (void)viewDidUnload
{
    allow_first_responder = true;
    [super viewDidUnload];
}

- (void) viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
    [vkWebView stopLoading];
    vkWebView.delegate = nil;
}

/*
- (void)keyboardWillShow:(NSNotification *)aNotification
{
    if (allow_keyboard_show <= 0) [self performSelector:@selector(FuckKeyboard) withObject:nil afterDelay:0];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}*/

#pragma mark - Web View Delegate

- (BOOL)webView:(UIWebView *)aWbView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType {

    NSURL *URL = [request URL];
    
    NSLog(@"Request: %@", [URL absoluteString]);
    
    // Пользователь нажал Отмена в веб-форме
    if ([[URL absoluteString] rangeOfString:@"error=access_denied&error_reason=user_denied&error_description=User%20denied%20your%20request"].location != NSNotFound)
    {
        vk_login_view_status = 1;
        
        if (vk_login_need_call_ion)
        {
            en_core.OnlineUserStats()->VK()->SetSignInStatus(false, "error", "noid");
        }
        
        [super dismissModalViewControllerAnimated:YES];
        return NO;
    }
    
	return YES;
}

- (NSUInteger)supportedInterfaceOrientations
{
    /*
     if ( project_orintation == 1)
     return UIInterfaceOrientationPortrait|UIInterfaceOrientationPortraitUpsideDown;
     else
     return UIInterfaceOrientationLandscapeLeft|UIInterfaceOrientationLandscapeRight;
     */
    if ( project_orintation == 0)
        return UIInterfaceOrientationLandscapeLeft|UIInterfaceOrientationLandscapeRight ;
    else if ( project_orintation == 1)
        return UIInterfaceOrientationPortrait|UIInterfaceOrientationPortraitUpsideDown;
    else
        return UIInterfaceOrientationLandscapeLeft|UIInterfaceOrientationLandscapeRight|
        UIInterfaceOrientationPortrait|UIInterfaceOrientationPortraitUpsideDown;
    
}

-(void)webViewDidStartLoad:(UIWebView *)webView
{
    //[self FuckKeyboard];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    [vkWebView endEditing:YES];
}

- (void)FuckKeyboard
{
    for (UIWindow *keyboardWindow in [[UIApplication sharedApplication] windows])
    {
        for (UIView *keyboard in [keyboardWindow subviews])
        {
            if([[keyboard description] hasPrefix:@"<UIPeripheralHostView"] == YES)
            {
                [keyboard removeFromSuperview];
            }
        }
    }
}

-(void)webViewDidFinishLoad:(UIWebView *)webView
{
    //allow_keyboard_show--;
    //if (allow_keyboard_show<=0) [self FuckKeyboard];

    // Если есть токен сохраняем его
    NSLog(vkWebView.request.URL.absoluteString);
    
    if ([vkWebView.request.URL.absoluteString rangeOfString:@"access_token"].location != NSNotFound)
    {
        NSString *accessToken = [self stringBetweenString:@"access_token=" 
                                                andString:@"&" 
                                              innerString:[[[webView request] URL] absoluteString]];
        
        NSArray *userAr = [[[[webView request] URL] absoluteString] componentsSeparatedByString:@"&user_id="];
        NSString *user_id = [userAr lastObject];
        
        vk_login_view_token = [accessToken UTF8String];
        vk_login_view_user_id = [user_id UTF8String];
        vk_login_view_logined = true;
        
        vk_login_view_status = 1;
        
        NSUserDefaults* def = [NSUserDefaults standardUserDefaults];
        
        [def setObject: accessToken forKey:@"vk_token"];
        [def setObject: user_id forKey:@"vk_user_id"];
        
        [def synchronize];
        
        if (vk_login_need_call_ion)
        {
            en_core.OnlineUserStats()->VK()->SetSignInStatus(true, vk_login_view_token.c_str(), vk_login_view_user_id.c_str());
        }
        
        if (!view_was_showed)
        {
            vk_need_hide_view = true;
        }
        else
        {
            [self dismissModalViewControllerAnimated:YES];
        }
    }
    else
    if ([vkWebView.request.URL.absoluteString rangeOfString:@"error"].location != NSNotFound)
    {
        vk_login_view_status = 1;
        
        if (vk_login_need_call_ion)
        {
            en_core.OnlineUserStats()->VK()->SetSignInStatus(false, "error", "noid");
        }
        
        NSLog(@"Error: %@", vkWebView.request.URL.absoluteString);
        [self dismissModalViewControllerAnimated:YES];
    }
}

-(void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error
{
    NSLog(@"vkWebView Error: %@", [error localizedDescription]);
    [self dismissModalViewControllerAnimated:YES];
}

#pragma mark - Methods

- (NSString*)stringBetweenString:(NSString*)start 
                       andString:(NSString*)end 
                     innerString:(NSString*)str 
{
    NSScanner* scanner = [NSScanner scannerWithString:str];
    [scanner setCharactersToBeSkipped:nil];
    [scanner scanUpToString:start intoString:NULL];
    
    if ([scanner scanString:start intoString:NULL])
    {
        NSString* result = nil;
        if ([scanner scanUpToString:end intoString:&result])
        {
            return result;
        }
    }
    return nil;
}



@end
