
#include "Utils.h"
#include "Core/Core.h"

#include <openssl/evp.h>
#include <openssl/md5.h>

#include <openssl/hmac.h>
#include <openssl/sha.h>

#ifdef PC
#include <windows.h>
#endif

#if defined(OSUNIX) || defined(ANDROID)

#include <sys/stat.h>
#include <unistd.h>

#endif

#if defined(OSX) || defined(IOS)|| defined(OSUNIX)
#include <sys/time.h>
#endif

#include <iostream>

BoundingBox ReCalcBBox(BoundingBox box,Matrix mat)
{		
	Vector cube[8];

	cube[0] = Vector(box.maxV.x,box.maxV.y,box.maxV.z);
	cube[1] = Vector(box.minV.x,box.minV.y,box.minV.z);
	cube[2] = Vector(box.minV.x,box.maxV.y,box.maxV.z);
	cube[3] = Vector(box.maxV.x,box.minV.y,box.maxV.z);
	cube[4] = Vector(box.maxV.x,box.maxV.y,box.minV.z);
	cube[5] = Vector(box.maxV.x,box.minV.y,box.minV.z);
	cube[6] = Vector(box.minV.x,box.maxV.y,box.minV.z);
	cube[7] = Vector(box.minV.x,box.minV.y,box.maxV.z);
	
	BoundingBox outBox;

	Vector p;

	for (int i=0;i<8;i++)
	{
		p = cube[i]*mat;
	
		if (i==0||outBox.minV.x>p.x) outBox.minV.x=p.x;
		if (i==0||outBox.maxV.x<p.x) outBox.maxV.x=p.x;

		if (i==0||outBox.minV.y>p.y) outBox.minV.y=p.y;
		if (i==0||outBox.maxV.y<p.y) outBox.maxV.y=p.y;

		if (i==0||outBox.minV.z>p.z) outBox.minV.z=p.z;
		if (i==0||outBox.maxV.z<p.z) outBox.maxV.z=p.z;
	}		

	return outBox;
}
//-----------------------------------------------------------------------------
Vector pointAdd(Vector& p, Vector& q)
{
	p.x += q.x;
	p.y += q.y;
	p.z += q.z;
	
	return p;
}
//-----------------------------------------------------------------------------
Vector pointTimes(float c, Vector& p)
{
	p.x *= c;	p.y *= c;	p.z *= c;
	return p;
}
//-----------------------------------------------------------------------------
Vector makePoint(float a, float b, float c)
{
	Vector p;
	p.x = a;	p.y = b;	p.z = c;
	return p;
}
//-----------------------------------------------------------------------------   
Vector Bernstein(float u, Vector *p)
{
	Vector	a = p[0], b = p[1], c = p[2], d = p[3], r;

	a = pointTimes(powf(u,3), a);
	b = pointTimes(3*powf(u,2)*(1-u), b);
	c = pointTimes(3*u*powf((1-u),2), c);
	d = pointTimes(powf((1-u),3), d);

	Vector tmp1 = pointAdd(a, b);
	Vector tmp2 = pointAdd(c, d);
	
	r = pointAdd(tmp1, tmp2);

	return r;
}
//-----------------------------------------------------------------------------
float  SIGN(float X)
{	
	if (X>0) return (1);
	if (X<0) return (-1);
	return (0);
}
//-----------------------------------------------------------------------------
/*void UpdateFrustum( D3DMATRIX mat, TPlane *CamPlanes)
{
	// èçâëåêàåì óðàâíåíèå ïðàâîé ïëîñêîñòè
	CamPlanes[0].normal.x = mat._14 - mat._11;
	CamPlanes[0].normal.y = mat._24 - mat._21;
	CamPlanes[0].normal.z = mat._34 - mat._31;
	CamPlanes[0].dist     = mat._44 - mat._41;

	NormalizePlane( CamPlanes[0] );

	// èçâëåêàåì óðàâíåíèå ëåâîé ïëîñêîñòè
	CamPlanes[1].normal.x = mat._14 + mat._11;
	CamPlanes[1].normal.y = mat._24 + mat._21;
	CamPlanes[1].normal.z = mat._34 + mat._31;
	CamPlanes[1].dist     = mat._44 + mat._41;

	NormalizePlane( CamPlanes[1] );

	// èçâëåêàåì óðàâíåíèå íèæíåé ïëîñêîñòè
	CamPlanes[2].normal.x = mat._14 + mat._12;
	CamPlanes[2].normal.y = mat._24 + mat._22;
	CamPlanes[2].normal.z = mat._34 + mat._32;
	CamPlanes[2].dist     = mat._44 + mat._42;	

	NormalizePlane( CamPlanes[2] );

	// èçâëåêàåì óðàâíåíèå âåðõíåé ïëîñêîñòè
	CamPlanes[3].normal.x = mat._14 - mat._12;
	CamPlanes[3].normal.y = mat._24 - mat._22;
	CamPlanes[3].normal.z = mat._34 - mat._32;
	CamPlanes[3].dist     = mat._44 - mat._42;	

	NormalizePlane( CamPlanes[3] );

	// èçâëåêàåì óðàâíåíèå äàëüíåé ïëîñêîñòè
	CamPlanes[4].normal.x = mat._14 - mat._13;
	CamPlanes[4].normal.y = mat._24 - mat._23;
	CamPlanes[4].normal.z = mat._34 - mat._33;
	CamPlanes[4].dist     = mat._44 - mat._43;	

	NormalizePlane( CamPlanes[4] );

	// èçâëåêàåì óðàâíåíèå áëèæíåé ïëîñêîñòè
	CamPlanes[5].normal.x = mat._14 + mat._13;
	CamPlanes[5].normal.y = mat._24 + mat._23;
	CamPlanes[5].normal.z = mat._34 + mat._33;
	CamPlanes[5].dist     = mat._44 + mat._43;	

	NormalizePlane( CamPlanes[5] );
}
//-----------------------------------------------------------------------------
bool CubeInFrustum( point_3d *vec3, TPlane *CamPlanes )
{
	int i,j;

	for(i=0;i<6;i++)
	{
		for(j=0;j<8;j++)
		{
			float dot=CamPlanes[i].normal.x * vec3[j].x +
					  CamPlanes[i].normal.y * vec3[j].y +
					  CamPlanes[i].normal.z * vec3[j].z;

			if (dot + CamPlanes[i].dist >= 0) break;
		}

		if (j==8) return false;
	}
  
	return true;
}*/

