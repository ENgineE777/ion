#include "Core/Core.h"

#include "Common/libgif/gif_lib.h"
#include "Common/libpng/lodepng.h"
#include "Core/Files/File.h"

//#define STBI_NO_JPEG
#define STBI_NO_PNG
#define STBI_NO_BMP
#define STBI_NO_PSD
//	STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM


#define STB_IMAGE_IMPLEMENTATION
#include "Common/stb_image.h"

int GifInputFunc(GifFileType* gifFile, GifByteType* buffer, int len)
{
	memcpy(buffer, gifFile->UserData, len);

	byte* ptr = (byte*)gifFile->UserData;
	ptr += len;
	gifFile->UserData = ptr;	

	return len;
}

struct BitmapHeader
{
	word bfType; 
	dword bfSize; 
	word bfReserved1; 
	word bfReserved2; 
    dword bfOffBits;
}; 

struct BitmapInfoHeader
{
	dword biSize;
	int  biWidth;
	int  biHeight;
	word  biPlanes;
	word  biBitCount;
	dword biCompression;
	dword biSizeImage;
	word  biXPelsPerMeter;
	word  biYPelsPerMeter;
	dword biClrUsed;
	dword biClrImportant;
};

RenderService::TaskManager::TaskManager(): task_groups(_FL_)
{
	phase_locker.Init();
}

void RenderService::TaskManager::AddMainTaskGroup(const char* phase_name, int level)
{	
	phase_locker.Lock();

	int index = -1;	
	
	for (int i=0;i<main_task_groups.size();i++)
	{
		if (level<main_task_groups[i].level)
		{
			index = i;
			break;
		}
	}

	main_task_groups.push_back(MainTaskGroup());
	MainTaskGroup* group = &main_task_groups[main_task_groups.size()-1];
	
	String::Copy(group->name, 32, phase_name);
	group->level = level;

	if (index != -1)
	{
		for (int i=main_task_groups.size()-1;i>index;i--)
		{
			MainTaskGroup tmp = main_task_groups[i];
			main_task_groups[i] = main_task_groups[i-1];
			main_task_groups[i-1] = tmp;
		}
	}	

	phase_locker.UnLock();
}

void RenderService::TaskManager::Execute()
{		
	phase_locker.Lock();

	for (int i=0;i<main_task_groups.size();i++)
	{				
		ExecutePhase(main_task_groups[i].name);				
	}	

	phase_locker.UnLock();
}

void RenderService::TaskManager::ExecutePhase(const char* group_name)
{	
	float dt = core->GetDeltaTime();

	TaskList* task_list = task_groups.Get(group_name);
	
	if (task_list)
	{		
		for (int j=0;j<task_list->list.size();j++)
		{
			if (task_list->list[j].owner->IsTaskHoldersOwnerActive()) (task_list->list[j].entity->*task_list->list[j].call)(dt, task_list->list[j].level);			
		}
	}	
}

void RenderService::TaskManager::AddTask(const char* group_name, TaskHolder* entity, TaskHoldersOwner* taskHolderOwner, TaskHolder::Task call, int level)
{	
	phase_locker.Lock();

	TaskList* task_group = task_groups.Get(group_name);
	
	if (!task_group)
	{
		task_group = task_groups.Add(group_name);		
	}
	
	task_group->list.push_back(Task());
	
	Task* elem = &task_group->list[task_group->list.size()-1];

	elem->entity = entity;
	elem->owner = taskHolderOwner;
	elem->call = call;
	elem->level = level;

	int index = task_group->list.size()-1;

	while (index>0 && task_group->list[index-1].level>task_group->list[index].level)
	{	
		Task tmp = task_group->list[index];
		task_group->list[index] = task_group->list[index-1];
		task_group->list[index-1] = tmp;

		index--;
	}	

	phase_locker.UnLock();
}

void RenderService::TaskManager::DelTask(const char* group_name, TaskHolder* entity)
{	
	phase_locker.Lock();

	TaskList* task_group = task_groups.Get(group_name);
	
	if (task_group)
	{
		for (int i=0;i<task_group->list.size();i++)
		{
			if (entity == task_group->list[i].entity)
			{
				task_group->list.erase(task_group->list.begin() + i);				
				break;
			}
		}
	}

	phase_locker.UnLock();
}

void RenderService::TaskManager::DelAllTasks(TaskHolder* entity)
{	
	phase_locker.Lock();

	for (int j=0;j<task_groups.Size();j++)
	{
		TaskList* task_group = task_groups.Get(j);
	
		if (task_group)
		{
			for (int i=0;i<task_group->list.size();i++)
			{
				if (entity == task_group->list[i].entity)
				{
					task_group->list.erase(task_group->list.begin() + i);				
					break;
				}
			}
		}
	}

	phase_locker.UnLock();
}

