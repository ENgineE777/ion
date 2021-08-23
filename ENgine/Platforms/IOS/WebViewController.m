
#include "core/core.h"
#import "WebViewController.h"

string webview_url;
string webview_stop_url;

@implementation WebViewController
@synthesize /*webView,*/ delegate;

UIButton* btn_prev;
UIButton* btn_next;
UIButton* btn_refresh;
UIButton* btn_open_panel;
UILabel*  lbl_url;
UILabel*  lbl_loading;

NSString* webview_url_ns;
NSString* webview_stop_url_ns;

extern bool allow_first_responder;

- (void) dealloc
{
    [delegate release];
    webView.delegate = nil;
    [webView release];
    [super dealloc];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    
}



#pragma mark - View lifecycle
- (void)viewDidLoad
{
    [super viewDidLoad];
    
    if(!webView)
    {
        webView = [[UIWebView alloc] initWithFrame:self.view.bounds];
        webView.delegate = self;
        webView.scalesPageToFit = YES;
        [self.view addSubview:webView];
    }
    
    UIImage *image;
    UIScreen* mainscr = [UIScreen mainScreen];

    int offset = 0;
    
    if (![UIApplication sharedApplication].statusBarHidden)
    {
        if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 7.0f)
        {
            offset = [UIApplication sharedApplication].statusBarFrame.size.height;
        }
    }
    
    webView.frame = CGRectMake(0, 55 + offset, mainscr.bounds.size.width, mainscr.bounds.size.height - 55 - offset);
    
    UILabel* top_line = [[UILabel alloc] initWithFrame:CGRectMake(0, offset, mainscr.bounds.size.width, 55)];
    top_line.backgroundColor = [UIColor colorWithRed:0.97 green:0.97 blue:0.97 alpha:1.0];
    [self.view addSubview:top_line];
    
    lbl_url = [[UILabel alloc] initWithFrame:CGRectMake(50, 10 + offset, mainscr.bounds.size.width - 100, 36)];
    lbl_url.text = @"dsafasfafaf";
    lbl_url.backgroundColor = [UIColor clearColor];
    lbl_url.textAlignment = NSTextAlignmentCenter;
    [self.view addSubview:lbl_url];
    
    lbl_loading = [[UILabel alloc] initWithFrame:CGRectMake(mainscr.bounds.size.width * 0.5 - 50, offset + 10, 100, 36)];
    lbl_loading.text = @"Загрузка...";
    lbl_loading.backgroundColor = [UIColor clearColor];
    lbl_loading.textAlignment = NSTextAlignmentCenter;
    [self.view addSubview:lbl_loading];
    
    UIButton* btn_close = [UIButton buttonWithType:UIButtonTypeCustom];
    btn_close.frame = CGRectMake(10, 10 + offset, 36, 36);
    btn_close.tag = 1001;
    image = [UIImage imageNamed:@"action_close_p_night.png"];
    if (image != nil) [btn_close setBackgroundImage:image forState:UIControlStateNormal];
    [btn_close addTarget:self action:@selector(ButtonResp:) forControlEvents:UIControlEventTouchDown];
    [self.view addSubview:btn_close];

    
    btn_prev = [UIButton buttonWithType:UIButtonTypeCustom];
    btn_prev.frame = CGRectMake(10, mainscr.bounds.size.height - offset - 46, 36, 36);
    btn_prev.tag = 1002;
    image = [UIImage imageNamed:@"action_backward_normal_night.png"];
    if (image != nil) [btn_prev setBackgroundImage:image forState:UIControlStateNormal];
    [btn_prev addTarget:self action:@selector(ButtonResp:) forControlEvents:UIControlEventTouchDown];
    [self.view addSubview:btn_prev];
    
    btn_next = [UIButton buttonWithType:UIButtonTypeCustom];
    btn_next.frame = CGRectMake(mainscr.bounds.size.width - 46, mainscr.bounds.size.height - offset - 46, 36, 36);
    btn_next.tag = 1003;
    image = [UIImage imageNamed:@"action_forward_normal_night.png"];
    if (image != nil) [btn_next setBackgroundImage:image forState:UIControlStateNormal];
    [btn_next addTarget:self action:@selector(ButtonResp:) forControlEvents:UIControlEventTouchDown];
    [self.view addSubview:btn_next];
    
    btn_refresh = [UIButton buttonWithType:UIButtonTypeCustom];
    btn_refresh.frame = CGRectMake(mainscr.bounds.size.width * 0.5 - 10 - 58, 10 + offset + 8, 22, 22);
    btn_refresh.tag = 1004;
    image = [UIImage imageNamed:@"ic_refresh_n.png"];
    if (image != nil) [btn_refresh setBackgroundImage:image forState:UIControlStateNormal];
    [btn_refresh addTarget:self action:@selector(ButtonResp:) forControlEvents:UIControlEventTouchDown];
    [self.view addSubview:btn_refresh];
   