bool IntersectTriangle( const Vector& orig,
                        const Vector& dir,
						Vector& v0,Vector& v1, Vector& v2,
                        float* dist)
{
	float u,v;
   
	// Find vectors for two edges sharing vert0
    Vector edge1 = v1 - v0;
    Vector edge2 = v2 - v0;

    // Begin calculating determinant - also used to calculate U parameter
    Vector pvec = dir ^ edge2;
    
    // If determinant is near zero, ray lies in plane of triangle
    float det = edge1 | pvec ;

    Vector tvec;
    if( det > 0 )
    {
        tvec = orig - v0;
    }
    else
    {
        tvec = v0 - orig;
        det = -det;
    }

    if( det < 0.0001f ) return false;

    // Calculate U parameter and test bounds
    u = tvec | pvec;
    if( u < 0.0f || u > det ) return false;

    // Prepare to test V parameter
    Vector qvec = tvec  ^ edge1;

    // Calculate V parameter and test bounds
    v = dir | qvec;
    if( v < 0.0f || u + v > det ) return false;

    // Calculate t, scale parameters, ray intersects triangle
    *dist = edge2 | qvec;
    float fInvDet = 1.0f / det;
    *dist *= fInvDet;
    //u *= fInvDet;
    //v *= fInvDet;

    return true;
}

bool CubeRayIntersection(const BoundingBox& box, const Vector& origin, const Vector& dir)
{
	/*float eps = 0.01f;
	box.maxV += eps;
	box.minV -= eps;

	dir = origin + dir * 1000.0f;
	
	Vector vIntersection;

	Plane plane;

	for (int i=0;i<6;i++)
	{
		switch (i)
		{
			case 0:
			{
				plane = Plane(Vector(-1,0,0), box.maxV);
			}
			break;
			case 1:
			{
				plane = Plane(Vector(0,-1,0), box.maxV);
			}
			break;
			case 2:
			{
				plane = Plane(Vector(0,0,-1), box.maxV);
			}
			break;
			case 3:
			{
				plane = Plane(Vector(1,0,0), box.minV);
			}
			break;
			case 4:
			{
				plane = Plane(Vector(0,1,0), box.minV);
			}
			break;
			case 5:
			{
				plane = Plane(Vector(0,0,1), box.minV);
			}
			break;
		}	

		if (plane.Intersection(origin, dir))
		{
			return true;
		}	
	}*/

	return false;
}



Vector getRotationDegrees(Matrix mat) 
{ 	
	float Y = -asin(mat._31); 
	//float D = Y; 
	float C = cos(Y); 
	Y /= RADIAN; // <- this was "180.000f/PI" before 
	//   (not part of the bug) 

	float rotx, roty, X, Z;

	if (fabs(C)>0.0005f) // <- C not Y 
	{ 
		rotx = mat._33 / C; 
		roty = mat._32 / C; 
		X = atan2( roty, rotx ) / RADIAN; 
		rotx = mat._11 / C; 
		roty = mat._21 / C; 
		Z = atan2( roty, rotx ) / RADIAN; 
	} 
	else 
	{ 
		X  = 0.0f; 
		rotx = mat._22; // <- no minus here 
		roty = -mat._12; // <- but here, and not (1,0) 
		Z  = atan2( roty, rotx ) / RADIAN; 
	} 

	// fix values that get below zero 
	// before it would set (!) values to 360 
	// that where above 360: 
	if (X < 0.00f) X += 360.00; 
	if (Y < 0.00f) Y += 360.00; 
	if (Z < 0.00f) Z += 360.00; 

	//if (X>0) X = 360.0f - X;
	//if (Y>0) Y = 360.0f - Y;
	//if (Z>0) Z = 360.0f - Z;

	return Vector(X,Y,Z); 
}

Matrix GetCubeMapViewMatrix( int dwFace, Vector vEyePt)
{	
	Vector vLookDir;
	Vector vUpDir;

	switch( dwFace )
	{
	case 0:
		vLookDir = Vector( 1.0f, 0.0f, 0.0f );
		vUpDir   = Vector( 0.0f, 1.0f, 0.0f );
		break;
	case 1:
		vLookDir = Vector(-1.0f, 0.0f, 0.0f );
		vUpDir   = Vector( 0.0f, 1.0f, 0.0f );
		break;
	case 2:
		vLookDir = Vector( 0.0f, 1.0f, 0.0f );
		vUpDir   = Vector( 0.0f, 0.0f,-1.0f );
		break;
	case 3:
		vLookDir = Vector( 0.0f,-1.0f, 0.0f );
		vUpDir   = Vector( 0.0f, 0.0f, 1.0f );
		break;
	case 4:
		vLookDir = Vector( 0.0f, 0.0f, 1.0f );
		vUpDir   = Vector( 0.0f, 1.0f, 0.0f );
		break;
	case 5:
		vLookDir = Vector( 0.0f, 0.0f,-1.0f );
		vUpDir   = Vector( 0.0f, 1.0f, 0.0f );
		break;
	}

	Matrix mView;
	mView.BuildView(vEyePt, vEyePt+vLookDir, vUpDir);
	
	return mView;
}

void LerpMatrixes(const Matrix mat_from,const Matrix mat_to, Matrix& mat_res, float blend)
{
	Quaternion from(mat_from);
	Quaternion to(mat_to);

	to.SLerp(from,to,blend);
	to.GetMatrix(mat_res);
}

void ExtractAlpaBetta(Vector dir,float& alpha,float& betta)
{
	dir.Normalize();

	betta=asinf(dir.y)/RADIAN;
	alpha=atanf((dir.z)/(dir.x))/RADIAN;

	if ((dir.x)<0) alpha+=180;
	if ((dir.x)>0&&(dir.z)<0) alpha+=360;
}

void SliceChangeValue(float &value, float needed, float accel, float timedelta)
{
	if (value<needed)
	{
		value+=timedelta*accel;

		if (value>needed)
		{
			value=needed;
		}
	}
	else
	{
		value-=timedelta*accel;

		if (value<needed)
		{
			value=needed;
		}
	}
}

bool ClipSprite(float& x, float& y, float& w, float& h, float& u, float& v, float& du, float& dv, float clip_x1, float clip_x2, float clip_y1, float clip_y2)
{    
	if (clip_y1 > y + h)
	{						
		return false;
	}

	if (y  > clip_y2)
	{						
		return false;
	}

	if (clip_x1 > x + w)
	{						
		return false;
	}

	if (x  > clip_x2)
	{						
		return false;
	}

	return true;

	if (clip_y1 > y)
	{
		float k =  (clip_y1 - y) / h;
		h *= (1.0f - k);
		y = clip_y1;

		v += dv * k;
		dv *= (1.0f - k);								
	}		

	if (y + h > clip_y2)
	{
		float k = (clip_y2 - y) / h;
		h *= k;				
				
		dv *= k;
	}				
	
	if (clip_x1 > x)
	{
		float k =  (clip_x1 - x) / w;
		w *= (1.0f - k);
		x = clip_x1;

		u += du * k;
		du *= (1.0f - k);								
	}		
	
	if (x + w > clip_x2)
	{
		float k = (clip_x2 - x) / w;
		w *= k;				
				
		du *= k;
	}	

	return true;
}