void RenderService::TaskManager::Release()
{
	for (int i=0;i<task_groups.Size();i++)
	{
		task_groups.Get(i)->list.clear();
	}

	task_groups.Clear();
}

RenderService::StateManager::StateManager()
{
}

void RenderService::StateManager::SetDefaultStates()
{
	SetCulling(RenderService::cullCCW);	    

#if defined (PC)
	SetStencilTestFunc(RenderService::cmpAlways, 0, 0xff);
	SetStencilWriteMask(0xff);
#endif

	SetDepthTest(1);
	SetDepthWriting(1);
	SetDepthFunc(RenderService::cmpLessEqual);
	SetAlphaBlend(0);
	SetAlphaTest(0);
	SetStencilTest(0);
	SetColorWrite(1,1,1,1);
	SetClipPlane(0, 0);
	SetAlphaTestFunc(RenderService::cmpGreater, 0x60);
}

RenderService::DisplayManager::DisplayManager()
{
	width = 800;
	height = 600;
	aspectRatio = 1.0f;

	ref_width = 1024.0f;
	ref_height = 768.0f;

	numModes = 0;	
	cur_mode = -1;
	screen_dpi = 300;
}

int RenderService::DisplayManager::GetWidth()
{
		return width;
}

int RenderService::DisplayManager::GetHeight()
{
	return height;
}

float RenderService::DisplayManager::GetReferenceWidth()
{
	return ref_width;
}

float RenderService::DisplayManager::GetReferenceHeight()
{	
	return ref_height;
}

void RenderService::DisplayManager::SetReferenceHeight(int hgt)
{
	ref_height = hgt;
	CalcReferenceWidth();
}

void RenderService::DisplayManager::CalcReferenceWidth()
{
	float aspect = 1.0f / ((float)height / (float)width);	

	float h_mul_ScreenToRef = 1.0f / (float)width * ref_height * aspect;

	ref_width = (float)width * h_mul_ScreenToRef;	
}

float RenderService::DisplayManager::GetAspectRatio()
{
	return aspectRatio;
}

void RenderService::DisplayManager::SetView(const Matrix& mat)
{
	view = mat;
	inv_view = mat;
	inv_view.Inverse();

	UpdateFrustum();
}

void RenderService::DisplayManager::SetProj(const Matrix& mat)
{
	proj = mat;
	UpdateFrustum();
}

void RenderService::DisplayManager::GetCamPos(Vector& pos)
{
	pos = inv_view.Pos();
}

void RenderService::DisplayManager::GetView(Matrix& mat)
{
	mat = view;
}

void RenderService::DisplayManager::GetInvView(Matrix& mat)
{
	mat = inv_view;
}

void RenderService::DisplayManager::GetProj(Matrix& mat)
{
	mat = proj;
}

void RenderService::DisplayManager::GetViewProj(Matrix& mat)
{
	mat = view_proj;
}

int RenderService::DisplayManager::GetVideoModeCount()
{
	return numModes;
}

RenderService::VideoMode RenderService::DisplayManager::GetVideoMode(int index)
{
	static VideoMode mode;

	mode.width = 0;
	mode.height = 0;

	if (index<0 || index>=numModes) return mode;

	mode.width = modes[index].width;
	mode.height = modes[index].height;

	return mode;
}

void RenderService::DisplayManager::ApplyVideoMode(VideoMode mode)
{
	CalcReferenceWidth();
}
		
bool RenderService::DisplayManager::SphereInFrustum( Vector pos, float radius )
{
	return ::SphereInFrustum(frustum, pos, radius);	
}

bool RenderService::DisplayManager::BoxInFrustum( Vector* box )
{
	return ::BoxInFrustum(frustum, box);	
}

Vector RenderService::DisplayManager::TransformToScreen(const Vector& pos, int type)
{
	Vector pre_ps = pos * view;
	Vector4 ps2 = view_proj.MulVertex4(pos);	
	Vector ps;
	ps.Set(ps2.x/ps2.w,ps2.y/ps2.w,ps2.z);

	if (type == 0)
	{
		ps.x =  ps.x;
		ps.y =  ps.y;
		ps.z = pre_ps.z;
	}
	else
	if (type == 1 || type == 2)
	{
		ps.x =  0.5f + ps.x*0.5f;
		ps.y =  0.5f - ps.y*0.5f;
		ps.z = pre_ps.z;

		if (type == 2)
		{
			ps.x *= GetWidth();
			ps.y *= GetHeight();
		}
	}

	return ps;
}