/*    [UIView animateWithDuration:0.3 delay:0.0 options:UIViewAnimationOptionRepeat | UIViewAnimationOptionCurveLinear animations:^{
        CGAffineTransform transform = CGAffineTransformMakeRotation(-2*M_PI);
        btn_refresh.transform = transform;
    } completion:NULL];*/
    
    btn_open_panel = [UIButton buttonWithType:UIButtonTypeCustom];
    btn_open_panel.frame = CGRectMake(mainscr.bounds.size.width - 46, 10 + offset - 2, 36, 36);
    btn_open_panel.tag = 1005;
    image = [UIImage imageNamed:@"action_open_panel.png"];
    if (image != nil) [btn_open_panel setBackgroundImage:image forState:UIControlStateNormal];
    [btn_open_panel addTarget:self action:@selector(ButtonResp:) forControlEvents:UIControlEventTouchDown];
    [self.view addSubview:btn_open_panel];
    
    NSString* siteUrl = [[NSString alloc] initWithFormat:@"%s", webview_url.c_str()];
    
    webview_url_ns = [[siteUrl stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding] copy];
    NSURL *weburl = [NSURL URLWithString:webview_url_ns];
    
    siteUrl = [[NSString alloc] initWithFormat:@"%s", webview_stop_url.c_str()];
    
    webview_stop_url_ns = [[siteUrl stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding] copy];
    
    [webView loadRequest:[NSURLRequest requestWithURL:weburl]];
    
    [NSTimer scheduledTimerWithTimeInterval:0.033 target:self selector:@selector(RotateRefresh) userInfo:nil repeats:YES];
}

- (void)RotateRefresh
{
    btn_refresh.transform = CGAffineTransformRotate(btn_refresh.transform, M_PI_2 * 0.1f);
}

- (void)ButtonResp:(UIButton*)button
{
    if (button.tag == 1001)
    {
        [self dismissModalViewControllerAnimated:YES];;
    }
    else
    if (button.tag == 1002)
    {
        [webView goBack];
    }
    else
    if (button.tag == 1003)
    {
        [webView goForward];
    }
    else
    if (button.tag == 1004)
    {
        [webView reload];
    }
    else
    if (button.tag == 1005)
    {
        UIActionSheet* actionSheet = [[UIActionSheet alloc] init] ;
    
        actionSheet.title = webview_url_ns;
        actionSheet.delegate = self;
        
        if (String::IsEqual(core->GetLocale(), "ru"))
        {
            [actionSheet addButtonWithTitle:@"Открыть в Safari"];
            [actionSheet addButtonWithTitle:@"Скопировать в буфер"];
            
            [actionSheet addButtonWithTitle:@"Отмена"];
        }
        else
        {
            [actionSheet addButtonWithTitle:@"Open in Safari"];
            [actionSheet addButtonWithTitle:@"Copy to clipboard"];
        
            [actionSheet addButtonWithTitle:@"Cancel"];
        }
        
        [actionSheet setCancelButtonIndex:(actionSheet.numberOfButtons - 1)];
        
        [actionSheet showInView:self.view];
        
        [actionSheet release];
    }
}

-(void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if (buttonIndex == 0)
    {
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:webview_url_ns]];
    }
    else
    if (buttonIndex == 1)
    {
        UIPasteboard *pb = [UIPasteboard generalPasteboard];
        [pb setString:webview_url_ns];
    }
}

- (BOOL)disablesAutomaticKeyboardDismissal
{
    return NO;
}

bool webview_anim_started = false;

- (void)StartRotate:(bool)start
{
    if (webview_anim_started == start) return;
    
    webview_anim_started = start;
    
    btn_refresh.hidden = !start;
    lbl_loading.hidden = !start;
    
    lbl_url.hidden = start;
}

- (void)UpdateNavButtons
{
    bool vis_back = [webView canGoBack];
    bool vis_frvd = [webView canGoForward];
    
    btn_prev.hidden = !vis_back;
    btn_next.hidden = !vis_frvd;
}

- (void)viewDidUnload
{
    allow_first_responder = true;
    [super viewDidUnload];
}

- (void) viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
    [webView stopLoading];
    webView.delegate = nil;
}


- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

#pragma mark - Web View Delegate

- (BOOL)webView:(UIWebView *)aWbView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType
{
    NSURL *URL = [request URL];
    
	NSLog(@"Request: %@", [URL absoluteString]);
    
    // Пользователь нажал Отмена в веб-форме
    if ([[URL absoluteString] rangeOfString:webview_stop_url_ns].location != NSNotFound)
    {
        [super dismissModalViewControllerAnimated:YES];
        
        return NO;
    }
    
    
    if ([[URL absoluteString] isEqualToString:@"about:blank"])
    {
        return NO;
    }
    
    webview_url_ns = [[URL absoluteString] copy];
    
    lbl_url.text = webview_url_ns;
    
    [self StartRotate:true];
    [self UpdateNavButtons];
    
    return YES;
}

-(void)webViewDidStartLoad:(UIWebView *)webView
{
}

-(void)webViewDidFinishLoad:(UIWebView *)webView
{
    [self UpdateNavButtons];
    [self StartRotate:false];
}

-(void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error
{
    
    NSLog(@"webView Error: %@", [error localizedDescription]);
    //[self dismissModalViewControllerAnimated:YES];
    [self StartRotate:false];
}

#pragma mark - Methods

- (NSString*)stringBetweenString:(NSString*)start 
                       andString:(NSString*)end 
                     innerString:(NSString*)str 
{
    NSScanner* scanner = [NSScanner scannerWithString:str];
    [scanner setCharactersToBeSkipped:nil];
    [scanner scanUpToString:start intoString:NULL];
    if ([scanner scanString:start intoString:NULL]) {
        NSString* result = nil;
        if ([scanner scanUpToString:end intoString:&result]) {
            return result;
        }
    }
    return nil;
}



@end
