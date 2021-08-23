#pragma once
#include "Core/Core.h"
#include "NGUIService.h"
#include "NGUIFont.h"
#include "NGUITexture.h"

class NGUIWidget : public SceneEntity
{			
protected:
		
	struct ScrollerData
	{
		bool is_draged;		
							
		float scroll_size;
	

		bool need_auto;
		bool need_check_dir;

		float velocity, amplitude, time_accum, offset_accum;
		int target;
		float rubber_time;
		float auto_delta;
		float time_to_block_childs;

		float offset;

		float auto_border;

        float refresh_offset;

		bool allow_rubber;

        inline void Reset()
        {
			if (allow_rubber)
			{
				auto_border = 200;
			}
			else
			{
				auto_border = 0;
			}

            time_accum = 0.0f;
            offset_accum = 0;
            rubber_time = -1.0f;
            velocity = amplitude = 0.0f;
            target = 0;
            refresh_offset = 0;
            is_draged = false;
            need_auto = false;
            need_check_dir = false;
            auto_delta = 0.0f;
            time_to_block_childs = 0.0f;
        }


		ScrollerData()
		{
			allow_rubber = true;            
            offset = 0;			

			Reset();
		};
	};

	struct DrawCall
	{		
		Texture* tex;		
		float  scr_x, scr_y;
		float  width, height;
		Color  color;		
		float  depth;
		float  u,v;
		float  du,dv;		
	};

	struct PrintCall
	{
		FontInstance* font;		
		std::string  text;		
		float scr_x, scr_y;		
		float scr_w, scr_h;	
		float font_scale;
		Color color;
		std::vector<FontRes::LineData>* line_indices;
	};			
		
	enum Align
	{
		align_left,
		align_center,
		align_right,
		align_top,
		align_bottom
	};
    
	struct TexSlice
	{
		TextureEntry::Slice* slice;
		NGUITexture*		 tex_entity;
		bool*				 prop_bool;

#ifdef EDITOR
		bool				 prev_use_texture_entity;		
		PropertyBase*			 prop_entity_name;
		PropertyBase*			 prop_tex_edit;	
#endif
		const char*				 prop_char_entity_name;
		
		TexSlice(TextureEntry::Slice* set_slice,bool* set_prop_bool
#ifdef EDITOR
				 , PropertyBase* set_prop_entity_name, PropertyBase* set_prop_tex_edit
#endif
				 , const char* set_prop_char_entity_name)
		{
			slice = set_slice;
			tex_entity = null;
			prop_bool = set_prop_bool;

#ifdef EDITOR
			prop_entity_name = set_prop_entity_name;
			prop_tex_edit = set_prop_tex_edit;
#endif

			prop_char_entity_name = set_prop_char_entity_name;
			
#ifdef EDITOR
			prev_use_texture_entity = true;
#endif
		};
	};


	class RequestListiner : public NetworkService::RequestListiner
	{
	public:
		virtual void OnRequestFinish(int res, void* data, const char* file_name);		
	};

	static RequestListiner net_listiner;	
public:	
    std::vector<TexSlice> slices;	
	
	Matrix local_mat;
	Matrix global_mat;
	Matrix childs_global_mat;

	bool  bUseVertTextAlignInDrawCall;

	bool  focused;	
	bool  need_calc_to_parent_space;
	float cur_anchor_x, cur_anchor_y;
		
	float pre_x, pre_y;
	float scr_x, scr_y;
	
	float scr_delta_anchor_x, scr_delta_anchor_y;
		
	float pre_anchor_x, pre_anchor_y;
	float scr_anchor_x, scr_anchor_y;
	
	float scr_w, scr_h;
	enum {eRecalcNone = 0,eRecalcPos = 2,eRecalcTextLine = 4};	
	int  need_recalc;
	float prev_w, prev_h;
	float prev_x, prev_y;
	
	float original_w, original_h;
	float scale_w;
	float scale_h;

	float cur_scale_w;
	float cur_scale_h;

	Color clr;
	float parent_alpha;
	
	NGUIWidget* parent;

	std::vector<PrintCall> text_draws;
	vector<DrawCall> draws;	

	float max_depth, min_depth;
	bool  need_sort;					
	
	bool deactivate_childs;

	TextureInstance* texMask;

	static NGUIWidget*  mscover_widget;
	static NGUIWidget*  mscover_widget_not_covered;
	static NGUIWidget*  focusedWidget;
	static float ms_pos_x;
	static float ms_pos_y;
	static float ms_prev_pos_x;
	static float ms_prev_pos_y;
	static int editor_offset_x;
	static int editor_offset_y;
	
	static int num_updates;

	char script_class_name[128];

    char sParentName[128];

	PROP_START
	FLOAT_PROP(NGUIWidget, x, 0, "Transform", "x")
	FLOAT_PROP(NGUIWidget, y, 0, "Transform", "y")