void RenderService::DisplayManager::TransformScreenToSpace(const Vector2& pos, float depth, Vector& screen, Vector& end_ray)
{
	Vector v;
	v.x =  ( 2.0f * pos.x - 1 ) / proj._11;
	v.y = -( 2.0f * pos.y - 1 ) / proj._22;
	v.z = 1.0f;	
	
	Vector camPos;
	camPos.x = inv_view._41;
	camPos.y = inv_view._42;
	camPos.z = inv_view._43;


	Vector dir;
	dir.x = v.x * inv_view._11 + v.y * inv_view._21 + v.z * inv_view._31;
	dir.y = v.x * inv_view._12 + v.y * inv_view._22 + v.z * inv_view._32;
	dir.z = v.x * inv_view._13 + v.y * inv_view._23 + v.z * inv_view._33;			
	screen = camPos + dir;
	dir.Normalize();

	//float zNear = -proj.m[3][2]/proj.m[2][2];
	end_ray = camPos + dir * depth;
	//screen = screen + dir * zNear;
}

void RenderService::DisplayManager::SetScreenDPI(int dpi)
{
	screen_dpi = dpi;
}

int RenderService::DisplayManager::GetScreenDPI()
{
	return screen_dpi;
}

void RenderService::DisplayManager::UpdateFrustum()
{	
	view_proj = view * proj;
	::UpdateFrustum(frustum, view_proj);
}

RenderService::DebugManager::DebugManager()
{
}

void RenderService::DebugManager::Init()
{
	debugFont.Init();
	debugSprites.Init();
    debugLines.Init();
	debugLines2D.Init();
	debugSpheres.Init();	
}

void RenderService::DebugManager::PrintText(float x, float y, ElemAlign corner, const Color& color, const char* format, ...)
{	
	char buf[1024];

	buf[0] = 0;

	va_list	args;

	va_start( args, format );
	vsnprintf( buf, sizeof( buf ), format, args );	
	va_end( args );	

	debugFont.Print(x,y,corner, color,buf);
}

void RenderService::DebugManager::PrintText3D(Vector pos, float dist, const Color& color, const char* format, ...)
{	
	char buf[1024];

	buf[0] = 0;

	va_list	args;

	va_start( args, format );
	vsnprintf( buf, sizeof( buf ), format, args );	
	va_end( args );	

	debugFont.Print3D(pos,dist,color,buf);
}

void RenderService::DebugManager::DrawSprite(Texture* texture, const Vector2& pos, const Vector2& size,
							   ElemAlign corner, const Color& color, float angle, 
							   const Vector2& uv1, const Vector2& uv2)
{	
	debugSprites.AddSprite(texture, pos, size, corner, color, angle, uv1, uv2);
}

void RenderService::DebugManager::DrawLine(const Vector& p1, const Color& color1, const Vector& p2, const Color& color2, bool noZ)
{	
	debugLines.AddLine(p1, color1, p2, color2, noZ);
}

void RenderService::DebugManager::DrawLine2D(const Vector2& p1, const Color& color1, const Vector2& p2, const Color& color2)
{
	debugLines2D.AddLine(p1, color1, p2, color2);
}

void RenderService::DebugManager::DrawCircle(int axis, Matrix& mat, float radius, Color color, Color back_color)
{	
	///FIX ME!!!!

	/*
	Vector tr = mat.Pos();	
	Vector trans = tr * view;	

	float last_dx = -radius * 2;
	float last_dz = -radius * 2;
	int nums = 32;

	for (int i = 0; i < nums+1; i++)
	{
		float dx = (float)sinf(2.0f * 3.14f / (float)nums * (float)i) * radius;
		float dz = (float)cosf(2.0f * 3.14f / (float)nums * (float)i) * radius;

		if (last_dx > -radius * 1.5f)
		{
			Vector pos;
			Vector pos2;

			if (axis == 0)
			{
				pos.Set(0.0f, last_dx, last_dz);
				pos2.Set(0.0f, dx, dz);
			}
			else
			if (axis == 1)
			{
				pos.Set(last_dx, 0.0f, last_dz);
				pos2.Set(dx, 0.0f, dz);
			}
			else
			{
				pos.Set(last_dx, last_dz, 0.0f);
				pos2.Set(dx, dz, 0.0f);
			}
			
			pos = mat.MulNormal(pos);
			pos2 = mat.MulNormal(pos2);

			pos += tr;
			pos2 += tr;

				Vector pos_post = pos * view;
				Vector pos2_post = pos2 * view;

				if (pos_post.z > trans.z && pos2_post.z > trans.z)
				{
					DrawLine(pos, back_color, pos2, back_color, true);
				}
				else
				{
					DrawLine(pos, color, pos2, color, true);
				}
		}

		last_dx = dx;
		last_dz = dz;
	}*/
}