bool ClipSprite(float& x, float& y, float& w, float& h, float clip_x1, float clip_x2, float clip_y1, float clip_y2, float rotate, float delta_x, float delta_y)
{	
	float x1,y1,x2,y2;

	if (fabs(rotate) < 0.25f)
	{	
		x1 = x;
		x2 = x + w;
		y1 = y;
		y2 = y + h;
	}
	else
	{
		x1 = 100000.0f;
		x2 = -100000.0f;
		y1 = 100000.0f;
		y2 = -100000.0f;

		float sn = sin(RADIAN * rotate);
		float cs = sin(RADIAN * rotate);

		float half_w = w * 0.5f;
		float half_h = h * 0.5f;

		Vector2 coord[] = { Vector2(-1.0f, 1.0f), Vector2(1.0f, 1.0f), Vector2(1.0f, -1.0f), Vector2(-1.0f, -1.0f) };
		
		for (int i=0;i<4;i++)
		{
			float pos_x = x + cs * coord[i].x * half_w - coord[i].y * sn * half_h + delta_x;
			float pos_y = y + sn * coord[i].x * half_w + coord[i].y * cs * half_h + delta_y;

			x1 = MathMin(x1, pos_x);
			y1 = MathMin(y1, pos_y);
			
			x2 = MathMax(x2, pos_x);
			y2 = MathMax(y2, pos_y);
		}		
	}


	
	{	
		if (clip_y1 > y2)
		{						
			return false;
		}

		if (y1  > clip_y2)
		{						
			return false;
		}
	
		if (clip_x1 > x2)
		{						
			return false;
		}

		if (x1  > clip_x2)
		{						
			return false;
		}
	}

	return true;
}

int GetPow(int value)
{
	int val = 1;
	for (int i=0;i<10;i++)
	{
		if (value <= val) return val;
		val *= 2;
	}

	return 1024;
}

#if defined(IOS) || defined(OSUNIX) || defined(ANDROID) || defined(OSX)

void DeleteFile(const char* name)
{
	unlink(name);
}

#endif

#if defined(PC) || defined(OSUNIX)

bool DeleteDirectory(const char* dir, bool noRecycleBin)
{	  
#ifdef PC
	int len = strlen(dir);
	TCHAR *pszFrom = new TCHAR[len+2];
	String::Copy(pszFrom, len + 2, dir);
	pszFrom[len] = 0;
	pszFrom[len+1] = 0;

	SHFILEOPSTRUCT fileop;
	fileop.hwnd   = NULL;    // no status display
	fileop.wFunc  = FO_DELETE;  // delete operation
	fileop.pFrom  = pszFrom;  // source file name as double null terminated string
	fileop.pTo    = NULL;    // no destination needed
	fileop.fFlags = FOF_NOCONFIRMATION|FOF_SILENT;  // do not prompt the user

	if(!noRecycleBin) fileop.fFlags |= FOF_ALLOWUNDO;

	fileop.fAnyOperationsAborted = FALSE;
	fileop.lpszProgressTitle     = NULL;
	fileop.hNameMappings         = NULL;

	int ret = SHFileOperation(&fileop);
	delete [] pszFrom;  
	return (ret == 0);
#endif

	return true;
}

