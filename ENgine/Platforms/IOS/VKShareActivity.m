
#include <string>
#import "VKShareActivity.h"
#import "Core/Core.h"

extern int vk_share_stage;
extern float vk_share_stage_time;

@implementation VKShareActivity

-(NSString *)activityType
{
    return @"com.ion.vkshareactivity";
}

-(NSString *)activityTitle
{
    return @"VK";
}

-(UIImage *)activityImage
{
    /*if (en_core.Render()->Display()->GetWidth() > 1024 &&
        en_core.Render()->Display()->GetHeight() > 1024)
    {
        return [UIImage imageNamed:@"icon_vk_120.png"];
    }*/
    
    return [UIImage imageNamed:@"icon_vk.png"];
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
    vk_share_stage = 1;
    vk_share_stage_time = 0.0f;
    
    [self activityDidFinish:YES];
}
@end