void RenderService::DebugManager::DrawBox(const Vector& size, const Matrix transform, const Color& color, bool noZ)
{	
	Vector unitBox[8];

	unitBox[0] = Vector(-1, -1, -1);
    unitBox[1] = Vector( 1, -1, -1);
    unitBox[2] = Vector(-1,  1, -1);
    unitBox[3] = Vector( 1,  1, -1);
    unitBox[4] = Vector(-1, -1, 1);
    unitBox[5] = Vector( 1, -1, 1);
    unitBox[6] = Vector(-1,  1, 1);
    unitBox[7] = Vector( 1,  1, 1);

	Vector box[8];

    for(int i = 0; i < 8; i++)
        box[i] = 0.5f * size * unitBox[i] * transform;

	DrawLine(box[0],color,box[1],color, noZ);
    DrawLine(box[4],color,box[5],color, noZ);
    DrawLine(box[2],color,box[3],color, noZ);
    DrawLine(box[6],color,box[7],color, noZ);

    DrawLine(box[0],color,box[4],color, noZ);
    DrawLine(box[1],color,box[5],color, noZ);
    DrawLine(box[2],color,box[6],color, noZ);
    DrawLine(box[3],color,box[7],color, noZ);

    DrawLine(box[0],color,box[2],color, noZ);
    DrawLine(box[1],color,box[3],color, noZ);
    DrawLine(box[4],color,box[6],color, noZ);
    DrawLine(box[5],color,box[7],color, noZ);
}

void RenderService::DebugManager::DrawSolidBox(const Vector& size, const Matrix transform, const Color& color)
{	
}

void RenderService::DebugManager::DrawSphere(const Vector& pos, float radius, const Color& color)
{	
	debugSpheres.AddSphere(pos, radius, color);
}

void RenderService::DebugManager::Release()
{
	debugFont.Release();
	debugSprites.Release();
	debugLines.Release();
	debugLines2D.Release();
	debugSprites.Release();
	debugSpheres.Release();	
}

bool RenderService::TextureData::GetInfo()
{
	format = tex_unknown;

	byte header[32];

	IFile* fl = core->Files()->OpenFile(fl_name, IFile::read);

	if (!fl)
	{
		return false;
	}

	if (fl->Read(&header, 32) != 32)
	{
		fl->Release();
		return false;
	}

	bool res = GetInfo(&header[0], fl);

	fl->Release();
	
	return res;
}

bool RenderService::TextureData::GetInfo(byte* header, IFile* fl)
{
	// BMP
	{
		byte* ptr = &header[0];

		BitmapHeader bmp_header;
		bmp_header.bfType = *((word*)ptr);
		ptr += 14;    
		
		BitmapInfoHeader* bmp_info = (BitmapInfoHeader*)ptr;

		if (bmp_header.bfType == 'MB')
		{        			
			width = bmp_info->biWidth;
			height = bmp_info->biHeight;  
			lods = 1;

			format = tex_bitmap;

			return true;
		}
	}

	// PNG
	{
		if (header[0] == 137 && header[1] == 80 && header[2] == 78 && header[3] == 71 &&
			header[4] == 13 && header[5] == 10 && header[6] == 26 && header[7] == 10)
		{
			width = *((int*)&header[16]);
			SwapInt(width);

			height = *((int*)&header[20]);
			SwapInt(height);

			lods = 1;
		
			format = tex_png;

			return true;
		}		
    }

	// GIFF
	{
		if (header[0] == 'G' && header[1] == 'I' && header[2] == 'F')
		{
			width  = header[7] * 256 + header[6];
			height = header[9] * 256 + header[8];

			format = tex_gif;

			return true;
		}
	}

	// TGA
	{
		if ((header[2] == 2 || header[2] == 10) &&
			(header[16] == 24 || header[16] == 32))
		{
			width  = header[13] * 256 + header[12];
			height = header[15] * 256 + header[14];

			format = tex_tga;

			return true;
		}
	}

	// JPEG
	{
		if (header[0] == 0xFF && header[1] == 0xD8)
		{						
			#define MOTOSHORT(p) ((*(p))<<8) + *(p+1)

			unsigned char cBuf[32];
			int iBytes, i, j, iMarker, iFilesize;
			unsigned char ucSubSample;
			int iBpp, iHeight, iWidth;

			iFilesize = fl->GetSize();
			fl->Seek(0, IFile::seek_set);
			fl->Read(&cBuf, 32);

			i = j = 2; /* Start at offset of first marker */
			iMarker = 0; /* Search for SOF (start of frame) marker */
			
			while (i < 32 && iMarker != 0xffc0 && j < iFilesize)
            {
				iMarker = MOTOSHORT(&cBuf[i]) & 0xfffc;

				if (iMarker < 0xff00) // invalid marker, could be generated by "Arles Image Web Page Creator" or Accusoft
				{
					i += 2;
					continue; // skip 2 bytes and try to resync
				}

				if (iMarker == 0xffc0) // the one we're looking for
				break;

				j += 2 + MOTOSHORT(&cBuf[i+2]); /* Skip to next marker */

				if (j < iFilesize) // need to read more
				{		
					fl->Seek(j, IFile::seek_set);
					fl->Read(&cBuf, 32);               
					i = 0;
				}
				else
				{// error, abort
					break;
				}
            } // while
         
			if (iMarker != 0xffc0)
			{				
				return false;
			}
	        else
            {
				iBpp = cBuf[i+4]; // bits per sample
				height = MOTOSHORT(&cBuf[i+5]);
				width = MOTOSHORT(&cBuf[i+7]);
				iBpp = iBpp * cBuf[i+9]; /* Bpp = number of components * bits per sample */
				ucSubSample = cBuf[i+11];				
            }

			format = tex_jpeg;

			return true;		
		}
	}

	return false;
}

