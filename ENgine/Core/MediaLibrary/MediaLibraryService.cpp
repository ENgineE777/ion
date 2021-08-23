#include "Core/Core.h"
#include "MediaLibraryService.h"
#include <algorithm>
#include "Common/libxml/tinyxml2.h"


MediaLibraryService::MediaLibraryService() : state(Idle), scanning_apps_done(false)
{
	scanning_time = -1;
	on_end_callback = null;
	worker.owner = this;

	on_end_callback = null;
	on_end_callback_data = null;
}

bool  MediaLibraryService::Init()
{
	core->AddTask(this, this, (TaskHolder::Task)&MediaLibraryService::Work, -320, -1.f);
	return true;
}

void MediaLibraryService::Work(float dt, int level)
{
	//if (scanning_time >= 0)
	//	scanning_time += dt;

	//if (scanning_time > 1)
	//{
	//	on_end_callback(0);
	//	scanning_time = -1;
	//}

	if (worker.IsWorking()) return;

	if (state == ReadyToFinalize) FinalizeScan();

	if (on_end_callback && (state == Done || state == LastScanFailed))
	{
		if (state == Done)
		{
			on_end_callback->OnFinish(on_end_callback_data, 0);
		}
		else
		if (state == LastScanFailed)
		{			
			on_end_callback->OnFinish(on_end_callback_data, -1);
		}

		on_end_callback = null;
		on_end_callback_data = 0;
	}
}

void MediaLibraryService::StartScanning(FinishListiner* listiner, void* data)
{
	if (on_end_callback)
	{
		core->TraceTo("MediaLibrary", "Error: previous call not ended");
		return;
	}

	on_end_callback = listiner;
	on_end_callback_data = data;

	Scan();

	//scanning_time = 0;

}
int MediaLibraryService::GetFilesCount()
{
	return images.size();
}

#ifdef IOS
void SaveBytes(unsigned char* bytes, int size, const char* path)
{
    //core->TraceTo("UserStats", "Start ThreadSaver::Loop()");
    
	IFile* file = core->Files()->OpenFile(path, IFile::write);
    
	if (file)
	{
		file->Write(bytes, size);
        
		file->Release();
	}
}

@implementation ImageRotation

+ (CGImageRef)CGImageRotatedByAngle:(CGImageRef)imgRef angle:(CGFloat)angle
{
    CGFloat angleInRadians = angle * (M_PI / 180);
    CGFloat width = CGImageGetWidth(imgRef);
    CGFloat height = CGImageGetHeight(imgRef);
    CGRect imgRect = CGRectMake(0, 0, width, height);
    CGAffineTransform transform = CGAffineTransformMakeRotation(angleInRadians);
    CGRect rotatedRect = CGRectApplyAffineTransform(imgRect, transform);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef bmContext = CGBitmapContextCreate(NULL,
                                                   rotatedRect.size.width,
                                                   rotatedRect.size.height,
                                                   8,
                                                   0,
                                                   colorSpace,
                                                   kCGImageAlphaPremultipliedFirst);
    CGContextSetAllowsAntialiasing(bmContext, YES);
    CGContextSetInterpolationQuality(bmContext, kCGInterpolationHigh);
    CGColorSpaceRelease(colorSpace);
    CGContextTranslateCTM(bmContext,
                          +(rotatedRect.size.width/2),
                          +(rotatedRect.size.height/2));
    CGContextRotateCTM(bmContext, angleInRadians);
    CGContextDrawImage(bmContext, CGRectMake(-width/2, -height/2, width, height),
                       imgRef);
    CGImageRef rotatedImage = CGBitmapContextCreateImage(bmContext);
    CFRelease(bmContext);
    [(id)rotatedImage autorelease];
    return rotatedImage;
}
@end

#endif