int RunSilent(char* strFunct, char* strstrParams)
{
#ifdef PC
	STARTUPINFO StartupInfo;
	PROCESS_INFORMATION ProcessInfo;
	char Args[4096];
	char *pEnvCMD = NULL;
	char *pDefaultCMD = "CMD.EXE";
	ULONG rc;

	memset(&StartupInfo, 0, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(STARTUPINFO);
	StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	StartupInfo.wShowWindow = SW_HIDE;

	Args[0] = 0;

	pEnvCMD = getenv("COMSPEC");

	if(pEnvCMD)
	{		
		strcpy(Args, pEnvCMD);
	}
	else
	{
		strcpy(Args, pDefaultCMD);
	}

	// "/c" option - Do the command then terminate the command window
	strcat(Args, " /c "); 
	//the application you would like to run from the command window
	strcat(Args, strFunct);  
	strcat(Args, " "); 
	//the parameters passed to the application being run from the command window.
	strcat(Args, strstrParams); 

	if (!CreateProcess( NULL, Args, NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, 
		NULL, 
		NULL,
		&StartupInfo,
		&ProcessInfo))
	{
		return GetLastError();		
	}

	WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
	if(!GetExitCodeProcess(ProcessInfo.hProcess, &rc))
		rc = 0;

	CloseHandle(ProcessInfo.hThread);
	CloseHandle(ProcessInfo.hProcess);

	return rc;
#endif	 

#ifdef OSUNIX

	string cmd = strFunct;
	cmd = cmd + " " + strstrParams;

	int ret = system(cmd.c_str());

	return ret;
#endif	 
}

void GetCurDirectory(char* path, int size)
{
#ifdef PC
	GetCurrentDirectory(size, path);			
#endif
	
#ifdef OSUNIX	
    getcwd(path, size);
#endif
}

#include <iostream>
#include <string>
#include <sys/stat.h>

void CreateDirectories(const char* path)
{
	char ApplicationDir[512];
	
	GetCurDirectory(ApplicationDir, 512);

	int index = 0;
	char PathTo[512];
	
	if (
#ifdef PC
		path[1] != ':'
#endif
#ifdef OSUNIX	
		(path[0] != '/' || path[1] != 'h' || path[2] != 'o' ||
		 path[3] != 'm' || path[4] != 'e' || path[5] != '/' || path[6] != 'u')
#endif
		 )
	{
		strcpy(PathTo,"/");	
	}
	else
	{
		strcpy(PathTo,"");	
	}

#ifdef OSUNIX	
	//String::LowerCase((char*)path);
#endif

	strcat(PathTo,path);

	while (index<(int)strlen(path))
	{
		while (index<(int)strlen(PathTo)&&(PathTo[index]!='/'&&PathTo[index]!='\\'))
		{
			index++;
		}

		if (index<(int)strlen(PathTo))
		{
			char FolderTo[512];

			strcpy(FolderTo,PathTo);

			FolderTo[index]=0;

			char FullPath[512];

			if (
#ifdef PC
				path[1] != ':'
#endif
#ifdef OSUNIX	
				(path[0] != '/' || path[1] != 'h' || path[2] != 'o' ||
				 path[3] != 'm' || path[4] != 'e' || path[5] != '/' || path[6] != 'u')
#endif
				 )
			{
				strcpy(FullPath,ApplicationDir);
			}
			else
			{
				strcpy(FullPath,"");
			}

			strcat(FullPath,FolderTo);

#ifdef PC
			CreateDirectory(FullPath, NULL);
#endif

#ifdef OSUNIX			
			mkdir(FullPath, 0777);
#endif
			index++;
		}
	}
}

bool CopyFileEX(const char* FileName, const char* NewFileName)
{
	char ApplicationDir[512];
	GetCurDirectory(ApplicationDir, 512);
	
	char strFrom[512];
	
	if (FileName[1] == ':')
	{
		strcpy(strFrom,FileName);
	}
	else
	{
		strcpy(strFrom,ApplicationDir);
		strcat(strFrom,"/");	
		strcat(strFrom,FileName);
	}
	
	char strTo[512];
	
	if (NewFileName[1] == ':')
	{
		strcpy(strTo,NewFileName);
	}
	else
	{
		strcpy(strTo,ApplicationDir);
		strcat(strTo,"/");	
		strcat(strTo,NewFileName);
	}
	
	CreateDirectories(NewFileName);	
	
				
	char buf[8192];
    int size;

    FILE* source = fopen(strFrom, "rb");

	if (!source)
	{		
		return false;
	}

	FILE* dest = fopen(strTo, "wb");

	if (!dest)
	{
		fclose(source);
		return false;
	}    

	while (size = fread(buf, 1, BUFSIZ, source))
	{
		fwrite(buf, 1, size, dest);
	}

	fclose(source);
	fclose(dest);		
		
	return true;	
}


void DeleteFolder(const char* FolderName)
{
#ifdef PC
	WIN32_FIND_DATA ffd;
	HANDLE hFile;
	CHAR SerarchDir[200];
	CHAR SerarchParams[200];
	
	BOOL fFile=TRUE;
	
	strcpy(SerarchDir,FolderName);
	
	strcpy(SerarchParams,FolderName);
	strcat(SerarchParams,"\\*.*");
	
	hFile=FindFirstFile(SerarchParams,&ffd);	
	
	if (hFile!=INVALID_HANDLE_VALUE)
	{
		while (fFile)
		{
			if (ffd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
			{
				char FileName[512];
				
				strcpy(FileName,FolderName);
				strcat(FileName,"//");
				strcat(FileName,ffd.cFileName);
				
				DeleteFile(FileName);
			}
			else 
				if (ffd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY &&
					strcmp(ffd.cFileName,".")!=0&&strcmp(ffd.cFileName,"..")!=0)
				{
					char FileName[512];
					
					strcpy(FileName,FolderName);
					strcat(FileName,"//");
					strcat(FileName,ffd.cFileName);
					
					DeleteFolder(FileName);
					
					RemoveDirectory(FileName);
				}			
			
			
			fFile=FindNextFile(hFile,&ffd);
		}
		
		FindClose(hFile);
	}
	
	RemoveDirectory(FolderName);
#endif/*
#ifdef OSUNIX
	char path[512];
	DIR *current = opendir(FolderName);
	if (current == NULL) 
	{
		return -1;
	}
	struct dirent* entry;
	while (entry = readdir(current))
	{
		if (entry->d_type == DT_LNK) 
		{
			strcpy(path,FolderName);
			strcat(path,"/");	
			strcat(path,entry->d_name);
			unlink(path);
		}
		if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) 
		{
			strcpy(path,FolderName);
			strcat(path,"/");	
			strcat(path,entry->d_name);
			DeleteFolder(path);
		}
	}
	closedir(current);
	unlink(FolderName);
#endif*/
}

#endif

#ifdef PC

bool DialogSelectFile(bool open, char* file_name, bool full_path, const char* filter)
{	
	char curDir[512];
	GetCurrentDirectoryA(512, curDir);

	OPENFILENAMEA	ofn = {0};	
	char name[512];
	name[0] = 0;

	char short_name[512];
	short_name[0] = 0;

	ofn.lStructSize = sizeof( ofn );
	ofn.hInstance = NULL;
	//ofn.hwndOwner = core->GetHWND();
	ofn.hwndOwner = NULL;
	ofn.Flags = OFN_FILEMUSTEXIST;

	ofn.lpstrFile = name;
	ofn.nMaxFile = sizeof( name );

	ofn.lpstrFileTitle = short_name;	
	ofn.nMaxFileTitle = sizeof( short_name );

	ofn.nFilterIndex = 1;
	ofn.lpstrFilter = filter;

	char initDir[512];
	sprintf(initDir, "%s\\Projects\\", curDir);

	ofn.lpstrInitialDir = initDir;

	bool res = false;

	if (open)
	{
		res = GetOpenFileNameA( &ofn );
	}
	else
	{
		res = GetSaveFileNameA( &ofn );
	}

	SetCurrentDirectoryA(curDir);

	if ( res )
	{
		if (full_path)
		{
			strcpy( file_name, name );		
		}
		else
		{
			strcpy( file_name, short_name );		
		}

		return true;
	}

	return false;
}

void GetFolderFileList(const char* FolderName,vector<string>& list)
{
	WIN32_FIND_DATA ffd;
	HANDLE hFile;
	CHAR SerarchParams[200];
	BOOL fFile=TRUE;
	char Folder[512];
	if (FolderName[1] == ':')
	{
		strcpy(Folder,FolderName);
	}
	else
	{
		char ApplicationDir[512];
		GetCurDirectory(ApplicationDir, 512);
		strcpy(Folder,ApplicationDir);
		strcat(Folder,"\\");	
		strcat(Folder,FolderName);
	}
	strcpy(SerarchParams,Folder);
	strcat(SerarchParams,"\\*.*");
	hFile=FindFirstFile(SerarchParams,&ffd);	
	if (hFile!=INVALID_HANDLE_VALUE)
	{
		while (fFile)
		{
			if (ffd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
			{
				list.push_back(ffd.cFileName);
			}
			fFile=FindNextFile(hFile,&ffd);
		}
		FindClose(hFile);
	}
}


char* ReadINI(const char* INIFileName,const  char *defaultvalue,const  char *group,const  char *name)
{
	const  int  bufsize = 1024;
	static char buf[bufsize];
	char dir[512];
	GetCurrentDirectory(511, dir);
	String::Cat(dir,512,"//");
	String::Cat(dir,512,INIFileName);

	int k = GetPrivateProfileString (group, name, defaultvalue, buf, bufsize, dir);

	return buf;
}

int ReadINI(const char* INIFileName, int defaultvalue,const  char *group,const  char *name)
{	
	char dir[512];
	GetCurrentDirectory(511, dir);
	String::Cat(dir,512,"//");
	String::Cat(dir,512,INIFileName);
	
	return GetPrivateProfileInt(group, name, defaultvalue, dir);
}
bool WriteINI(const char* INIFileName, const char *value, const char *group, const char *name)
{
    char dir[512];
    GetCurrentDirectory(511, dir);
    String::Cat(dir,512,"//");
    String::Cat(dir,512,INIFileName);
    return WritePrivateProfileString(group,name,value,dir);
}

bool WriteINI(const char* INIFileName, int value, const char *group, const char *name)
{
    char dir[512];
    GetCurrentDirectory(511, dir);
    String::Cat(dir,512,"//");
    String::Cat(dir,512,INIFileName);
    static char buff[1024];
    itoa(value,buff,10);
    return WritePrivateProfileString(group,name,buff,dir);
}

bool OperateFile(char* filename, const char* initdir, const char* mask, const char* caption,bool save)
{
	//IRenderService* render = (IRenderService*)core->GetService("RenderService");
	//render->WNDPresent();

	char ApplicationDir[512];
	GetCurrentDirectory(512,ApplicationDir);

	CHAR strFileName[512];
	CHAR strCurrentName[512];
	strCurrentName[0] = 0;
	CHAR InitDir[512];
	strcpy(InitDir,ApplicationDir);
	if (initdir[0] != '/' && initdir[0] != '\\')
	{
		strcat(InitDir,"\\");
	}

	strcat(InitDir,initdir);

	if (InitDir[strlen(InitDir)-1] == '/')
	{
		InitDir[strlen(InitDir)-1] = 0;
	}
	
	if (InitDir[strlen(InitDir)-1] == '/')
	{
		InitDir[strlen(InitDir)-1] = 0;
	}

	OPENFILENAME ofn = { sizeof(OPENFILENAME), (HWND)en_core.GetHWND(), NULL,
						 mask,
						 NULL, 0, 1, strCurrentName, 512, strFileName, 512,
						 InitDir, caption,
						 OFN_FILEMUSTEXIST, 0, 1,
						 "", 0, NULL, NULL };

	bool res = false;

	if (save)
	{
		res = GetSaveFileName( &ofn );
	}
	else
	{
		res = GetOpenFileName( &ofn );
	}

	if (res)
	{
		SetCurrentDirectory(ApplicationDir);

		char path[512];
		char path2[512];
		strcpy(path2,ApplicationDir);
		strcat(path2,"/");

		String::GetCropPath(path2,strCurrentName,path, 512);
		String::ExtractPath(path,filename, false);

		strcat(filename,strFileName);

		return true;
	}

	return false;
}

bool OpenFile(char* filename, const char* initdir, const char* mask, const char* caption)
{
	return OperateFile(filename, initdir, mask, caption, false);
}

bool SaveFile(char* filename, const char* initdir, const char* mask, const char* caption)
{
	return OperateFile(filename, initdir, mask, caption, true);
}

bool OpenDialog(char* file_name, bool relativePath, const char* filter)
{	
	char curDir[512];
	GetCurrentDirectory(512,curDir);

	OPENFILENAME	ofn = {0};	
	char name[512];
	name[0] = 0;

	ofn.lStructSize = sizeof( ofn );
	ofn.hInstance = NULL;
	ofn.hwndOwner = en_core.GetHWND();
	ofn.Flags = OFN_FILEMUSTEXIST;
	ofn.lpstrFile = name;
	ofn.nMaxFile = sizeof( name );
	ofn.nFilterIndex = 1;
	ofn.lpstrFilter = filter;

	if ( GetOpenFileName( &ofn ) )
	{
		SetCurrentDirectory(curDir);

		strcpy( file_name, name );
		
		if (relativePath)
		{
			en_core.EN_Files()->RelativePath(file_name);
			String::FixSlashes(file_name);
		}

		return true;
	}
	
	return false;
}

void ColorDialog(Color& color)
{	
	CHOOSECOLOR cs_color;

	static dword colors[16];

	cs_color.lStructSize = sizeof(cs_color);
	cs_color.hwndOwner = en_core.GetHWND();
	cs_color.hInstance = NULL;
	cs_color.Flags = CC_FULLOPEN | CC_ANYCOLOR | CC_RGBINIT;

	cs_color.rgbResult = RGBA2COLORF(color.b,color.g,color.r,color.a);
	cs_color.lpCustColors = (COLORREF*)&colors;
	cs_color.lCustData = 0;
	cs_color.lpfnHook = 0;
	cs_color.lpTemplateName = 0;	

	if (ChooseColor(&cs_color))
	{			
		float a,r,g,b;
		COLOR2RGBA(cs_color.rgbResult,b,g,r,a)
		color = Color(r,g,b,a);
	}	
}

#endif

bool ReadArg(int numArg, char** arg, const char* name, int& value)
{
	for (int i=0; i<numArg-1; i++)
	{
		if (arg[i][0] == '-')
		{
			if (String::IsEqual(&arg[i][1], name))
			{				
				value = atoi(arg[i+1]);
				return true;
			}
		}
	}

	return false;
}

bool ReadArg(int numArg, char** arg, const char* name, char* value, int val_len)
{
	for (int i=0; i<numArg-1; i++)
	{
		if (arg[i][0] == '-')
		{
			if (String::IsEqual(&arg[i][1], name))
			{
				String::Copy(value, val_len, arg[i+1]);				
				return true;
			}
		}
	}

	return false;
}

void GetMD5Hash(const char* in_str, char* res)
{
	unsigned char md[MD5_DIGEST_LENGTH];

	EVP_Digest(in_str,strlen(in_str),md,NULL,EVP_md5(), NULL);

	for (int i=0; i<MD5_DIGEST_LENGTH; i++)
	{
		sprintf(&(res[i*2]),"%02x",md[i]);
	}
}

void GetHMac(const char* in_str, const char* key, char* res)
{
	unsigned char md[SHA_DIGEST_LENGTH];

	HMAC(EVP_sha1(),	key, strlen(key), (const unsigned char*)in_str, strlen(in_str), md,NULL);

	for (int i=0; i<SHA_DIGEST_LENGTH; i++)
	{
		sprintf(&(res[i*2]),"%02x",md[i]);
	}
}

int GetTimeInner(int index, tm* ttime)
{
	switch (index)
	{
	case 0:		
		{
			return ttime->tm_hour;
		};
		break;
	case 1:		
		{
			return ttime->tm_min;
		};
		break;
	case 2:		
		{
			return ttime->tm_sec;
		};
		break;
	case 3:		
		{
			return ttime->tm_mday;
		};
		break;
	case 4:		
		{
			return ttime->tm_mon + 1;
		};
		break;
	case 5:		
		{
			return ttime->tm_year + 1900;
		};
		break;
	}		

	return 0;
}

int GetTime(int index)
{
#ifdef PC
	
	time_t szClock;	
	time( &szClock );

	tm *ttime;
	ttime = localtime( &szClock );

#endif

#if defined(OSX) || defined(ANDROID) || defined(IOS) || defined(OSUNIX)
	
	timeval tv;
	gettimeofday(&tv, NULL);

	tm* ttime;
	ttime = localtime(&tv.tv_sec);

#endif

	return GetTimeInner(index, ttime);
}

int GetTimeUTC(int index)
{
	time_t szClock;	
	time( &szClock );

	tm *ttime;
	ttime = gmtime( &szClock );

	return GetTimeInner(index, ttime);
}

int GetTime(int index, int64 unix_time)
{
	time_t tv = (time_t)unix_time;
	tm* ttime;
	ttime = localtime(&tv);

	return GetTimeInner(index, ttime);
}

int64 GetTimestamp(int year, int month, int day, int hour, int minute, int second)
{
	tm ttime;

	time_t szClock;	
	time( &szClock );

	ttime = *gmtime( &szClock );

	ttime.tm_hour = hour;
	ttime.tm_min = minute;
	ttime.tm_sec = second;
	ttime.tm_year = year - 1900;
	ttime.tm_mon = month - 1;
	ttime.tm_mday = day;

#ifdef PC
	_putenv_s("TZ", "UTC");
#else
	setenv("TZ", "", 1);
#endif

	return mktime (&ttime);
}

void Get_ISO8601_datetime( time_t time, std::string& str)
{
	struct tm * timeinfo;
	char buffer [80];

	timeinfo = gmtime ( &time );
	strftime (buffer,80,"%Y-%m-%dT%H:%M:%SZ",timeinfo);

	str = buffer;
}

bool ParseCSV(const char* csvSource, int size, vector<vector<string> >& lines)
{
	bool inQuote(false);
	bool newLine(false);
	string field;
	lines.clear();
	vector<string> line;

	int tab_sz = -1;

	for (int i=0;i<size;i++)
	{
		char ch = csvSource[i];

		switch (ch)
		{
		case '"':
			newLine = false;
			inQuote = !inQuote;
			break;

		case ';':
			newLine = false;
			if (inQuote == true)
			{
				field += ch;
			}
			else
			{
				line.push_back(field);
				field.clear();
			}
			break;

		case '\n':
		case '\r':
			if (inQuote == true)
			{
				field += ch;
			}
			else
			{
				if (newLine == false)
				{
					if (tab_sz == -1)
					{
						tab_sz = field.size();
					}
					else
					if (newLine != field.size())
					{
						lines.clear();

						core->TraceTo("Script", "Error in parsing csv");
						return false;
					}

					line.push_back(field);
					lines.push_back(line);
					field.clear();
					line.clear();
					newLine = true;
				}
			}
			break;

		default:
			newLine = false;
			field.push_back(ch);
			break;
		}
	}

	if (field.size())
	{
		line.push_back(field);
	}

	if (line.size())
	{
		lines.push_back(line);
	}

	return true;
}


static const std::string base64_chars = 
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789+/";


static inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for(i = 0; (i <4) ; i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i)
	{
		for(j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while((i++ < 3))
			ret += '=';

	}

	return ret;

}

std::string base64_decode(std::string const& encoded_string)
{
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i ==4) {
			for (i = 0; i <4; i++)
				char_array_4[i] = base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret += char_array_3[i];
			i = 0;
		}
	}

	if (i) {
		for (j = i; j <4; j++)
			char_array_4[j] = 0;

		for (j = 0; j <4; j++)
			char_array_4[j] = base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
	}

	return ret;
}