bool RenderService::TextureData::Load()
{
	file = core->Files()->LoadFile(fl_name);
    
    if (!file) return false;
	
    bool res = LoadImpl();

    file->Release();

	return res;
}

bool RenderService::TextureData::LoadImpl()
{	
	if (format == tex_bitmap)
	{
		return LoadBITMAP();
	}
	else
	if (format == tex_png)
	{
		return LoadPNG();
	}    
	else	        
	if (format == tex_gif)
    {
        return LoadGIF();
    }	    	
	else
	if (format == tex_tga || format == tex_jpeg)
    {
        return LoadSTB();
    }    

	return false;
}

bool RenderService::TextureData::LoadBITMAP()
{
	byte* ptr = file->GetBuffer();

    BitmapHeader header;
    header.bfType = *((word*)ptr);
    ptr += 2;

    header.bfSize = *((dword*)ptr);
    ptr += 4;

    header.bfReserved1 = *((word*)ptr);
    ptr += 2;
            
    header.bfReserved2 = *((word*)ptr);
    ptr += 2;

    header.bfOffBits = *((dword*)ptr);
    ptr += 4;
		
    BitmapInfoHeader* info = (BitmapInfoHeader*)ptr;

    if (header.bfType == 'MB')
    {
        ptr = file->GetBuffer() + header.bfOffBits;
			
		width = info->biWidth;
		height = info->biHeight;        
        
		if (info->biBitCount == 24)
		{
			bytesPerPixel = 3;			
		}
		else
		{
			bytesPerPixel = 4;            
		}

		int len = bytesPerPixel * width * height;
		data = (byte*)malloc(len);

#ifdef PC
		for(int j=0; j < height;j++)
		{
			byte* rows = (byte*)&data[(height - j - 1) * width * bytesPerPixel];				

			for(int i=0; i < width;i++)
			{
				byte* color = &ptr[(i + j * width) * bytesPerPixel];
			
				rows[i*bytesPerPixel + 0] = color[2];		
				rows[i*bytesPerPixel + 1] = color[1];
				rows[i*bytesPerPixel + 2] = color[0];

				if (bytesPerPixel == 4)
				{
					rows[i*bytesPerPixel + 3] = color[3];
				}
			}
		}
#else
		memcpy(data, ptr, len);
#endif

        return true;
    }

	return false;
}

bool RenderService::TextureData::LoadPNG()
{
	byte* ptr = file->GetBuffer();

	if (ptr[1] == 'P' && ptr[2] == 'N' && ptr[3] == 'G')
	{
		unsigned error;		 

		error = lodepng_decode32(&data, (unsigned int*)&width, (unsigned int*)&height, ptr, file->GetSize());

		if (error)
		{
			core->TraceTo("Render", "error in %s %u: %s", fl_name, error, lodepng_error_text(error));

			return false;
		}

		bytesPerPixel = 4;		 		 

		return true;
	}

	return false;
}