void MediaLibraryService::GetFileName(int i, std::string& name, bool prepareForLoading)
{
	name = images[i].full_name;
   
#ifdef IOS
	if (prepareForLoading)
	{
		ALAsset* result = images[i].asset;

		if (result != nil)
		{
			//NSDate* date = [result valueForProperty:ALAssetPropertyDate];

			if (result.retainCount > 0)
			{
                /*
				Byte *buffer = (Byte*)malloc([[result defaultRepresentation ] size]);

				NSUInteger k = [result.defaultRepresentation getBytes:buffer fromOffset: 0.0
length:result.defaultRepresentation.size error:nil];

				NSString* filename = [result.defaultRepresentation filename];

				//UIImage* thumbnailImage = [UIImage imageWithCGImage:[result aspectRatioThumbnail]];
				//NSData * data = UIImageJPEGRepresentation(thumbnailImage, 1);
				//SaveBytes( (Byte*)data.bytes, data.length, filename.UTF8String);

				SaveBytes( buffer, result.defaultRepresentation.size, filename.UTF8String);
                free(buffer);
*/
                
                // Retrieve the image orientation from the ALAsset
                UIImageOrientation orientation = UIImageOrientationUp;
                NSNumber* orientationValue = [result valueForProperty:@"ALAssetPropertyOrientation"];
                if (orientationValue != nil) {
                    orientation = [orientationValue intValue];
                }
                
                CGFloat scale  = 1;
                int angle = 0;
                if ( orientation == UIImageOrientationUp)            // default orientation
                    angle = 0;
                if ( orientation == UIImageOrientationDown)          // 180 deg rotation
                    angle = 180;
                if ( orientation == UIImageOrientationLeft)          // 90 deg CCW
                    angle = +90;
                if ( orientation == UIImageOrientationRight)         // 90 deg CW
                    angle = -90;
                //if (orientation == 3)
                CGImageRef imageRef = nil;
                /*
                imageRef = [ImageRotation CGImageRotatedByAngle:result.defaultRepresentation.fullResolutionImage angle:angle];

                UIImage* image2 = [UIImage imageWithCGImage:imageRef
                                                      scale:scale orientation:orientation];*/
                
                UIImage* image2 = [UIImage imageWithCGImage:result.defaultRepresentation.fullScreenImage
                                                    scale:scale orientation:orientation];
                NSData * data = UIImageJPEGRepresentation(image2, 1);
				SaveBytes( (Byte*)data.bytes, data.length, name.c_str());

				
			}
		}
	}
#endif
}

void MediaLibraryService::GetFileNameThumbnail(int i, std::string& name, bool prepareForLoading)
{
	name = images[i].full_name;

#ifdef PC
	char ext[8];
	String::ExtractExctention(name.c_str(), ext, 7);
	name.resize(name.length() - 4);
	name += "_thumbnail.";
	name += ext;

	IFile* file = core->Files()->OpenFile( name.c_str(), IFile::read);
	

	if ( prepareForLoading && !file )
	{
		char cmd[1024];
		sprintf(cmd, " -thumb 350 250 -o %s -overwrite  %s", name.c_str()
			, images[i].full_name );


		HINSTANCE code = ShellExecute(null, null, "Bin\\nconvert.exe", cmd, NULL, SW_HIDE);
		ThreadWorker::Sleep(1000);
	}
	RELEASE(file);

#endif

#ifdef IOS
	char ext[8];
	String::ExtractExctention(name.c_str(), ext, 7);
	name.resize(name.length() - 4);
	name += "_thumbnail.";
	name += ext;

	if ( prepareForLoading )
	{


		ALAsset* result = images[i].asset;

		if (result != nil)
		{
			//NSDate* date = [result valueForProperty:ALAssetPropertyDate];

			if (result.retainCount > 0)
			{          
				//Byte *buffer = (Byte*)malloc([[result defaultRepresentation ] size]);

				//NSUInteger k = [result.defaultRepresentation getBytes:buffer fromOffset: 0.0
//length:result.defaultRepresentation.size error:nil];

				//NSString* filename = [result.defaultRepresentation filename];

				UIImage* thumbnailImage = [UIImage imageWithCGImage:[result aspectRatioThumbnail]];
				NSData * data = UIImageJPEGRepresentation(thumbnailImage, 1);
				SaveBytes( (Byte*)data.bytes, data.length, name.c_str());


				//free(buffer);
			}
		}
	}
#endif
}