	FLOAT_PROP(NGUIWidget, height, 25, "Transform", "height")	
	FLOAT_PROP(NGUIWidget, width, 100, "Transform", "width")
	
	FLOAT_PROP(NGUIWidget, rot_x, 0, "Transform", "rot_x")
	FLOAT_PROP(NGUIWidget, rot_y, 0, "Transform", "rot_y")
	FLOAT_PROP(NGUIWidget, rot_z, 0, "Transform", "rot_z")

	BOOL_PROP(NGUIWidget, scaleChilds, false, "Properties", "scaleChilds")
	
	BOOL_PROP(NGUIWidget, alignByParent, true, "Transform", "alignByParent")

	ENUM_PROP(NGUIWidget, horzAlign, 0, "Transform", "horz_align")		
		ENUM_ITEM("left", 0)
		ENUM_ITEM("center", 1)
		ENUM_ITEM("right", 2)
	ENUM_END(NGUIWidget, horzAlign)
	
	ENUM_PROP(NGUIWidget, vertAlign, 3, "Transform", "vert_align")		
		ENUM_ITEM("top", 3)
		ENUM_ITEM("center", 1)
		ENUM_ITEM("bottom", 4)
	ENUM_END(NGUIWidget, vertAlign)
		
	BOOL_PROP(NGUIWidget, clipChilds, false, "Properties", "clip_childs")
	STRING_PROP(NGUIWidget, loc_text, "", "Text", "Text")	

	ENUM_PROP(NGUIWidget, text_align, 0, "Text", "text_align")
		ENUM_ITEM("left", 0)
		ENUM_ITEM("center", 1)
		ENUM_ITEM("right", 2)
	ENUM_END(NGUIWidget, text_align)

	ENUM_PROP(NGUIWidget, vtext_align, 0, "Text", "vrt_text_align")
		ENUM_ITEM("top", 0)
		ENUM_ITEM("center", 1)
		ENUM_ITEM("bottom", 2)
	ENUM_END(NGUIWidget, vtext_align)

	COLOR_PROP(NGUIWidget, overColor, 0xffffffff, "Visual", "overColor")
	FLOAT_PROP(NGUIWidget, alpha, 1.0f, "Visual", "alpha")

	FLOAT_PROP(NGUIWidget, anchor_x, 0.5f, "Anchorn", "anchor_x")
	FLOAT_PROP(NGUIWidget, anchor_y, 0.5f, "Anchorn", "anchor_y")
	BOOL_PROP(NGUIWidget, abs_anchor, true, "Anchorn", "AnchAbsUnits")	

	BOOL_PROP(NGUIWidget, cellMove, false, "Properties", "CellMove")

	BOOL_PROP(NGUIWidget, allow_mouse_cover, true, "Properties", "AllowMouseCover")	
	
	std::vector<NGUIWidget*> childs;
	std::vector<Str128> childs_names;
		
	std::vector<FontRes::LineData> line_indexes;
	std::string text;	

	FontInstance* font;

	std::vector<NGUIWidget*> sub_drawers;

	int weight;
	bool is_symbol;

#if defined(PC) || defined(OSUNIX)
	char parent_path[2048];
    void SetParentPath();
    const char* GetFullName(); 
#endif

#ifdef EDITOR
	TextureInstance* dot_tex;
	TextureInstance* empty_tex;
#endif

	ENTITY_CONSTRUCT(NGUIWidget, SceneEntity)

	virtual ~NGUIWidget();

	virtual void RegisterProperties();
	virtual void Init();
	virtual void ReInit();
	
    virtual bool HaveDifferences(PropertiesOwner* holder);

	virtual void LoadProperties(IStreamLoader* loader);
	virtual void SaveProperties(IStreamSaver* saver);
	virtual void SaveDiffProperties(IStreamSaver* saver, PropertiesOwner* holder);

	virtual void Copy(PropertiesOwner* propHolder);
    virtual SceneEntity* Clone();

	virtual bool Start();
	virtual void Finish();

	virtual void Show(bool set_show);

	virtual bool IsFocused();
	virtual void SetFocused(bool focus);

	bool NWgtIsFocused();
	void NWgtSetFocused(bool focus);

    virtual void AddChild(NGUIWidget* child);
    virtual void DelChild(NGUIWidget* child);
    virtual void InsertChild(NGUIWidget* child,unsigned nPos);
	   
	void Render(float dt, int level);
	virtual void RenderWidget() {};
	virtual void FullRender();
	
	virtual void CalcColors();
	virtual void CalcHorzPos();
	virtual void CalcVertPos();
	virtual void CalcChildsGlobalMat();
	virtual void UpdateChildsPos();
	virtual void CheckMouseCover();	

	void NWgtScriptSetAlpha(float set_x);
	float NWgtScriptGetAlpha();

	void NWgtScriptSetX(float set_x);
	float NWgtScriptGetX();

	virtual void NWgtScriptSetY(float set_y);
	float NWgtScriptGetY();

