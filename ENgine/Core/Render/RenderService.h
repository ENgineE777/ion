
#pragma once

#include "Common/Containers/HashMap.h"
#include "Core/Render/Textures/TextureRes.h"
#include "Core/Render/Textures/Texture.h"
#include "Core/Render/Textures/TextureInstance.h"
#include "Core/Render/Geometry/VertexBuffer.h"
#include "Core/Render/Geometry/IndexBuffer.h"
#include "Core/Render/Shaders/VertexShader.h"
#include "Core/Render/Shaders/PixelShader.h"
#include "Core/Render/Technique/RenderTechnique.h"

#include "Core/Render/Debug/DebugFont.h"
#include "Core/Render/Debug/DebugLines.h"
#include "Core/Render/Debug/DebugLines2D.h"
#include "Core/Render/Debug/DebugSphere.h"
#include "Core/Render/Debug/DebugSprites.h"

#include "Common/Common.h"
#include "Common/ThreadWorker.h"
#include "Common/Frustum.h"

class RenderService : public TaskHoldersOwner
{			
public:	

	enum Primitive
    {
        pointsList,
        lineStrip,		
        linesList,
        triangleStrip,        
        trianglesList,
        primForceDword = 0x7fffffff
    };
			
	enum BlendArg
	{
		argZero         = 1,
		argOne          = 2,		
		argSrcColor     = 3,
		argInvSrcColor  = 4,
		argSrcAlpha     = 5,
		argInvSrcAlpha  = 6,
		argDestAlpha    = 7,
		argInvDestAlpha = 8,
		argDestColor    = 9,
		argInvDestColor = 10,		
		argForceDword   = 0x7fffffff
	};

	enum BlendOp
	{
		blendAdd        = 1,
		blendSub        = 2,
		blendRevSub       = 3,
		blendMin        = 4,
		blendMax      = 5,
		blendForceDword = 0x7fffffff
	};

	enum CompareFunc
	{
		cmpNever       = 1,
		cmpLess        = 2,
		cmpEqual       = 3,
		cmpLessEqual   = 4,
		cmpGreater     = 5,
		cmpNotequal    = 6,
		cmpGreaterqual = 7,
		cmpAlways      = 8,
		cmpForceDword  = 0x7fffffff
	};

	enum CullMode
	{
		cullNone       = 1,
		cullCW         = 2,
		cullCCW        = 3,
		cullForceDword = 0x7fffffff
	};

	enum StencilOp
	{
		stencilKeep       = 1,
		stencilZero       = 2,
		stencilReplace    = 3,
		stencilIncSat     = 4,
		stencilDecSat     = 5,
		stencilInv        = 6,
		stencilInc        = 7,
		stencilDec        = 8,
		stencilForceDword = 0x7fffffff
	};

	enum SamplerState
	{
		SamplerAddressU = 1,
		SamplerAddressV = 2,
		SamplerAddressW = 3,
		SamplerBorederColor = 4,
		SamplerMagFilter = 5,
		SamplerMinFilter = 6,
		SamplerMipFilter = 7,
		SamplerMipmapLodBias = 8,
		SamplerMaxMipmapLevel = 9,
		SamplerMaxAnizotropy = 10,		
		SamplerForceDword = 0x7fffffff
	};
	
	struct Viewport
	{
		short x;
		short y;            
		word width;
		word height;       
		float minZ;         
		float maxZ;
	};

	struct Rect
	{
		short left;
		short right;            
		short top;
		short bottom;		
	};

	struct VideoMode
	{
		int width;
		int height;
	};	

	class TaskManager
	{
		struct Task
		{
			int level;
			TaskHolder* entity;
			TaskHoldersOwner* owner;
			TaskHolder::Task call;		
		};

		struct TaskList
		{		
			std::vector<Task> list;
		};
	
		HashMap<TaskList> task_groups;

		struct MainTaskGroup
		{
			char name[32];
			int level;			
			TaskList* phase;
		};

		std::vector<MainTaskGroup> main_task_groups;

		ThreadLocker phase_locker;

	public:

		TaskManager();
		void AddMainTaskGroup(const char* group_name, int level);
		void Execute();
		void ExecutePhase(const char* group_name);
		void AddTask(const char* group_name, TaskHolder* entity, TaskHoldersOwner* taskHolderOwner, TaskHolder::Task call, int level);
		void DelTask(const char* group_name, TaskHolder* entity);
		void DelAllTasks(TaskHolder* entity);
		void Release();
	};

	class StateManager
	{
	public:

		StateManager();
		virtual void SetAlphaTest(dword enable) = 0;
		virtual void SetAlphaTestFunc(CompareFunc func, dword ref) = 0;