void  MediaLibraryService::Release()
{
	while (worker.IsWorking())
	{
		ThreadWorker::Sleep(100);			
	}
}

#ifdef PC
#include <sstream>

static const char* kImageDir = "MediaLibrary/Images/";
static const char* kAppDir = "MediaLibrary/Apps/";
#endif

#ifdef OSUNIX
#include <sstream>

static const char* kImageDir = "MediaLibrary/Images/";
static const char* kAppDir = "MediaLibrary/Apps/";
#endif

#ifdef ANDROID
#include <dirent.h>
#include <sys/stat.h>

static const char* kImageDir = "/sdcard/DCIM/";

extern void CallFromJava(const char* function);
#endif

#ifdef IOS

#import <AssetsLibrary/AssetsLibrary.h>

#include <sstream>

static NSMutableArray* detected_apps = nil;


static const char* kImageDir = "";


void MediaLibraryService::AddItem(int day, int month, int year, const char* path, ALAsset* asset)
{
	ImageDesc desc;

	desc.creation_date.Init(day, month, year);
	String::Copy(desc.full_name, sizeof(desc.full_name), path);
    //sprintf(desc.full_name, "DCIM/100APPLE/%s", path);
    String::LowerCase(desc.full_name);
    
    char temp[512];
    String::Copy(temp, 512, path);
    String::LowerCase(temp);
    NSString *imageFilename = [NSString stringWithFormat:@"%s", temp];
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *fullpath = [NSString stringWithFormat:@"%@/%@", [paths objectAtIndex:0], imageFilename];
    String::Copy(desc.full_name, sizeof(desc.full_name), [fullpath UTF8String]);
	desc.name = desc.full_name;
    desc.asset = asset;
    [asset retain];
	images.push_back(desc);
}

static ALAssetsGroupEnumerationResultsBlock groupEnumerationResultsBlock = ^(ALAsset* result, NSUInteger index, BOOL* stop)
{
	if (result != nil)
	{
		NSDate* date = [result valueForProperty:ALAssetPropertyDate];

		if (date)
		{
			NSDateFormatter* dateFormat = [[NSDateFormatter alloc] init];

			[dateFormat setDateFormat:@"yyyy"];

			NSString* sYear = [dateFormat stringFromDate:date];
			NSNumber* nYear = [NSNumber numberWithInteger:[sYear integerValue]];

			[dateFormat setDateFormat:@"MM"];
			NSString* sMonth = [dateFormat stringFromDate:date];
			NSNumber* nMonth = [NSNumber numberWithInteger:[sMonth integerValue]];

			[dateFormat setDateFormat:@"dd"];
			NSString* sDay = [dateFormat stringFromDate:date];
			NSNumber* nDay = [NSNumber numberWithInteger:[sDay integerValue]];

			ALAssetRepresentation* representation = [result defaultRepresentation];
			NSString* URL = [[representation url] absoluteString];
             NSString* filename = [representation filename];
            

            
            /*
            Byte *buffer = (Byte*)malloc([[result defaultRepresentation ] size]);
            
            NSUInteger k = [result.defaultRepresentation getBytes:buffer fromOffset: 0.0
                                                          length:result.defaultRepresentation.size error:nil];
            
           
            
            SaveBytes( buffer, result.defaultRepresentation.size, filename.UTF8String);
            free(buffer);
             */
           
			en_core.MediaLibrary()->AddItem([nDay integerValue], [nMonth integerValue], [nYear integerValue], [filename UTF8String], result);
		}
	}
};

