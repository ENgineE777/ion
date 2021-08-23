#pragma once
#include "Core/Core.h"


class NGUIWidget;

class NGUIService : public ISceneService
{		
	RenderTechnique* tech;
	RenderTechnique* tech_alpha;
	RenderTechnique* tech_mask;
	RenderTechnique* tech_video;
	VertexBuffer* vbuffer;
	VertexBuffer* vbuffer_mask;
	
	RenderTechnique* last_tech;

	RenderTechnique* shape_tech;
	RenderTechnique* shape_tech_alpha;
	VertexBuffer* shape_vbuffer;
	
    struct TouchRegion
    {
        NGUIWidget*             pWidget;
        std::vector<ClipRect>   vRects;
        TouchRegion():pWidget(null){}
        TouchRegion(float x,float y,float w,float h,NGUIWidget* widget):pWidget(widget)
        {
            AddRect(x,y,w,h);
        }
        void AddRect(float x,float y,float w,float h)
        {
            vRects.push_back(ClipRect(x,y,x + w,y + h));
        }
        bool IsIn(float x,float y) const
        {
            for(unsigned i =0;i < vRects.size();++i)
            {
                const ClipRect& rect = vRects[i];
                if(rect.x1 < x && x < rect.x2 && rect.y1 < y && y < rect.y2)return true;
            }
            return false;
        }
    };
    typedef std::vector<TouchRegion> TTouchRegions;
	struct SpriteVertex
	{
		Vector  Position;
		Vector2 Texcoords;
	};			

	struct SpriteVertexMask
	{
		Vector  Position;
		Vector2 Texcoords;
		Vector2 Texcoords2;
	};			

	int draw_mode;
	Vector4 params[5];

	int state;

	ThreadLocker locker;

	struct DrawCall
	{		
		Texture* tex;		
		float  scr_x, scr_y;
		float  width, height;
		Color  color;
		Matrix transform;
		float  depth;
		float  u,v;
		float  du,dv;		
	};

    struct DrawTextCall
    {		
        FontInstance*  font;		
        float          font_scale;
        ClipRect       rect; 
        Color          color;
        Matrix         transform;
        std::string    text;
        TextAlign      align;

        std::vector<FontRes::LineData> line_indexes;
    };
		
	std::vector<DrawCall>     draws;
    std::vector<DrawTextCall> drawsText;
	std::vector<FontRes::LineData> line_indexes;

public:

	std::vector<NGUIWidget*> top_widgets;	
	std::vector<NGUIWidget*> symbol_widgets;	
	NGUIWidget* symbol_widget;	
    TTouchRegions touch_regions;

	static NGUIService* instance;

//#ifdef EDITOR
	bool debug_color;
	bool trace_hovered_widget;
//#endif

    TextureInstance* pFilledTexture;

#ifdef EDITOR
    TextureInstance* checker_tex;	
	static int       display_width;
	static int       display_height;
#endif

	float h_mul_RefToScreen;
	float v_mul_RefToScreen;
	float h_mul_ScreenToRef;
	float v_mul_ScreenToRef;

	int cur_rwidth;
	int cur_rheight;	
	bool render_changed;
		
	int ref_width;
	int ref_height;

	virtual void Init();

    void AddTouchRegion(float x,float y,float w,float h,NGUIWidget* widget);
    void RemoveTouchRegion(NGUIWidget* widget);

	void AddSpriteCall(Texture* texture, TextureInstance* textureMask, float x, float y, float wdt, float hgt, Color& color, Matrix& transform, float depth,
					   float u, float v, float du, float dv);

	void AddTopSpriteCall(Texture* texture, float x, float y, float wdt, float hgt, Color& color, Matrix& transform, float depth,
					   float u, float v, float du, float dv);

    void AddTopPrintCall(FontInstance* font, Matrix& transform,const ClipRect& rect, float font_scale, Color color, TextAlign align,  const char* text);

	void AddSquareCall(TextureInstance* texture, float depth,
					   Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4,
					   Vector2 uv1, Vector2 uv2, Vector2 uv3, Vector2 uv4, Color color);
	
	void AddPrintCall(FontInstance* font, Matrix& transform, ClipRect* rect, std::vector<FontRes::LineData>* line_indices, float font_scale, Color color, TextAlign align,  const char* text);

	void ExecuteCalls(float dt, int level);

    void ClearWidgetsLists();
	void AddTopWidget(NGUIWidget* widget,bool bSort = true);
	void DelTopWidget(NGUIWidget* widget);
    inline int TopWidgetsCount(){return top_widgets.size();}


	void AddSymbolWidget(NGUIWidget* symbol);
	void DelSymbolWidget(NGUIWidget* symbol);

	NGUIWidget* FindSymbolWidget(ISceneInstance* scene, const char* name);

	void CalcRefMultiplaers(float dt, int level);	
	void UpdateChildsPos(float dt, int level);	    
    
#ifdef EDITOR
	bool DebugColor();	
#endif

	virtual void Release();
};