		virtual void SetAlphaBlend(dword enable) = 0;
		virtual void SetBlendFunc(BlendArg src, BlendArg dest) = 0;
		virtual void SetBlendOperation(BlendOp op) = 0;
	
		virtual void SetDepthTest(dword enable) = 0;	
		virtual void SetDepthWriting(dword enable) = 0;	
		virtual void SetDepthFunc(CompareFunc func) = 0;

		virtual void SetCulling(CullMode mode) = 0;	

		virtual void SetStencilTest(dword enable) = 0;	
		virtual void SetStencilTestFunc(CompareFunc func, dword ref, dword mask) = 0;
		virtual void SetStencilWriteMask(dword mask) = 0;
		virtual void SetStencilOperation(StencilOp failOp, StencilOp zFailOp, StencilOp zPass) = 0;

		virtual void SetColorWrite(bool red, bool green, bool blue, bool alpha) = 0;	
		virtual void SetClipPlane(dword clipIndex, dword enable) = 0;	
	
		virtual void SetWireFrameMode(dword enable) = 0; 
	
		virtual void SetSamplerState(dword sampler, SamplerState state, dword value) = 0;	
	
		virtual void SetupClipPlane(dword index, Vector4 plane) = 0;

		virtual void SetupSlopeZBias(bool enable, float slopeZBias, float depthOffset) = 0;

		virtual void SetScissor(dword enable) = 0;
		virtual void SetScissorRect(const Rect& rect) = 0;
		virtual void GetScissorRect(Rect& rect) = 0;	

		virtual void SetDefaultStates();		
	};

	class DisplayManager
	{
		friend class RenderServiceD3D;
		friend class RenderServiceD3D11;		
		friend class RenderServiceGLES;		

	public:	
		
		DisplayManager();
		int GetWidth();
		int GetHeight();
		float GetAspectRatio();
		float GetReferenceWidth();
		float GetReferenceHeight();
		void SetReferenceHeight(int hgt);

		virtual void SetView(const Matrix& mat);
		virtual void SetProj(const Matrix& mat);
		
		virtual void GetCamPos(Vector& pos);
		virtual void GetView(Matrix& mat);
		virtual void GetInvView(Matrix& mat);
		virtual void GetProj(Matrix& mat);
		virtual void GetViewProj(Matrix& mat);	

		int GetVideoModeCount();
		VideoMode GetVideoMode(int index);

		virtual void ApplyVideoMode(VideoMode mode);
		
		virtual bool SphereInFrustum( Vector pos, float radius );
		virtual bool BoxInFrustum( Vector* box );	

		virtual void SetViewport(const Viewport& viewport) = 0;
		virtual void GetViewport(Viewport& viewport) = 0;			

		Vector TransformToScreen(const Vector& pos, int type);
		void TransformScreenToSpace(const Vector2& pos, float depth, Vector& screen, Vector& end_ray);

		virtual void SetScreenDPI(int dpi); 
		virtual int GetScreenDPI(); 

	protected:

		FrustumPlane frustum[6];

		int screen_dpi;

		Matrix view;
		Matrix inv_view;
		Matrix proj;
		Matrix view_proj;

		int width;
		int height;
		float aspectRatio;
		float ref_width;
		float ref_height;

		int cur_mode;
		int numModes;
		VideoMode modes[64];

		void UpdateFrustum();
		void CalcReferenceWidth();
	};

	class DebugManager
	{
		DebugFont    debugFont;
		DebugLines   debugLines;
		DebugLines2D debugLines2D;
		DebugSprites debugSprites;
		DebugSphere  debugSpheres;

	public:

		DebugManager();
		void Init();
			
		void PrintText(float x, float y, ElemAlign corner, const Color& color, const char* format, ...);
		void PrintText3D(Vector pos, float dist, const Color& color, const char* format, ...);		
	
		void DrawLine(const Vector& p1, const Color& color1, const Vector& p2, const Color& color2, bool noZ = false);	
		void DrawLine2D(const Vector2& p1, const Color& color1, const Vector2& p2, const Color& color2);
		void DrawCircle(int axis, Matrix& mat, float radius, Color color, Color back_color);	
		void DrawSprite(Texture* texture, const Vector2& pos, const Vector2& size,
						ElemAlign corner, const Color& color, float angle, 
						const Vector2& uv1 = 0.0f, const Vector2& uv2 = 1.0f);
		void DrawBox(const Vector& size, const Matrix transform, const Color& color, bool noZ = false);
		void DrawSolidBox(const Vector& size, const Matrix transform, const Color& color);
		void DrawSphere(const Vector& pos, float radius, const Color& color);

		void Release();
	};
	