static ALAssetsLibraryAccessFailureBlock failureBlock = ^(NSError *error)
{
	NSLog(@"Failed to enumerate library %@", error);
};

static ALAssetsLibraryGroupsEnumerationResultsBlock assetGroupEnumerator = ^(ALAssetsGroup *group, BOOL *stop)
{
	[group setAssetsFilter:[ALAssetsFilter allPhotos]];

	if (group != nil)
	{
		[group enumerateAssetsUsingBlock:groupEnumerationResultsBlock];
		en_core.MediaLibrary()->SetState(MediaLibraryService::ReadyToFinalize);
	}
};

#endif

static bool SortByCreationDate(const MediaLibraryService::ImageDesc& lhs, const MediaLibraryService::ImageDesc& rhs)
{
	return lhs.creation_date < rhs.creation_date;
}







void MediaLibraryService::Scan(const char* path, int depth)
{
	core->TraceTo("MediaLibrary", "path: %s, depth: %d", path, depth);

	if (depth > 5) return;

#ifdef PC
	char find_pattern[MAX_PATH];
	String::Copy(find_pattern, MAX_PATH, path);
	String::Cat(find_pattern, MAX_PATH, "*");

	WIN32_FIND_DATA find_data;
	HANDLE find_handle = ::FindFirstFile(find_pattern, &find_data);

	if (find_handle == INVALID_HANDLE_VALUE)
	{
		core->TraceTo("Script", "Error. Cannot scan the directory: %s", path);
		return;
	}

	do
	{
		if (find_data.cFileName[0] == '.') continue;

		if ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			const int ext_offset = strlen(find_data.cFileName) - 4;
			if (ext_offset < 0) continue;

			if (String::Compare(find_data.cFileName + ext_offset, ".jpg") != 0
				&& String::Compare(find_data.cFileName + ext_offset, ".png") != 0
				&& String::Compare(find_data.cFileName + ext_offset, ".bmp") != 0
				&& String::Compare(find_data.cFileName + ext_offset, ".JPG") != 0
				&& String::Compare(find_data.cFileName + ext_offset, ".PNG") != 0
				&& String::Compare(find_data.cFileName + ext_offset, ".BMP") != 0)
					continue;
				
				if ( String::IsSubstr(find_data.cFileName, "thumbnail")) 
					continue;

			ImageDesc desc;

			desc.creation_date.Init(find_data.ftCreationTime);
			String::Copy(desc.full_name, sizeof(desc.full_name), path);
			String::Cat(desc.full_name, sizeof(desc.full_name), find_data.cFileName);

			desc.name = desc.full_name + strlen(path);

			images.push_back(desc);
		}
		else
		{
			String::Copy(find_pattern, MAX_PATH, path);
			String::Cat(find_pattern, MAX_PATH, find_data.cFileName);
			String::Cat(find_pattern, MAX_PATH, "/");

			Scan(find_pattern, depth + 1);
		}
	}
	while (::FindNextFile(find_handle, &find_data));

	::FindClose(find_handle);

	if (depth == 0) state = ReadyToFinalize;
#endif

#ifdef ANDROID
	DIR* image_dir = opendir(path);

	if (!image_dir)
	{
		core->TraceTo("MediaLibrary", "Error. Cannot scan the directory: %s", path);
		return;
	}

	struct dirent* ent = NULL;
	while ((ent = readdir(image_dir)))
	{
		if (ent->d_name[0] == '.') continue;

		if ((ent->d_type & DT_DIR) == 0)
		{
			const int ext_offset = strlen(ent->d_name) - 4;
			if (ext_offset < 0) continue;

			if (String::Compare(ent->d_name + ext_offset, ".jpg") != 0
				&& String::Compare(ent->d_name + ext_offset, ".png") != 0
				&& String::Compare(ent->d_name + ext_offset, ".bmp") != 0) continue;

			ImageDesc desc;

			String::Copy(desc.full_name, sizeof(desc.full_name), path);
			String::Cat(desc.full_name, sizeof(desc.full_name), ent->d_name);

			desc.name = desc.full_name + strlen(path);

			struct stat st;
			if (stat(desc.full_name, &st) < 0)
			{
				core->TraceTo("MediaLibrary", "Cannot stat file: %s", desc.full_name);
			}

			desc.creation_date.Init(st.st_ctime);

			images.push_back(desc);
		}
		else
		{
			char find_pattern[MAX_PATH];

			String::Copy(find_pattern, MAX_PATH, path);
			String::Cat(find_pattern, MAX_PATH, ent->d_name);
			String::Cat(find_pattern, MAX_PATH, "/");

			Scan(find_pattern, depth + 1);
		}
	}

	closedir(image_dir);

	state = ReadyToFinalize;