bool BuildUtf16fromUtf8(byte c, int& bytes, int& w)
{		
	//wchar_t w = 0;
	//int bytes = 0;
	//wchar_t err =;
	
	//unsigned char c = (unsigned char)src[i];

	if (c <= 0x7f)
	{
		//first byte
		if (bytes)
		{
			//dest.push_back(err);
			bytes = 0;
		}

		//dest.push_back((wchar_t)c);
		w = (wchar_t)c;

		return true;
	}
	else
	if (c <= 0xbf)
	{
		//second/third/etc byte
		if (bytes)
		{
			w = ((w << 6)|(c & 0x3f));
			bytes--;
			
			if (bytes == 0)
			{
				return true;
			}			
		}
		else
		{
			//dest.push_back(err);
		}
	}
	else
	if (c <= 0xdf)
	{
		//2byte sequence start
		bytes = 1;
		w = c & 0x1f;
	}
	else
	if (c <= 0xef)
	{
		//3byte sequence start
		bytes = 2;
		w = c & 0x0f;
	}
	else
	if (c <= 0xf7)
	{
		//3byte sequence start
		bytes = 3;
		w = c & 0x07;
	}
	else
	{
		//dest.push_back(err);
		bytes = 0;
	}	

	return false;
}

void BuildUtf8fromUtf16(int c, string& dest)
{
	dest.clear();

	if (c < (1 << 7))
	{
		dest += c;
	}
	else
	if (c < (1 << 11))
	{
		dest += ((c >> 6) | 0xcC0);
		dest += ((c & 0x3F) | 0x80);
	}
	else
	if (c < (1 << 16))
	{
		dest += ((c >> 12) | 0xE0);
		dest += (((c >> 6) & 0x3F) | 0x80);
		dest += ((c & 0x3F) | 0x80);
	}
	else
	if (c < (1 << 21))
	{
		dest += ((c >> 18) | 0xE0);
		dest += (((c >> 12) & 0x3F) | 0x80);
		dest += (((c >> 6) & 0x3F) | 0x80);
		dest += ((c & 0x3F) | 0x80);
	}
}