bool RenderService::TextureData::LoadGIF()
{
	int interlacedOffset[] = { 0, 4, 2, 1 };
    int interlacedJumps[] = { 8, 8, 4, 2 };
	ColorMapObject* colorMap;
	GifColorType* colorMapEntry;	

    GifRecordType recordType;    
	GifFileType* gifFile = DGifOpen(file->GetBuffer(), GifInputFunc);

	if (!gifFile)
	{		
		return false;
	}
	
	width = gifFile->SWidth;
	height = gifFile->SHeight;

	int size = width * height;
    byte* buffer = (byte*)malloc(size);
	memset(buffer, gifFile->SBackGroundColor, size);	

	bool imgae_loaded = false;
	
	do
	{		
		if (DGifGetRecordType(gifFile, &recordType) == GIF_ERROR)
		{
			PrintGifError();
			break;
			//exit(EXIT_FAILURE);
		}

		switch (recordType)
		{
			case IMAGE_DESC_RECORD_TYPE:
			{
				if (DGifGetImageDesc(gifFile) == GIF_ERROR)
				{
					PrintGifError();
					//exit(EXIT_FAILURE);
				}				

				int row = gifFile->Image.Top; /* Image Position relative to Screen. */
				int col = gifFile->Image.Left;
				int img_width = gifFile->Image.Width;
				int img_height = gifFile->Image.Height;

				if (gifFile->Image.Left + gifFile->Image.Width > width ||
					gifFile->Image.Top + gifFile->Image.Height > height)
				{
					//fprintf(stderr, "Image %d is not confined to screen dimension, aborted.\n", GifFile->ImageCount);
					//exit(EXIT_FAILURE);
				}
            
				if (gifFile->Image.Interlace)
				{					
					for (int i = 0; i < 4; i++)
					{				
						for (int j = row + interlacedOffset[i]; j < row + height; j += interlacedJumps[i])
						{										    
							if (DGifGetLine(gifFile, &buffer[j * width + col], img_width) == GIF_ERROR)
							{
								PrintGifError();
								//exit(EXIT_FAILURE);
							}
						}
					}
				}
				else
				{
					for (int i = 0; i < img_height; i++)
					{												
						if (DGifGetLine(gifFile, &buffer[(row + i) * width + col], img_width) == GIF_ERROR)
						{
							break;
							//PrintGifError();
							//exit(EXIT_FAILURE);
						}
					}
				}

				if (gifFile->Image.ColorMap)
				{
					colorMap = gifFile->Image.ColorMap;
				}
				else
				{
					colorMap = gifFile->SColorMap;
				}

				imgae_loaded = true;
			}
			break;
			case EXTENSION_RECORD_TYPE:
			{
				GifByteType  *Extension;
				int extCode;

				// Skip any extension blocks in file except comments:
				if (DGifGetExtension(gifFile, &extCode, &Extension) == GIF_ERROR)
				{
					PrintGifError();
					//exit(EXIT_FAILURE);
				}
							
				while (Extension != NULL)
				{
					/*if(ExtCode == COMMENT_EXT_FUNC_CODE)
					{
						Extension[Extension[0]+1] = '\000';   // Convert gif's pascal-like string
						Comment = (char*) realloc(Comment, strlen(Comment) + Extension[0] + 1);
						strcat(Comment, (char*)Extension+1);
					}*/
				
					if (DGifGetExtensionNext(gifFile, &Extension) == GIF_ERROR)
					{
						PrintGifError();
						//exit(EXIT_FAILURE);
					}
				}
			}
			break;

			case TERMINATE_RECORD_TYPE:
			break;
			default:		    // Should be traps by DGifGetRecordType.
			break;
		}

		if (imgae_loaded) break;
    }
	while (recordType != TERMINATE_RECORD_TYPE);
			
	if (!imgae_loaded)
	{
		DGifCloseFile(gifFile);
		free(buffer);		

		return false;
	}

	bytesPerPixel = 4;

	data = (byte*)malloc(size * bytesPerPixel);
	
	for(int j=0; j < height;j++)
	{
		byte* rows = (byte*)&data[j * width * bytesPerPixel];				

		for(int i=0; i < width;i++)
		{
			colorMapEntry = &colorMap->Colors[buffer[(i + j * width) * 1 + 0]];
			
			rows[i*bytesPerPixel + 0] = colorMapEntry->Red;		
			rows[i*bytesPerPixel + 1] = colorMapEntry->Green;
			rows[i*bytesPerPixel + 2] = colorMapEntry->Blue;
            rows[i*bytesPerPixel + 3] = 255;
		}
	}
	
	DGifCloseFile(gifFile);
	free(buffer);	

	return true;
}

bool RenderService::TextureData::LoadSTB()
{
	int bytes;
	bytesPerPixel = 4;
	data = stbi_load_from_memory(file->GetBuffer(), file->GetSize(), &width, &height, &bytes, STBI_rgb_alpha);

	if (!data)
	{			
		return false;
	}

	return true;
}