#endif

#ifdef IOS
	ALAssetsLibrary *assetsLibrary = [[ALAssetsLibrary alloc] init];
	[assetsLibrary enumerateGroupsWithTypes:ALAssetsGroupSavedPhotos usingBlock:assetGroupEnumerator failureBlock:failureBlock];
#endif
}

void MediaLibraryService::FinalizeScan()
{
	std::sort(images.begin(), images.end(), SortByCreationDate);

	ImageDesc* prev_img_by_year = &images[0];
	ImageDesc* prev_img_by_month = &images[0];
	ImageDesc* prev_img_by_day = &images[0];
	ImageDesc* prev = &images[0];

	for (int i = 0, size = images.size(); i < size; ++i)
	{
		ImageDesc& img = images[i];

		img.index = i;

		if (img.creation_date.GetYear() != prev_img_by_year->creation_date.GetYear())
		{
			prev_img_by_year->next_by_year = &img;
			prev_img_by_year = &img;
		}

		if (!(img.creation_date.GetMonth() == prev_img_by_month->creation_date.GetMonth()
			&& img.creation_date.GetYear() == prev_img_by_month->creation_date.GetYear()))
		{
			if (img.creation_date.GetYear() == prev_img_by_month->creation_date.GetYear())
			{
				prev_img_by_month->next_by_month = &img;
			}

			prev_img_by_month = &img;
		}

		if (!(img.creation_date.GetDay() == prev_img_by_day->creation_date.GetDay()
			&&img.creation_date.GetYear() == prev_img_by_day->creation_date.GetYear()
			&& img.creation_date.GetMonth() == prev_img_by_day->creation_date.GetMonth()))
		{
			if (img.creation_date.GetYear() == prev_img_by_day->creation_date.GetYear()
				&& img.creation_date.GetMonth() == prev_img_by_day->creation_date.GetMonth())
			{
				prev_img_by_day->next_by_day = &img;
			}

			prev_img_by_day = &img;
		}

		if (&img == prev) continue;

		if (img.creation_date.GetYear() == prev->creation_date.GetYear()
			&& img.creation_date.GetMonth() == prev->creation_date.GetMonth()
			&& img.creation_date.GetDay() == prev->creation_date.GetDay())
		{
			prev->next = &img;
		}

		prev = &img;
	}

	state = Done;
}

void MediaLibraryService::Scan()
{
	if (state != Done && state != Idle)
	{
		core->TraceTo("MediaLibrary", "Error: The previous scanning has not been finished yet.");
		return;
	}

	state = ScanInProgress;
	images.clear();

	worker.Start(ThreadWorker::normal);
}



bool MediaLibraryService::GetDate(int index, int& day, int& month, int& year)
{
	if (state != Done) return false;
	if (index >= images.size()) return false;

	const Date& date = images[index].creation_date;

	day = date.GetDay();
	month = date.GetMonth();
	year = date.GetYear();

	return true;
}

bool MediaLibraryService::GetTime(int index, int& hour, int& minute, int& second)
{
	if (state != Done) return false;
	if (index >= images.size()) return false;

	const Date& date = images[index].creation_date;

	hour = date.GetHour();
	minute = date.GetMinute();
	second = date.GetSecond();

	return true;
}

