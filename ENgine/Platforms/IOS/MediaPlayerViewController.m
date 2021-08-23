//
//  MediaPlayerViewController.m
//  kxmovie
//
//  Created by Kolyvan on 18.10.12.
//  Copyright (c) 2012 Konstantin Boukreev . All rights reserved.
//
//  https://github.com/kolyvan/kxmovie
//  this file is part of KxMovie
//  KxMovie is licenced under the LGPL v3, see lgpl-3.0.txt

#import "MediaPlayerViewController.h"
#import "KxMovieViewController.h"
#import "KxAudioManager.h"
#include <string>

std::string mediaplayer_path;
std::string mediaplayer_videoName;
static MediaPlayerViewController* g_vc = nil;
int g_num = 0;

@interface MediaPlayerViewController () {
    NSArray *_localMovies;
    NSArray *_remoteMovies;
}
@property (strong, nonatomic) UITableView *tableView;
@end

@implementation MediaPlayerViewController

+ (void)showKxMoviViewController:(UIViewController*)parent

{
    NSString *path;
    NSMutableDictionary *parameters = [NSMutableDictionary dictionary];
    
    path = [[NSString alloc ]initWithUTF8String: mediaplayer_path.c_str()] ;
    NSString* name = [[NSString alloc ]initWithUTF8String: mediaplayer_videoName.c_str()] ;
    
    if (mediaplayer_videoName != "")
    {
        while (name == nil && mediaplayer_videoName.length())
        {
            mediaplayer_videoName.resize(mediaplayer_videoName.length() - 1);
            name = [[NSString alloc ]initWithUTF8String: mediaplayer_videoName.c_str()] ;

        }
    }
   
    // increase buffering for .wmv, it solves problem with delaying audio frames
    if ([path.pathExtension isEqualToString:@"wmv"])
        parameters[KxMovieParameterMinBufferedDuration] = @(5.0);
    
    // disable deinterlacing for iPhone, because it's complex operation can cause stuttering
    if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)
        parameters[KxMovieParameterDisableDeinterlacing] = @(YES);
    
    // disable buffering
    //parameters[KxMovieParameterMinBufferedDuration] = @(0.0f);
    //parameters[KxMovieParameterMaxBufferedDuration] = @(0.0f);
    
    KxMovieViewController *vc = [KxMovieViewController movieViewControllerWithContentPath:path
                                                                               parameters:parameters glcontext:nil videoName:name];
    
    {
        //id<KxAudioManager> audioManager = [KxAudioManager audioManager];
        //[audioManager activateAudioSession];
        //vc = [[KxMovieViewController alloc] initWithContentPath: path parameters: parameters];
    }
    
    [parent presentViewController:vc animated:YES completion:nil];
    

    //[parent presentModalViewController:vc animated:NO ];
    //[ path release];
    
}
/*
- (id)init
{
    self = [super init];
    if (self)
    {
        g_vc = self;
        self.title = @"Movies";
        self.tabBarItem = [[UITabBarItem alloc] initWithTabBarSystemItem:UITabBarSystemItemFeatured tag: 0];
        
        _remoteMovies = @[
                          @"rtsp://r1---sn-5hn7su76.c.youtube.com/CiILENy73wIaGQlZ3Vu_YhygrxMYESARFEgGUgZ2aWRlb3MM/0/0/0/video.3gp",
            @"http://eric.cast.ro/stream2.flv",
            @"http://liveipad.wasu.cn/cctv2_ipad/z.m3u8",                          
            @"http://www.wowza.com/_h264/BigBuckBunny_175k.mov",
            // @"http://www.wowza.com/_h264/BigBuckBunny_115k.mov",
            @"rtsp://184.72.239.149/vod/mp4:BigBuckBunny_115k.mov",
            @"http://santai.tv/vod/test/test_format_1.3gp",
            @"http://santai.tv/vod/test/test_format_1.mp4",
        
            //@"rtsp://184.72.239.149/vod/mp4://BigBuckBunny_175k.mov",
            //@"http://santai.tv/vod/test/BigBuckBunny_175k.mov",
        
            @"rtmp://aragontvlivefs.fplive.net/aragontvlive-live/stream_normal_abt",
            @"rtmp://ucaster.eu:1935/live/_definst_/discoverylacajatv",
            @"rtmp://edge01.fms.dutchview.nl/botr/bunny.flv"
        ];
        
    }
    _ion_window = nil;
    return self;
}

- (void)loadView
{
    self.view = [[UIView alloc] initWithFrame:[[UIScreen mainScreen] applicationFrame]];
    self.tableView = [[UITableView alloc] initWithFrame:self.view.bounds style:UITableViewStylePlain];
    self.tableView.backgroundColor = [UIColor whiteColor];
    //self.tableView.backgroundView = [[UIImageView alloc] initWithImage:image];
    self.tableView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleRightMargin | UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleBottomMargin;
    self.tableView.delegate = self;
    self.tableView.dataSource = self;
    self.tableView.separatorStyle = UITableViewCellSeparatorStyleSingleLine;
    
    [self.view addSubview:self.tableView];
    
   }

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    

}




- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];    
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown);
}

- (void)viewWillAppear:(BOOL)animated
{
    g_num++;
    if (g_num == 2)
    {
        //g_vc.ion_window.makeKeyAndVisible;
        //return;
    }
    
    [super viewWillAppear:animated];
    
    [self reloadMovies];
    [self.tableView reloadData];
}

- (void) reloadMovies
{
    NSMutableArray *ma = [NSMutableArray array];
    NSFileManager *fm = [[NSFileManager alloc] init];
    NSString *folder = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,
                                                            NSUserDomainMask,
                                                            YES) lastObject];
    NSArray *contents = [fm contentsOfDirectoryAtPath:folder error:nil];
    
    for (NSString *filename in contents) {
        
        if (filename.length > 0 &&
            [filename characterAtIndex:0] != '.') {
            
            NSString *path = [folder stringByAppendingPathComponent:filename];
            NSDictionary *attr = [fm attributesOfItemAtPath:path error:nil];
            if (attr) {
                id fileType = [attr valueForKey:NSFileType];
                if ([fileType isEqual: NSFileTypeRegular] ||
                    [fileType isEqual: NSFileTypeSymbolicLink]) {
                    
                    NSString *ext = path.pathExtension.lowercaseString;
                    
                    if ([ext isEqualToString:@"mp3"] ||
                        [ext isEqualToString:@"caff"]||
                        [ext isEqualToString:@"aiff"]||
                        [ext isEqualToString:@"ogg"] ||
                        [ext isEqualToString:@"wma"] ||
                        [ext isEqualToString:@"m4a"] ||
                        [ext isEqualToString:@"m4v"] ||
                        [ext isEqualToString:@"wmv"] ||
                        [ext isEqualToString:@"3gp"] ||
                        [ext isEqualToString:@"mp4"] ||
                        [ext isEqualToString:@"mov"] ||
                        [ext isEqualToString:@"avi"] ||
                        [ext isEqualToString:@"mkv"] ||
                        [ext isEqualToString:@"mpeg"]||
                        [ext isEqualToString:@"mpg"] ||
                        [ext isEqualToString:@"flv"] ||
                        [ext isEqualToString:@"vob"]) {
                        
                        [ma addObject:path];
                    }
                }
            }
        }
    }
    
    _localMovies = [ma copy];
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 2;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    switch (section) {
        case 0:     return @"Remote";
        case 1:     return @"Local";
    }
    return @"";
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    switch (section) {
        case 0:     return _remoteMovies.count;
        case 1:     return _localMovies.count;
    }
    return 0;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *cellIdentifier = @"Cell";
    UITableViewCell *cell = [self.tableView dequeueReusableCellWithIdentifier:cellIdentifier];
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault
                                      reuseIdentifier:cellIdentifier];
        cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    }
    
    NSString *path;
    
    
    if (indexPath.section == 0) {
        
        path = _remoteMovies[indexPath.row];
        
    } else {
        
        path = _localMovies[indexPath.row];
    }

    cell.textLabel.text = path.lastPathComponent;
    return cell;
}

#pragma mark - Table view delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    //g_vc.ion_window.makeKeyAndVisible;
    //return;
    NSString *path;
    NSMutableDictionary *parameters = [NSMutableDictionary dictionary];
    
    
    //path = _remoteMovies[indexPath.row];
    
    if (indexPath.section == 0)
    {
        
        path = _remoteMovies[indexPath.row];
        
    }
    else if (_localMovies != nil)
    {
        
        path = _localMovies[indexPath.row];
    }
    
    
    //path = [[NSString alloc ]initWithUTF8String: mediaplayer_path.c_str()] ;
    
    // increase buffering for .wmv, it solves problem with delaying audio frames
    if ([path.pathExtension isEqualToString:@"wmv"])
        parameters[KxMovieParameterMinBufferedDuration] = @(5.0);
    
    // disable deinterlacing for iPhone, because it's complex operation can cause stuttering
    if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)
        parameters[KxMovieParameterDisableDeinterlacing] = @(YES);
    
    // disable buffering
    //parameters[KxMovieParameterMinBufferedDuration] = @(0.0f);
    //parameters[KxMovieParameterMaxBufferedDuration] = @(0.0f);
    
    KxMovieViewController *vc = [KxMovieViewController movieViewControllerWithContentPath:path
                                                                               parameters:parameters glcontext: nil];
    //[self presentViewController:vc animated:YES completion:nil];
    [self presentModalViewController:vc animated:NO];
    //path.release;
     
    //[self.navigationController pushViewController:vc animated:YES];    
}
*/
@end