void RenderService::TextureLoader::Prepare()
{
}

void RenderService::TextureLoader::Loop()
{	
	while (!NeedExit())
	{
        TextureData* data = (TextureData*)&quene_texture[quene_class_size * cur_data];
        		
		if (data->state == tls_waiting_processing)
		{
			data->state = tls_processing;

			if (!data->need_skip)
			{
				data->Load();
			}			

			data->state = tls_ready_to_grab;

			cur_data++;

			if (cur_data >= 512)
			{
				cur_data = 0;
			}
		}
		else
		{
			ThreadWorker::Sleep(100);
		}		
	}
}

int RenderService::quene_class_size = 0;
byte* RenderService::quene_texture = null;
int RenderService::texture_count = 0;
int RenderService::texture_memory = 0;

RenderService::RenderService() : textures(_FL_), vshaders(_FL_), pshaders(_FL_), techniques(_FL_)
{
	cur_depth = -1;	

	draw_calls = 0;
	poly_count = 0;

	cur_tech = 0;	

	null_render = false;

	need_reset = false;		

	use_dummy_render = false;

	last_quene_texture = 0;
	cur_quene_texture = 0;
}

bool RenderService::Init(int wgt, int hgt)
{
    task_manager.AddMainTaskGroup("camera", -5);	

    task_manager.AddMainTaskGroup("pre_pass", -2);
    task_manager.AddMainTaskGroup("post_process", -1);
    task_manager.AddMainTaskGroup("geometry", 0);
    task_manager.AddMainTaskGroup("alpha", 2);
    task_manager.AddMainTaskGroup("fx2", 4);	
		
    task_manager.AddMainTaskGroup("debug_render", 8);
	task_manager.AddMainTaskGroup("debug_sphere", 9);
    task_manager.AddMainTaskGroup("gui", 400);    
    
    task_manager.AddMainTaskGroup("debug_geom", 500);	
	task_manager.AddMainTaskGroup("debug_sprite", 510);			
	task_manager.AddMainTaskGroup("debug_lines2d", 515);
    task_manager.AddMainTaskGroup("debug_font", 520);    

	debug_manager.Init();

	State()->SetDefaultStates();

	core->AddTask(this, this, (TaskHolder::Task)&RenderService::Work, -320, -1.f);

	texture_loader.Start(ThreadWorker::normal);

	return true;
}

int RenderService::GetDrawCalls()
{
	return draw_calls;
}

int RenderService::GetPrimCount()
{
	return poly_count;
}

bool RenderService::Reset()
{
	return true;
}

void RenderService::Begin()
{
	if (use_dummy_render) return;	

	draw_calls = 0;
	poly_count = 0;

	State()->SetDefaultStates();	
}

void RenderService::End()
{
	if (use_dummy_render) return;	
}

void RenderService::EnableNullRender(bool act)
{
	null_render = act;
}

bool RenderService::IsNullRender()
{
	return null_render;
}

TextureInstance* RenderService::LoadTexture(const char* name, bool async, const char* file, int line)
{	
#ifdef EDITOR
	async = en_core.IsEditorMode();
#endif

#if defined(PC) || defined(OSUNIX)	
	en_core.EN_Files()->RegFile(name);
#endif

    if (!name[0]) return null;
    
	char nm[1024];
	String::Copy(nm, 1024, name);
    
    if (nm[0] != '/' || nm[1] != 'v')
    {
        String::LowerCase(nm);
    }

	TextureRes* res = textures.Get(nm);

	if (!res)
	{
		res = new TextureRes();
		
		if (!res->Load(nm, async, file, line))
		{
			res->Release();

			return NULL;
		}

		textures.Add(nm, res);
	}
	else
	{
		//core->Trace("%s already loaded", name);
	}

	return res->CreateReference(file, line);
}

VertexShader* RenderService::CreateVertexShader(const char* name, const char* file, int line)
{
	char nm[256];
	String::Copy(nm, 256, name);
	String::LowerCase(nm);

	VertexShader* shader = null;//vshaders.Get(nm);

	if (!shader)
	{						
		shader = CreateVertexShaderImpl(nm);
		shader->Load();		

		vshaders.Add(nm, shader);		
	}

	return shader;
}

PixelShader* RenderService::CreatePixelShader(const char* name, const char* file, int line)
{
	char nm[256];
	String::Copy(nm, 256, name);
	String::LowerCase(nm);

	PixelShader* shader = null;//pshaders.Get(nm);

	if (!shader)
	{		
		shader = CreatePixelShaderImpl(nm);
		shader->Load();		

		pshaders.Add(nm, shader);		
	}

	return shader;
}