	virtual void NWgtScriptSetWidth(float w);
	float NWgtScriptGetWidth();

	virtual void NWgtScriptSetHeight(float h);
	float NWgtScriptGetHeight();

	virtual void NWgtScriptSetRotx(float rotx);
	float NWgtScriptGetRotx();

	virtual void NWgtScriptSetRoty(float roty);
	float NWgtScriptGetRoty();

	virtual void NWgtScriptSetRotz(float rotz);
	float NWgtScriptGetRotz();	

	virtual void SetText(const char* txt);
	virtual void NWgtSetTextScript(const string& txt);	
	virtual void NWgtGetTextScript(std::string& val);	

	virtual bool CheckClassName(const char* clsName);

	virtual bool AllowRegisterSelfToScript();
	virtual void MethodsToScript(const char* script_class_name);	
	virtual void FullMethodsToScript(const char* class_name);
	virtual void MethodsToScriptSub(const char* class_name);

	virtual bool IsActive();
	virtual bool IsVisible();

	float   ConvertHorz(float sz);
	float   ConvertVert(float sz);

	float   Convert2AbsHorz(float sz);
	float   Convert2AbsVert(float sz);

	float NWgtGetAbsoluteX();
	float NWgtGetAbsoluteY();	

#ifdef PC
	virtual void SetGizmo(Gizmo* gz);
	virtual void DataToGizmo();
	virtual void DataFromGizmo();	
	virtual bool Select2D(float mx, float my, SceneEntity* cur_slection);
#endif

	virtual void AddDrawCall(Texture* tex, float set_scr_x, float set_scr_y, float wdt, float hgt, Color color, float u, float v, float du, float dv);	
	virtual void AddPrintCall(FontInstance* font, float scr_x, float scr_y, float scr_w, float scr_h, std::vector<FontRes::LineData>* line_indices, float font_scale, Color color, const char* txt_text);		

	virtual void AddDrawCallLevel(float depth, Texture* tex, float set_scr_x, float set_scr_y, float wdt, float hgt, Color color, float u, float v, float du, float dv);

	virtual void ExecuteDrawCalls();

	virtual bool AllowMouseCover();	

	virtual bool IsAffectChildDrawCall();
	virtual void AffectChildDrawCall(float& x, float& y, float w, float h, float& a);

	virtual void GetClipRect(float& clip_x, float& clip_y, float& clip_x2, float& clip_y2);

	virtual float GetBorderVert();
	virtual float GetBorderHorz();

	void NWgtSetColorScript(float r, float g, float b);
    void NWgtSetColorStringScript(std::string& txt);
	void NWgtGetColorScript(float& r, float& g, float& b);
	
	virtual float NWgtGetCurTextWidth();	
	virtual float NWgtGetCurTextHeight();	
	virtual int NWgtGetCurTextLinesCount();	

	virtual float GetTextWidth(std::string& txt);	
	virtual float GetTextHeight(std::string& txt);	
	virtual int GetTextLinesCount(std::string& txt);	

	float NWgtGetTextWidth(std::string& txt);	
	float NWgtGetTextHeight(std::string& txt);	
	int NWgtGetTextLinesCount(std::string& txt);	

	void NWgtGetNameScript(std::string& nm);	

	virtual bool IsChild(NGUIWidget* wgt);	

	virtual bool IsSymbol();	

	void FixTextures(TextureInstance* tex, TextureInstance* prev_tex);
	void SetTextureData(TextureEntry::TextureData* data, const char* name);	

	virtual void CalcLinesData();

	void Embed2ScriptBaseClass();

	virtual void ModifyTextClipRect(ClipRect& rect);
#ifdef EDITOR
	virtual void SetEditMode(bool ed);
	virtual Color GetDebugColor();
	virtual bool UpdateEditorWidgets();
#endif

	void GetTextureFileName(TextureEntry::TextureData* data, std::string& name);	
	virtual void SetTextureDataURL(TextureEntry::TextureData* texture, const char* url, float url_fade_time);			

	virtual bool IsHitted(float mx, float my);
	virtual bool IsHitted(float mx, float my, float x, float y, float w, float h);
		
	virtual NGUIWidget* GetRoot();	

	int NWgtGetParentIndexScript();
	void NWgtSetParentIndexScript(int index);

	virtual float GetScaleX();
	virtual float GetScaleY();
	virtual void Scale(float scale_x, float scale_y);
	virtual void Release();	

	virtual void NotifyVisibilityChange(bool vis);

	static void ScrollerWork(NGUIWidget* widget, ScrollerData& data, float dt, int scrollerType, bool allow_scroll);

#if defined(PC) || defined(OSUNIX)
    static HashTable<string,string> g_vSignatures;
    static void BuildSignatures();
    static void GetMembersForAutoComplete(std::vector<string>& vMembers);
    static void GetSignatureForAutoComplete(const string& sMethod,std::vector<string>& vSignatures);
#endif
};