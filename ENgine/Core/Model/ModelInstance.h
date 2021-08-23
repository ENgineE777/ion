#pragma once

class ModelRes;
class RenderTechnique;

class DEF_EXPORT ModelInstance
{	
	ModelRes* res;
	Matrix transform;

public:	
		
	ModelInstance(ModelRes* rs);

	void Draw(RenderTechnique* rt);

	void SetTransform(Matrix& transform);
	void SetScale(Vector &scale);

	void Release();
};