void HTML2Text(string& in, string& out)
{
	bool in_tag = false;

	int len = strlen(in.c_str());	

	for (int j=0;j<len;j++)
	{
		int depth = len - j - 1;

		char c = in[j];

		if (c == 13)
		{
			out += '\\';
			out += 'n';
			continue;
		}

		if (c == '<')
		{
			if ( in[j+1] == 'b' && in[j+2] == 'r' &&  in[j+3] == '>')
			{
				out += '\\';
				out += 'n';

				j+= 3;
				continue;
			}
			else
			if ( in[j+1] == 'b' && in[j+2] == 'r' &&  in[j+3] == '1' ||			
				 in[j+1] == 'b' && in[j+2] == 'r' &&  in[j+3] == '2' ||
				 in[j+1] == 'b' && in[j+2] == 'r' &&  in[j+3] == '3')
			{
				out += '\\';
				out += 'n';

				j+= 4;
				continue;
			}
		}		

		if (c == '&' && !in_tag && depth>1)
		{						
			if (depth >= 4 && in[j+1] == 'a' && in[j+2] == 'm')
			{
				out += '&';
				j+= 4;
				continue;
			}
			else
			if (depth >= 5 && in[j+1] == 'b' && in[j+2] == 'u')
			{
				j+= 5;
				continue;
			}
			else
			if (depth >= 4 && in[j+1] == 'r' && in[j+2] == 'e')
			{
				j+= 4;
				continue;
			}
			else		
			if (depth >= 5 && in[j+1] == 'n' && in[j+2] == 'b' && in[j+3] == 's' && in[j+4] == 'p')
			{
				out += ' ';
				j+= 5;
				continue;
			}
			else
			if (depth >= 5 && in[j+1] == 'q' && in[j+2] == 'u')
			{
				out += '\'';
				j+= 5;
				continue;
			}
			else
			if (depth >= 6 && in[j+1] == 'r' && in[j+2] == 'a')
			{
				out += '\'';
				j+= 6;
				continue;
			}
			else
			if (depth >= 6 && in[j+1] == 'r' && in[j+2] == 's')
			{
				out += '\'';
				j+= 6;
				continue;
			}
			else
			if (depth >= 6 && in[j+1] == 'n' && in[j+2] == 'd')
			{
				out += '-';
				j+= 6;
				continue;
			}
			else
			if (depth >= 6 && in[j+1] == 'm' && in[j+2] == 'd')
			{
				out += '-';
				j+= 6;
				continue;
			}
			else				
			if (depth >= 6 && in[j+1] == 'l' && in[j+2] == 'd')
			{
				out += '"';
				j+= 6;
				continue;
			}
			else			
			if (depth >= 6 && in[j+1] == 'r' && in[j+2] == 'd')
			{
				out += '"';
				j+= 6;
				continue;
			}
			else			
			if (depth >= 6 && in[j+1] == 'w' && in[j+2] == 'd')
			{
				out += '-';
				j+= 6;
				continue;
			}			
			else	
			if (depth >= 6 && in[j+1] == 'n' && in[j+2] == 'b')
			{
				out += ' ';
				j+= 6;
				continue;
			}
			else
			if (depth >= 6 && in[j+1] == 'l' && in[j+2] == 'a')
			{
				out += '\'';
				j+= 6;
				continue;
			}
			else
			if (depth >= 7 && in[j+1] == 'm' && in[j+2] == 'i')
			{
				out += '-';
				j+= 7;
				continue;
			}
			else
			if (depth >= 7 && in[j+1] == 'h' && in[j+2] == 'e')
			{
				out += '.';
				out += '.';
				out += '.';

				j+= 7;
				continue;
			}			
		}

		if (!in_tag)
		{
			if (c == '<')
			{
				if (j+1 < len)
				{
					if (46 < in[j+1] && in[j+1] < 123)
					{
						in_tag = true;
					}					
				}
			}
		}
		else
		{
			if (c == '>')
			{
				in_tag = false;
				continue;
			}
		}

		if (!in_tag)
		{
			out += c;
		}
	}
}	