MediaLibraryService::ImageDesc* MediaLibraryService::GetImageByYear(int year)
{
	if (year < 0) return NULL;

	int index = GetIndexByYear(year);
	if (index >= 0 )
		return &images[index];
	else 
		return null;

	ImageDesc* cur = &images[0];
	while (cur && --year >= 0) cur = cur->next_by_year;

	return cur;
}

MediaLibraryService::ImageDesc* MediaLibraryService::GetImageByMonth(int year, int month)
{
	if (month < 0) return NULL;

	int index = GetIndexByMonth(year, month);
	if (index >= 0 )
		return &images[index];
	else 
		return null;

	ImageDesc* cur = GetImageByYear(year);
	while (cur && --month >= 0) cur = cur->next_by_month;

	return cur;
}

MediaLibraryService::ImageDesc* MediaLibraryService::GetImageByDay(int year, int month, int day)
{
	if (day < 0) return NULL;

	int index = GetIndexByDay(year, month, day);
	if (index >= 0 )
		return &images[index];
	else 
		return null;

	ImageDesc* cur = GetImageByMonth(year, month);
	while (cur && --day >= 0) cur = cur->next_by_day;

	return cur;
}

MediaLibraryService::ImageDesc* MediaLibraryService::GetImage(int year, int month, int day, int index)
{
	if (index < 0) return NULL;

	ImageDesc* cur = GetImageByDay(year, month, day);
	while (cur && --index >= 0) cur = cur->next;

	return cur;
}

int MediaLibraryService::GetYearCount()
{
	if (state != Done) return 0;
	if (images.empty()) return 0;

	int count = 1;

	ImageDesc* cur = images[0].next_by_year;
	while (cur)
	{
		++count;
		cur = cur->next_by_year;
	}

	return count;
}

int MediaLibraryService::GetMonthCount(int year)
{
	if (state != Done) return 0;
	if (images.empty()) return 0;

	ImageDesc* cur = GetImageByYear(year);
	if (!cur) return 0;

	int count = 1;

	cur = cur->next_by_month;
	while (cur)
	{
		++count;
		cur = cur->next_by_month;
	}

	return count;
}

int MediaLibraryService::GetDayCount(int year, int month)
{
	if (state != Done) return 0;
	if (images.empty()) return 0;

	ImageDesc* cur = GetImageByMonth(year, month);
	if (!cur) return 0;

	int count = 1;

	cur = cur->next_by_day;
	while (cur)
	{
		++count;
		cur = cur->next_by_day;
	}

	return count;
}

int MediaLibraryService::GetImageCount(int year, int month, int day)
{
	if (state != Done) return 0;
	if (images.empty()) return 0;

	ImageDesc* cur = GetImageByDay(year, month, day);
	if (!cur) return 0;

	int count = 1;

	cur = cur->next;
	while (cur)
	{
		++count;
		cur = cur->next;
	}

	return count;
}

int MediaLibraryService::GetYearByIndex(int index)
{
	ImageDesc* img = GetImageByYear(index);
	if (img) return img->creation_date.GetYear();

	return 0;
}

int MediaLibraryService::GetMonthByIndex(int year, int index)
{
	ImageDesc* img = GetImageByMonth(year, index);
	if (img) return img->creation_date.GetMonth();

	return 0;
}

int MediaLibraryService::GetDayByIndex(int year, int month, int index)
{
	ImageDesc* img = GetImageByDay(year, month, index);
	if (img) return img->creation_date.GetDay();

	return 0;
}

int MediaLibraryService::GetIndexByYear(int year)
{
	int index = 0;
	if (!images.size())
		return -1;
	ImageDesc* cur = &images[0];
	while (cur && cur->creation_date.GetYear() != year)
	{
		++index;
		cur = cur->next_by_year;
	}

	return (cur ? cur->index : -1);
}

