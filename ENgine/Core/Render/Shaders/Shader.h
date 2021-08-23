
#pragma once

#include "Common/Common.h"
#include "Core/Resources/Resources.h"

class Shader : public BaseResource
{	
protected:	

	bool dirty;

public:

	Shader(const char* file) : BaseResource(file) { dirty = false; };	
	virtual ~Shader() {};

	virtual bool ParamExist(const char* param) { return false; };

	virtual bool SetVector4(const char* param, const Vector4& v) { return true; };
	virtual bool SetVector4Array(const char* param, int num, float* v) { return true; };	

	virtual bool SetMatrix(const char* param, const Matrix& mat) { return true; };
	virtual bool SetMatrixArray(const char* param, int num, float* mat) { return true; };

	virtual void ActivateParams(bool act){};
	virtual void UpdateConstants() {};
	virtual void UpdateTextures() {};

	void SetDirty() { dirty = true; };
	virtual void* GetData() = 0;	
	virtual void Release() = 0;
};