int CalcUtf8Len(const char* text)
{
	int w = 0;
	int bytes = 0;
	
	int len = strlen(text);
	
	int text_len = 0;

	for (int i=0;i<len;i++)
	{											
		if (!BuildUtf16fromUtf8(text[i], bytes, w))
		{
			continue;
		}

		text_len++;		
	}

	return text_len;
}

void ClampUtf8(string& text, int clamp_len)
{
	int w = 0;
	int bytes = 0;
	
	int len = text.length();
	
	int text_len = 0;
	
	for (int i=0; i<len; i++)
	{											
		if (!BuildUtf16fromUtf8(text[i], bytes, w))
		{
			continue;
		}

		text_len++;

		if (clamp_len == text_len)
		{
			text.resize(i + 1);
			return;
		}
	}
}

bool IsPointInsideTriangle(Vector2 s, Vector2 a, Vector2 b, Vector2 c)
{
    int as_x = s.x-a.x;
    int as_y = s.y-a.y;

    bool s_ab = (b.x-a.x)*as_y-(b.y-a.y)*as_x > 0;

    if((c.x-a.x)*as_y-(c.y-a.y)*as_x > 0 == s_ab) return false;

    if((c.x-b.x)*(s.y-b.y)-(c.y-b.y)*(s.x-b.x) > 0 != s_ab) return false;

    return true;
}

bool ConvertHexColor(const std::string& txt,long& red,long& green,long& blue)
{
    if(txt[0] == '#' && txt.size() > 6)
    {
        string sRed,sGreen,sBlue;
        sRed.assign(txt.begin() + 1,txt.begin() + 3);
        sGreen.assign(txt.begin() + 3,txt.begin() + 5);
        sBlue.assign(txt.begin() + 5,txt.begin() + 7);
        char * p;
        red = strtoul( sRed.c_str(), & p, 16 ); 
        if ( * p != 0 )return false;
        green = strtoul( sGreen.c_str(), & p, 16 ); 
        if ( * p != 0 )return false;
        blue = strtoul( sBlue.c_str(), & p, 16 ); 
        if ( * p != 0 )return false;
        return true;
    }
    return false;
}

bool ConvertHexColor(const std::string& txt,long& red,long& green,long& blue,float& alpha)
{
    if(txt[0] == '#' && txt.size() > 8)
    {
        string sRed,sGreen,sBlue,sAlpha;
        sRed.assign(txt.begin() + 1,txt.begin() + 3);
        sGreen.assign(txt.begin() + 3,txt.begin() + 5);
        sBlue.assign(txt.begin() + 5,txt.begin() + 7);
        sAlpha.assign(txt.begin() + 7,txt.begin() + 9);
        char * p;
        red = strtoul( sRed.c_str(), & p, 16 ); 
        if ( * p != 0 )return false;
        green = strtoul( sGreen.c_str(), & p, 16 ); 
        if ( * p != 0 )return false;
        blue = strtoul( sBlue.c_str(), & p, 16 ); 
        if ( * p != 0 )return false;
        alpha = (float)strtoul( sAlpha.c_str(), & p, 16 );
        if ( * p != 0 )return false;
        alpha /= 255;
        return true;
    }
    return false;
}


