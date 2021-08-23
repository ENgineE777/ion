
#include <string>
#import "FBShareActivity.h"
#import "Core/Core.h"

extern int fb_share_stage;
extern float fb_share_stage_time;

@implementation FBShareActivity

-(NSString *)activityType
{
    return @"com.ion.fbshareactivity";
}

-(NSString *)activityTitle
{
    return @"Facebook";
}

-(UIImage *)activityImage
{
    /*if (en_core.Render()->Display()->GetWidth() > 1024 &&
        en_core.Render()->Display()->GetHeight() > 1024)
    {
        return [UIImage imageNamed:@"icon_fb_120.png"];
    }*/

    return [UIImage imageNamed:@"icon_fb.png"];
}

-(BOOL)canPerformWithActivityItems:(NSArray *)activityItems
{
    for (NSObject *item in activityItems)
    {
        if (![item isKindOfClass:[NSString class]])
        {
            return NO;
        }
    }
    return YES;
}

-(void)prepareWithActivityItems:(NSArray *)activityItems
{
    self.message = @"";
    
    for (NSObject *item in activityItems)
    {
        self.message = [NSString stringWithFormat:@"%@ %@", self.message, item];
    }
}

-(void)performActivity
{
    fb_share_stage = 1;
    fb_share_stage_time = 0.0f;
    
    [self activityDidFinish:YES];
}
@end