
#include "ShaderInfoD3D.h"
#include "Core/Core.h"

ShaderInfoD3D::ShaderInfoD3D() : constantsInfo(_FL_), samplersInfo(_FL_)
{
	totalConstants = 0;
	constants = null;
}

void ShaderInfoD3D::ParseShader(byte* buffer)
{
	constants = null;

	ID3DXConstantTable* constantTable = null;
	
	D3DXGetShaderConstantTable((const dword*)buffer, &constantTable);

	int index = 0;
	D3DXHANDLE handle = 0;
	totalConstants = 0;

	int num_consts = 0;
	D3DXCONSTANT_DESC constDesc[24];

	do
	{		
		handle = constantTable->GetConstant(NULL,index);
		if (handle == 0) break;

		D3DXCONSTANT_DESC desc;
		UINT size = 1;
		constantTable->GetConstantDesc(handle, &desc, &size);

		if (desc.RegisterSet != D3DXRS_SAMPLER)
		{
			num_consts++;
			constDesc[num_consts-1] = desc;
		}
		else
		{
			ShaderParamInfo* spInfo = new ShaderParamInfo;
			spInfo->reg = desc.RegisterIndex;
			spInfo->regCount = desc.RegisterCount;
			spInfo->elementsCount = desc.Elements;
			
			samplersInfo.Add(desc.Name, spInfo);
		}

		index++;
	}
	while (handle != 0);

	for (int i = 0; i<num_consts-1;i++)
	{
		for (int j = i+1; j<num_consts;j++)
		{
			if (constDesc[i].RegisterIndex > constDesc[j].RegisterIndex)
			{
				D3DXCONSTANT_DESC tmp = constDesc[i];
				constDesc[i] = constDesc[j];
				constDesc[j] = tmp;				
			}			
		}
	}

	for (int i = 0; i<num_consts;i++)
	{
		ShaderParamInfo* spInfo = constantsInfo.Add(constDesc[i].Name);

		spInfo->reg = constDesc[i].RegisterIndex;
		spInfo->regCount = constDesc[i].RegisterCount;
		spInfo->elementsCount = MathMin(constDesc[i].Elements, constDesc[i].RegisterCount);
		spInfo->dirty = false;

		int lastNumber = constDesc[i].RegisterIndex + constDesc[i].RegisterCount;
		if(totalConstants < lastNumber) totalConstants = lastNumber;
	}

	if (totalConstants)
	{
		totalConstants = ((totalConstants + 4)/4)*4;
		constants = new Vector4[totalConstants];

		for (int i = 0; i < totalConstants; i++)
		{
			constants[i] = Vector4(0,0,0,0);
		}
	}

	if (constantTable)
	{
		constantTable->Release();
		constantTable = null;
	}  
}

bool ShaderInfoD3D::ParamExist(const char* param)
{
	return (constantsInfo.Get(param) != null);
}

bool ShaderInfoD3D::SetVector4(const char* param, const Vector4& v) 
{
    ShaderParamInfo* spInfo = constantsInfo.Get(param);

    if(spInfo == 0)
	{
		return false;
	}

	spInfo->setted_reg = 1;
    constants[spInfo->reg] = v;
    spInfo->dirty = true;

    return true; 
}

bool ShaderInfoD3D::SetVector4Array(const char* param, int num, float* v) 
{
    ShaderParamInfo* spInfo = constantsInfo.Get(param);

    if (spInfo == 0)
	{
		return false;
	}

	spInfo->setted_reg = MathMin(num,spInfo->elementsCount);
    memcpy((void*)&constants[spInfo->reg], v, sizeof(float) * 4 * spInfo->setted_reg);
	spInfo->dirty = true;

    return true;
}

bool ShaderInfoD3D::SetMatrix(const char* param, const Matrix& mat)
{ 
    ShaderParamInfo* spInfo = constantsInfo.Get(param);

    if(spInfo == 0)
	{
		return false;
	}

	spInfo->setted_reg = spInfo->regCount;
	if (spInfo->setted_reg>4) spInfo->setted_reg = 4;

	Matrix tmp = mat;
	tmp.Transposition();

    memcpy((void*)&constants[spInfo->reg], &tmp, sizeof(float) * 4 * 4);
    spInfo->dirty = true;	

    return true; 
}

bool ShaderInfoD3D::SetMatrixArray(const char* param, int num, float* mat)
{
    ShaderParamInfo* spInfo = constantsInfo.Get(param);

    if (spInfo == 0)
	{
		return false;
	}

    spInfo->setted_reg = (spInfo->regCount/spInfo->elementsCount) * MathMin(num, spInfo->elementsCount);
    memcpy((void*)&constants[spInfo->reg], mat, sizeof(float) * 4 * spInfo->setted_reg);
    spInfo->dirty = true;	

    return true; 
}

void ShaderInfoD3D::UpdateConstants(bool is_vs)
{
	int from = -1;
	int to = 1;

	for (int i=0;i<constantsInfo.Size();i++)
	{
		ShaderParamInfo* spInfo = constantsInfo.Get(i);

		if (spInfo->dirty)
		{					
			if (from == -1)
			{
				from = spInfo->reg;	
				to = from + spInfo->setted_reg;
			}			
			
			//if (spInfo->reg > to + 1)
			//{
				if((to - from) != 0)
				{
					SetConstants(is_vs, from, (float*)&constants[from], to - from);
				}
				else
				{
					//core->TraceTo("render", "Constant not setted. Shader: %s Reg: %d RegCount: %d Constant Name: %s", GetFileName(), spInfo->reg, spInfo->regCount, constantsInfo.GetName(spInfo));
				}
				
				from = -1;
			//}
			/*else
			{
				to = spInfo->reg + spInfo->setted_reg;				
			}*/
			
			spInfo->dirty = false;		
		}
    }

	if (from != -1)
	{
		SetConstants(is_vs, from, (float*)&constants[from], to - from);	
	}
}

void ShaderInfoD3D::SetConstants(bool is_vs, int index, float* ptr, int count)
{
	if (is_vs)
	{
		RenderServiceD3D::instance->device->SetVertexShaderConstantF(index, ptr, count);
	}
	else
	{
		RenderServiceD3D::instance->device->SetPixelShaderConstantF(index, ptr, count);
	}
}