float EasyInOut::QuadraticEaseIn(float p)
{
	return p * p;
}

float EasyInOut::QuadraticEaseOut(float p)
{
	return -(p * (p - 2));
}

float EasyInOut:: QuadraticEaseInOut(float p)
{
	if(p < 0.5)
	{
		return 2 * p * p;
	}
	else
	{
		return (-2 * p * p) + (4 * p) - 1;
	}
}

float EasyInOut::CubicEaseIn(float p)
{
	return p * p * p;
}

float EasyInOut::CubicEaseOut(float p)
{
	float f = (p - 1);
	return f * f * f + 1;
}

float EasyInOut::CubicEaseInOut(float p)
{
	if(p < 0.5f)
	{
		return 4 * p * p * p;
	}
	else
	{
		float f = ((2 * p) - 2);
		return 0.5f * f * f * f + 1;
	}
}

float EasyInOut::QuarticEaseIn(float p)
{
	return p * p * p * p;
}

float EasyInOut::QuarticEaseOut(float p)
{
	float f = (p - 1);
	return f * f * f * (1 - p) + 1;
}

float EasyInOut::QuarticEaseInOut(float p) 
{
	if(p < 0.5f)
	{
		return 8 * p * p * p * p;
	}
	else
	{
		float f = (p - 1);
		return -8 * f * f * f * f + 1;
	}
}

float EasyInOut::QuinticEaseIn(float p) 
{
	return p * p * p * p * p;
}

float EasyInOut::QuinticEaseOut(float p) 
{
	float f = (p - 1);
	return f * f * f * f * f + 1;
}

float EasyInOut::QuinticEaseInOut(float p) 
{
	if(p < 0.5f)
	{
		return 16 * p * p * p * p * p;
	}
	else
	{
		float f = ((2 * p) - 2);
		return  0.5f * f * f * f * f * f + 1;
	}
}

float EasyInOut::SineEaseIn(float p)
{
	return sin((p - 1) * TWO_PI) + 1;
}

float EasyInOut::SineEaseOut(float p)
{
	return sin(p * TWO_PI);
}

float EasyInOut::SineEaseInOut(float p)
{
	return 0.5f * (1 - cos(p * PI));
}

float EasyInOut::CircularEaseIn(float p)
{
	return 1 - sqrt(1 - (p * p));
}

float EasyInOut::CircularEaseOut(float p)
{
	return sqrt((2 - p) * p);
}

float EasyInOut::CircularEaseInOut(float p)
{
	if(p < 0.5)
	{
		return 0.5f * (1 - sqrt(1 - 4 * (p * p)));
	}
	else
	{
		return 0.5f * (sqrt(-((2 * p) - 3) * ((2 * p) - 1)) + 1);
	}
}

float EasyInOut::ExponentialEaseIn(float p)
{
	return (p == 0.0) ? p : (float)pow(2, 10 * (p - 1));
}

float EasyInOut::ExponentialEaseOut(float p)
{
	return (p == 1.0) ? p : 1 - (float)pow(2, -10 * p);
}

float EasyInOut::ExponentialEaseInOut(float p)
{
	if(p == 0.0 || p == 1.0) return p;
	
	if(p < 0.5)
	{
		return 0.5f * pow(2.f, (20 * p) - 10);
	}
	else
	{
		return -0.5f * pow(2.f, (-20 * p) + 10) + 1;
	}
}

float EasyInOut::ElasticEaseIn(float p)
{
	return sin(13 * TWO_PI * p) * pow(2.f, 10 * (p - 1));
}

float EasyInOut::ElasticEaseOut(float p)
{
	return sin(-13 * TWO_PI * (p + 1)) * pow(2.f, -10 * p) + 1;
}

float EasyInOut::ElasticEaseInOut(float p)
{
	if(p < 0.5)
	{
		return 0.5f * sin(13 * TWO_PI * (2 * p)) * pow(2.f, 10 * ((2 * p) - 1));
	}
	else
	{
		return 0.5f * (sin(-13 * TWO_PI * ((2 * p - 1) + 1)) * pow(2.f, -10 * (2 * p - 1)) + 2);
	}
}

float EasyInOut::BackEaseIn(float p)
{
	return p * p * p - p * sin(p * TWO_PI);
}

float EasyInOut::BackEaseOut(float p)
{
	float f = (1 - p);
	return 1 - (f * f * f - f * sin(f * TWO_PI));
}

float EasyInOut::BackEaseInOut(float p)
{
	if (p < 0.5)
	{
		float f = 2 * p;
		return 0.5f * (f * f * f - f * sin(f * TWO_PI));
	}
	else
	{
		float f = (1 - (2*p - 1));
		return 0.5f * (1 - (f * f * f - f * sin(f * TWO_PI))) + 0.5f;
	}
}

float EasyInOut::BounceEaseIn(float p)
{
	return 1 - BounceEaseOut(1 - p);
}

float EasyInOut::BounceEaseOut(float p)
{
	if(p < 4/11.0f)
	{
		return (121 * p * p)/16.0f;
	}
	else if(p < 8/11.0)
	{
		return (363/40.0f * p * p) - (99/10.0f * p) + 17/5.0f;
	}
	else if(p < 9/10.0)
	{
		return (4356/361.0f * p * p) - (35442/1805.0f * p) + 16061/1805.0f;
	}
	else
	{
		return (54/5.0f * p * p) - (513/25.0f * p) + 268/25.0f;
	}
}

float EasyInOut::BounceEaseInOut(float p)
{
	if(p < 0.5)
	{
		return 0.5f * BounceEaseIn(p*2);
	}
	else
	{
		return 0.5f * BounceEaseOut(p * 2 - 1) + 0.5f;
	}
}

FontParams::FontParams(const FontParams& other):font(NULL)
{
    other.Copy(*this);
    if(other.font)
        font = other.font->Clone();
}

FontParams& FontParams::operator=(const FontParams& other)
{
    other.Copy(*this);
    if(other.font)
        font = other.font->Clone();
    else
        RELEASE(font);
    return *this;
}

FontParams::~FontParams()
{
    RELEASE(font);
}
