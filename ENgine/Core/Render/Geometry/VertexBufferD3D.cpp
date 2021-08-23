
#include "VertexBufferD3D.h"
#include "Core/Core.h"

struct Decl
{
	IDirect3DVertexDeclaration9* decl;
	int num_elems;
	D3DVERTEXELEMENT9 Elements[16];
};

int num_decl = 0;
Decl declarations[100];

D3DVERTEXELEMENT9 Elements[16];

static const _D3DDECLTYPE g_NormalDeclType = D3DDECLTYPE_SHORT4N;
	
struct Normal4
{
	short x;
	short y;
	short z;
	short w;
};

VertexBufferD3D::VertexBufferD3D(int sz, int strd) : VertexBuffer(sz, strd)
{	
	vertexBuffer = NULL;

	if (RenderServiceD3D::instance->IsUseDummyRender())
	{
		dummy_data = (byte*)malloc(size);		
	}
	else
	{
		DWORD flag = D3DUSAGE_WRITEONLY;

		if ( FAILED(RenderServiceD3D::instance->device->CreateVertexBuffer(size, flag, NULL, D3DPOOL_MANAGED, &vertexBuffer, NULL ) ) )
		{
			core->TraceTo( "Render", "create vertex buffer failed" );
		}

		elem_index = 0;
		elem_offset = 0;
		elem_ps_index = 0;
		elem_tx_index = 0;
		elem_clr_index = 0;
		decl = NULL;
	}
}

void* VertexBufferD3D::Lock()
{
	if (RenderServiceD3D::instance->IsUseDummyRender()) return dummy_data;

	float* data = NULL;
	vertexBuffer->Lock(0, 0, (VOID**)&data, 0);

	return data;
}

void VertexBufferD3D::Unlock()
{
	if (RenderServiceD3D::instance->IsUseDummyRender()) return;

	vertexBuffer->Unlock();
}

void VertexBufferD3D::StartDesc()
{	
}

void VertexBufferD3D::AddDescElem(ElemUsage usage, ElemType type, const char* name)
{
	if (RenderServiceD3D::instance->IsUseDummyRender()) return;

	int tp = 0;
	int usg = 0;
	int sz = 0;

	switch (usage)
	{
		case position:
		{
			usg = D3DDECLUSAGE_POSITION;
		}
		break;
		case positiont:
		{
			usg = D3DDECLUSAGE_POSITIONT;
		}
		break;		
		case color:
		{
			usg = D3DDECLUSAGE_COLOR;
		}
		break;
		case normal:
		{
			usg = D3DDECLUSAGE_NORMAL;
		}
		break;
		case binormal:
		{
			usg = D3DDECLUSAGE_BINORMAL;
		}
		break;
		case tangent:
		{
			usg = D3DDECLUSAGE_TANGENT;
		}
		break;
		case texcoord:
		{
			usg = D3DDECLUSAGE_TEXCOORD;
		}
		break;

		case blendindex:
		{
			usg = D3DDECLUSAGE_BLENDINDICES;
		}
		break;

		case blendweight:
		{
			usg = D3DDECLUSAGE_BLENDWEIGHT;
		}
		break;
	}
	

	switch (type)
	{
		case float2:
		{
			tp = D3DDECLTYPE_FLOAT2;
			sz = sizeof(Vector2);
		}
		break;
		case float3:
		{
			tp = D3DDECLTYPE_FLOAT3;
			sz = sizeof(Vector);
		}
		break;
		case float4:
		{
			tp = D3DDECLTYPE_FLOAT4;
			sz = sizeof(Vector4);
		}
		break;
		case tp_color:
		{
			tp = D3DDECLTYPE_D3DCOLOR;
			sz = 4;
		}
		break;		
		case ubyte4:
		{
			tp = D3DDECLTYPE_UBYTE4;
			sz = sizeof(dword);
		}
		break;
		case short4:
		{
			tp = D3DDECLTYPE_SHORT4N;
			sz = 2 * sizeof(float);
		}
		break;
		case dec4:
		{
			tp = g_NormalDeclType;
			sz = sizeof(Normal4);
		}
		break;
		case short2:
			{
				tp = D3DDECLTYPE_SHORT2N;
				sz =sizeof(float);
			}
			break;
	}
	
	int sub_index = 0;
	if (usage == position) sub_index = elem_ps_index;
	if (usage == positiont) sub_index = elem_ps_index;
	if (usage == texcoord) sub_index = elem_tx_index;
    if (usage == color) sub_index = elem_clr_index;
	
	D3DVERTEXELEMENT9 Elem = {0, elem_offset, tp, D3DDECLMETHOD_DEFAULT, usg, sub_index};
	Elements[elem_index] = Elem;
	elem_offset += sz;
	elem_index++;	

	if (usage == position) elem_ps_index++;	
	if (usage == positiont) elem_ps_index++;	
    if(usage == color) elem_clr_index++;
	if (usage == texcoord) elem_tx_index++;	
}

void VertexBufferD3D::EndDesc()
{
	if (RenderServiceD3D::instance->IsUseDummyRender()) return;

	D3DVERTEXELEMENT9 Elem = D3DDECL_END();
	Elements[elem_index] = Elem;


	for (int i=0;i<num_decl;i++)
	{
		if (declarations[i].num_elems == elem_index+1)
		{
			bool tested = true;
			for (int j=0;j<declarations[i].num_elems;j++)
			{
				if (declarations[i].Elements[j].Stream != Elements[j].Stream ||
					declarations[i].Elements[j].Offset != Elements[j].Offset ||
					declarations[i].Elements[j].Type != Elements[j].Type ||
					declarations[i].Elements[j].Method != Elements[j].Method ||
					declarations[i].Elements[j].Usage != Elements[j].Usage ||
					declarations[i].Elements[j].UsageIndex != Elements[j].UsageIndex)
				{
					tested = false;
					break;
				}
			}

			if (tested)
			{
				decl = declarations[i].decl;
				return;
			}
		}
	}

	declarations[num_decl].num_elems = elem_index+1;

	for (int j=0;j<declarations[num_decl].num_elems;j++)
	{
		declarations[num_decl].Elements[j] = Elements[j];
	}			

	RenderServiceD3D::instance->device->CreateVertexDeclaration((const D3DVERTEXELEMENT9*)Elements, &declarations[num_decl].decl);	

	decl = declarations[num_decl].decl;
	num_decl++;
}

void* VertexBufferD3D::GetData()
{	
	if (decl) RenderServiceD3D::instance->SetVertexDeclaration(decl);
	return vertexBuffer;
}

void VertexBufferD3D::Release()
{
	if (!RenderServiceD3D::instance->IsUseDummyRender())
	{		
		RenderServiceD3D::instance->SetVertexBuffer(0, null);	

		if (vertexBuffer)
		{
			vertexBuffer->Release();
			vertexBuffer = null;
		}
	
		RenderServiceD3D::instance->SetVertexDeclaration(null);		
	}
	else
	{
		free(dummy_data);
	}

	RenderServiceD3D::instance->DeleteVBuffer(this);

	delete this;
}