RenderTechnique* RenderService::CreateRenderTechnique(const char* name)
{
	RenderTechnique* tech = techniques.Get(name);
	
	if (!tech)
	{
		tech = RTDeclBase::Create(name);

		if (!tech) return null;

		tech->Init();
		techniques.Add(name, tech);
	}

	return tech;
}

void RenderService::SetRenderTechnique(RenderTechnique* tech)
{
	if (use_dummy_render) return;
	
	//if (cur_tech != tech)
	{		
		if (cur_tech)
			cur_tech->PostApplyParams();

		
		cur_tech = tech;
		//cur_tech->SetPSVS();
		if(cur_tech)
			cur_tech->ApplyParams();
	}
}

void RenderService::DeleteRes(TextureRes* res)
{
	textures.Delete(res,false);
}

void RenderService::DeleteVBuffer(VertexBuffer* vb)
{
	for (int j=0;j<vbuffers.size();j++)
	{			
		if (vbuffers[j] == vb)
		{
			vbuffers.erase(vbuffers.begin() + j);
			return;
		}		
	}	
}

void RenderService::DeleteIBuffer(IndexBuffer* ib)
{
	for (int j=0;j<ibuffers.size();j++)
	{			
		if (ibuffers[j] == ib)
		{
			ibuffers.erase(ibuffers.begin() + j);
			return;
		}		
	}	
}

void RenderService::ReCreateResources()
{	
	if (use_dummy_render) return;

	for (int i=0; i < vshaders.Size(); i++)
	{
		vshaders.Get(i)->Load();
	}

	for (int i=0; i < pshaders.Size(); i++)
	{
		pshaders.Get(i)->Load();
	}

	for (int i=0; i < techniques.Size(); i++)
	{
		techniques.Get(i)->ReCreate();
	}	

	for (int i=0; i < vbuffers.size(); i++)
	{
		vbuffers[i]->ReCreate();
	}

	for (int i=0; i < ibuffers.size(); i++)
	{
		ibuffers[i]->ReCreate();
	}

	for (int i=0; i < textures.Size(); i++)
	{
		textures.Get(i)->ReCreate();
	}
}

void RenderService::SetUseDummyRender(bool dummy_rener)
{
	use_dummy_render = dummy_rener;	
}

bool RenderService::IsUseDummyRender()
{
	return use_dummy_render;
}

Texture::TextureData* RenderService::AddTextureToQuene(Texture* texture, const char* file_name)
{
	TextureData* data = (TextureData*)&quene_texture[quene_class_size * cur_quene_texture];

	data->state = tls_prepare_data;

	String::Copy(data->fl_name, 512, file_name);

	if (!data->GetInfo())
	{
		return null;
	}

	data->texture = texture;

	data->state = tls_waiting_processing;
	
	cur_quene_texture++;

	if (cur_quene_texture >= 512)
	{
		cur_quene_texture = 0;
	}

	return data;
}

void RenderService::MarkAsSkipped(Texture* texture)
{
	int index = last_quene_texture;

	while (index != cur_quene_texture)
	{		
		TextureData* data = (TextureData*)&quene_texture[quene_class_size * index];

		if (data->texture == texture)
		{
			data->need_skip = true;
			data->texture = null;
		}		

		index++;

		if (index >= 512)
		{
			index = 0;
		}
	}
}

void RenderService::Work(float dt, int level)
{   
	int counter = 0;

	while (last_quene_texture != cur_quene_texture)
	{
		if (counter > 3) break;

		TextureData* data = (TextureData*)&quene_texture[quene_class_size * last_quene_texture];

		if (data->state != tls_ready_to_grab) break;

		if (!data->need_skip)
		{
			data->texture->FlushData(data);
		}
		else
		{
			data->need_skip = false;
		}

		if (data->data)
		{			
			free(data->data);
			data->data = null;
		}

		data->state = tls_idle;

		counter++;

		last_quene_texture++;

		if (last_quene_texture >= 512)
		{
			last_quene_texture = 0;
		}
	}
}

void RenderService::GetTextureInfo(int& tex_count, int& tex_loading, int& tex_consuption)
{
	tex_count = texture_count;

	tex_loading = cur_quene_texture - last_quene_texture;

	if (last_quene_texture > cur_quene_texture)
	{
		tex_loading += 512;
	}		

	tex_consuption = texture_memory;
}

void RenderService::Release()
{
	cur_tech = null;		
		
	task_manager.Release();
	debug_manager.Release();

	texture_loader.Stop();
	delete[] quene_texture;

	textures.Clear();
	techniques.Clear();
	vshaders.Clear();
	pshaders.Clear();
	
}