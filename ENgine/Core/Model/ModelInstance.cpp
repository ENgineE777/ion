#include "Core/Core.h"

ModelInstance::ModelInstance(ModelRes* rs)
{
	res = rs;
}

void ModelInstance::Draw(RenderTechnique* rt)
{	
	/*if (animation)
	{
		for (int i=0;i<res->mesh.num_bones;i++)
		{
			Matrix mat = res->mesh.bones[i].bindMatrix * (*animation->GetBoneMatrix(i));
			//mat.Identity();
			//mat.Transposition();

			memcpy(&animBones[i*3].x, &mat._11, 4 * 4 * 4);
		}

		rt->VS_SetMatrixArray("animBones", res->mesh.num_bones, &animBones[0].x);	

		/*for (int i=0;i<res->mesh.num_bones;i++)
		{
			if (res->mesh.bones[i].parent == -1) continue;

			Matrix* m1 = animation->GetBoneMatrix(i);
			Matrix* m2 = animation->GetBoneMatrix(res->mesh.bones[i].parent);

			en_core.Render()->DrawLine(m1->Pos() * transform, COLOR_GREEN,
								    m2->Pos() * transform, COLOR_GREEN);
		}
	}
	else
	{
		int vertices_offest = 0;

		for (int i=0;i<res->num_meshes;i++)
		{
			for (int j=0;j<res->meshes[i].num_vertices;j++)
			{			
				en_core.Render()->DrawLine(res->meshes[i].vertices[j].pos * trans, COLOR_GREEN, (res->meshes[i].vertices[j].pos + res->meshes[i].vertices[j].normal * 0.1f) * trans, COLOR_GREEN);
			}
			
			vertices_offest += res->meshes[i].num_vertices;
		}
	}*/

	res->Draw(transform, rt);
}

void ModelInstance::SetTransform(Matrix& trans)
{
	transform = trans;
}

void ModelInstance::SetScale(Vector &scale)
{

}

void ModelInstance::Release()
{
	res->ref_count--;

	if (res->ref_count==0) res->Release();
	delete this;
}