int MediaLibraryService::GetIndexByMonth(int year, int month)
{
	int index = 0;

	ImageDesc* cur = GetImageByYear(year);
	while (cur && cur->creation_date.GetMonth() != month)
	{
		++index;
		cur = cur->next_by_month;
	}

	return (cur ? cur->index : -1);
}

int MediaLibraryService::GetIndexByDay(int year, int month, int day)
{
	int index = 0;

	ImageDesc* cur = GetImageByMonth(year, month);
	while (cur && cur->creation_date.GetDay() != day)
	{
		++index;
		cur = cur->next_by_day;
	}

	return (cur ? cur->index : -1);
}

int MediaLibraryService::GetImageFullNameByYear(int year, std::string& image_full_name)
{
	if (state != Done) return -1;
	if (images.empty()) return -1;

	ImageDesc* cur = GetImageByYear(year);

	if (cur)
	{
		image_full_name.assign(cur->full_name);
		return cur->index;
	}

	return -1;
}

int MediaLibraryService::GetImageFullNameByMonth(int year, int month, std::string& image_full_name)
{
	if (state != Done) return -1;
	if (images.empty()) return -1;

	ImageDesc* cur = GetImageByMonth(year, month);

	if (cur)
	{
		image_full_name.assign(cur->full_name);
		return cur->index;
	}

	return -1;
}

int MediaLibraryService::GetImageFullNameByDay(int year, int month, int day, std::string& image_full_name)
{
	if (state != Done) return -1;
	if (images.empty()) return -1;

	ImageDesc* cur = GetImageByDay(year, month, day);

	if (cur)
	{
		image_full_name.assign(cur->full_name);
		return cur->index;
	}

	return -1;
}

int MediaLibraryService::GetImageFullName(int year, int month, int day, int index, std::string& image_full_name)
{
	if (state != Done) return -1;
	if (images.empty()) return -1;

	ImageDesc* cur = GetImage(year, month, day, index);

	if (cur)
	{
		image_full_name.assign(cur->full_name);
		return cur->index;
	}

	return -1;
}

#ifdef ANDROID
void MediaLibraryService::SetScanApplicationResult(const char* result_str)
{
	tinyxml2::XMLDocument doc;
	doc.Parse(result_str);

	tinyxml2::XMLElement* root = doc.RootElement();
	if (!root)
	{
		core->TraceTo("MediaLibrary", "Cannot parse app XML");
		return;
	}

	apps.clear();

	tinyxml2::XMLElement* app = root->FirstChildElement("a");
	while (app)
	{
		AppDesc app_desc;

		String::Utf8toUtf16(app_desc.full_name, app->FirstChildElement("t")->GetText());
		app_desc.icon_full_name.assign(app->FirstChildElement("i")->GetText());
		app_desc.app_id.assign(app->FirstChildElement("p")->GetText());

		apps.push_back(app_desc);

		app = app->NextSiblingElement("a");
	}

	scanning_apps_done = true;
}
#endif

int MediaLibraryService::GetApplicationCount()
{
	return apps.size();
}

bool MediaLibraryService::GetApplicationId(int index, std::string& app_id)
{
	if (index < 0 || index >= apps.size()) return false;
	app_id.assign(apps[index].app_id);
	return true;
}

bool MediaLibraryService::GetApplicationName(int index, std::wstring& app_name)
{
	if (index < 0 || index >= apps.size()) return false;
	app_name.assign(apps[index].full_name);
	return true;
}

bool MediaLibraryService::GetApplicationIcon(int index, std::string& icon_name)
{
	if (index < 0 || index >= apps.size()) return false;
	icon_name.assign(apps[index].icon_full_name);
	return true;
}

void MediaLibraryService::ReqestWorker::Prepare()
{
}

void MediaLibraryService::ReqestWorker::Loop()
{
	owner->Scan(kImageDir, 0);
}