	enum TextureLoadState
	{
		tls_idle = 0,
		tls_prepare_data = 1,
		tls_waiting_processing,
		tls_processing,
		tls_ready_to_grab
	};

	class TextureData : public Texture::TextureData
	{
	public:

		enum Format
		{
			tex_bitmap,
			tex_jpeg,
			tex_png,			
			tex_gif,
			tex_tga,
			tex_unknown
		};

		Format format;
		Texture* texture;
		TextureLoadState state;				
		class IFileBuffer* file;

		TextureData() : Texture::TextureData()
		{
			texture = null;
			state = tls_idle;			
			file = null;
		};

		bool GetInfo();

		virtual bool GetInfo(byte* header, class IFile* fl);

		bool Load();

		virtual bool LoadImpl();

		bool LoadBITMAP();		
		bool LoadPNG();
		bool LoadGIF();		
		bool LoadSTB();		
	};
    
    static int   quene_class_size;
	static byte* quene_texture;

	class TextureLoader : public ThreadWorker
	{
	public:

		int  cur_data;		

		TextureLoader()
		{
			cur_data = 0;			
		};

		virtual void Prepare();	
		virtual void Loop();		
	};

	static int texture_count;
	static int texture_memory;

	int last_quene_texture;
	int cur_quene_texture;
	TextureLoader texture_loader;

protected:
	
	vector<VertexBuffer*>    vbuffers;
	vector<IndexBuffer*>     ibuffers;
	HashMap<TextureRes>      textures;
	HashMap<VertexShader>    vshaders;
	HashMap<PixelShader>     pshaders;
	HashMap<RenderTechnique> techniques;
	
	DebugManager   debug_manager;
	TaskManager    task_manager;		

	bool need_reset;	

	int cur_depth;	
	
	int draw_calls;
	int poly_count;

	RenderTechnique* cur_tech;

	bool null_render;		

	bool use_dummy_render;	

public:    

	RenderService();	
	virtual ~RenderService() {};
		
	virtual bool Init(int wgt, int hgt);		

	int GetDrawCalls();
	int GetPrimCount();
	
	virtual bool Reset();

	virtual void Begin();
	virtual void End();	

	void EnableNullRender(bool act);
	bool IsNullRender();			
	
	virtual void Clear(bool renderTarget, Color color, bool zbuffer, float zValue, bool stencil, dword stencilValue ) = 0;

	virtual void DrawPrimitive(Primitive type, int startVertex, int primCount) = 0;
	virtual void DrawIndexedPrimitive(Primitive type, int startIndex, int primCount) = 0;	

	virtual TextureInstance* LoadTexture(const char* name, bool async, const char* file, int line);
	virtual Texture* CreateTexture(int width, int height, int lods, Texture::Format format, const char* file, int line) = 0;	

	virtual VertexBuffer* CreateVertexBuffer(dword size,int stride, const char* file, int line) = 0;
	virtual void SetVertexBuffer(int index, VertexBuffer* buffer) = 0;

	virtual IndexBuffer* CreateIndexBuffer(dword size, const char* file, int line) = 0;
	virtual void SetIndexBuffer(IndexBuffer* buffer) = 0;	

	VertexShader* CreateVertexShader(const char* name, const char* file, int line);
	virtual VertexShader* CreateVertexShaderImpl(const char* name) = 0;
	virtual void SetVertexShader(VertexShader* shader) = 0;

	PixelShader* CreatePixelShader(const char* name, const char* file, int line);
	virtual PixelShader* CreatePixelShaderImpl(const char* name) = 0;
	virtual void SetPixelShader(PixelShader* shader) = 0;

	virtual RenderTechnique* CreateRenderTechnique(const char* name);
	virtual void SetRenderTechnique(RenderTechnique* tech);		

	void DeleteRes(TextureRes* res);	
	void DeleteVBuffer(VertexBuffer* vb);	
	void DeleteIBuffer(IndexBuffer* ib);	

	virtual void Release();

	virtual void ReCreateResources();	

	virtual void SetUseDummyRender(bool dummy_rener);
	virtual bool IsUseDummyRender();

	virtual void MakeScreenshot(std::string& out_image) = 0;

	void GetTextureInfo(int& tex_count, int& tex_loading, int& tex_consuption);

	TaskManager* Tasks()
	{
		return &task_manager;
	};

	virtual DebugManager* Debug()
	{
		return &debug_manager;
	};

	virtual DisplayManager* Display() = 0;		
	virtual StateManager* State() = 0;	

	Texture::TextureData* AddTextureToQuene(Texture* texture, const char* file_name);
	void MarkAsSkipped(Texture* texture);
	void Work(float dt, int level);
};
