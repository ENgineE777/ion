#include "Core/Core.h"
#include "ModelRes.h"
#include "ModelFbxLoader.h"

ModelRes::ModelRes()
{
	ref_count = 0;
	md_name[0] = 0;	

	vbuffer = null;
	ibuffer = null;

	buffer = null;
}

bool ModelRes::Load(const char* name)
{		
	String::Copy(md_name, 64, name);
	
	/*char file_path[512];
	strcpy(file_path, name);
	
	FileBuffer* file_buffer = null;
	
	if (file_buffer) return false;

	file_buffer = core->Files()->LoadFile(name);

	byte* buffer = file_buffer->GetBuffer();	

	file_buffer->Release();*/

#ifdef FBX
	if (ModelFbxLoader::Load(this, name))
	{
		if (mesh.num_meshes>0)
		{	
			en_core.EN_Files()->RegFile(name);

			int vrtx_sz = sizeof(Mesh::Vertex);

			if (mesh.type == Mesh::animated_mesh)
			{
				vrtx_sz = sizeof(Mesh::VertexAnim);
			}

			vbuffer = en_core.Render()->CreateVertexBuffer(mesh.num_vertices * vrtx_sz, vrtx_sz,false, _FL_);			

			vbuffer->StartDesc();
	
			vbuffer->AddDescElem(VertexBuffer::position,VertexBuffer::float3, "position");		
			vbuffer->AddDescElem(VertexBuffer::normal,VertexBuffer::float3, "normal");	
			vbuffer->AddDescElem(VertexBuffer::texcoord,VertexBuffer::float2, "tex");	
	
			if (mesh.type == Mesh::animated_mesh)
			{
				vbuffer->AddDescElem(VertexBuffer::blendindex,VertexBuffer::ubyte4, "indices");	
				vbuffer->AddDescElem(VertexBuffer::blendweight,VertexBuffer::float4, "weights");
			}

			vbuffer->EndDesc();	

			Mesh::Vertex* v = (Mesh::Vertex*)vbuffer->Lock(0);
			memcpy(v, mesh.vertices, vrtx_sz * mesh.num_vertices);
			vbuffer->Unlock();

			ibuffer = en_core.Render()->CreateIndexBuffer(mesh.num_indices, false, _FL_);
			
			int* indx = (int*)ibuffer->Lock(0);
			memcpy(indx, mesh.indices, sizeof(word) * mesh.num_indices);											
			ibuffer->Unlock();

			return true;
		}

	}
#endif

	if (LoadImpl(name))
	{
		return true;
	}

	return false;
}

bool ModelRes::LoadImpl(const char* name)
{
	IFileBuffer* file = core->Files()->LoadFile(name);

	if (!file) return false;

	byte* ptr = file->GetBuffer();

	if (*(dword*)&ptr[0] != Mesh::geomMagicID || *(dword*)&ptr[4] != Mesh::geomSubID)
	{
		file->Release();
		return false;
	}

	ptr += 8;

	memcpy(&mesh, ptr, sizeof(Mesh::MeshHolder));
	ptr += sizeof(Mesh::MeshHolder);

	int requared_size = mesh.num_meshes * sizeof(Mesh::Mesh) + mesh.num_materials * sizeof(Mesh::Material) +
		                mesh.num_bones * sizeof(Mesh::Bone);

	buffer = new byte[requared_size];

	memcpy(buffer, ptr, requared_size);
	ptr += requared_size;
	
	mesh.meshes = (Mesh::Mesh*)&buffer[0];	
	mesh.materials = (Mesh::Material*)&buffer[mesh.num_meshes * sizeof(Mesh::Mesh)];		
	mesh.bones = (Mesh::Bone*)&buffer[mesh.num_meshes * sizeof(Mesh::Mesh) + mesh.num_materials * sizeof(Mesh::Material)];	
	
	/*for (int i=0;i<mesh.num_bones;i++)
	{
		core->TraceTo("Models", mesh.bones[i].name);
	}*/
	
	int vrtx_sz = sizeof(Mesh::Vertex);

	if (mesh.type == Mesh::animated_mesh)
	{
		vrtx_sz = sizeof(Mesh::VertexAnim);
	}

	vbuffer = en_core.Render()->CreateVertexBuffer(mesh.num_vertices * vrtx_sz, vrtx_sz, _FL_);			

	vbuffer->StartDesc();

	vbuffer->AddDescElem(VertexBuffer::position,VertexBuffer::float3, "position");		
	vbuffer->AddDescElem(VertexBuffer::normal,VertexBuffer::float3, "normal");	
	vbuffer->AddDescElem(VertexBuffer::texcoord,VertexBuffer::float2, "tex");

	if (mesh.type == Mesh::animated_mesh)
	{
		vbuffer->AddDescElem(VertexBuffer::blendindex,VertexBuffer::ubyte4, "indices");	
		vbuffer->AddDescElem(VertexBuffer::blendweight,VertexBuffer::float4, "weights");
	}

	vbuffer->EndDesc();	

	Mesh::Vertex* v = (Mesh::Vertex*)vbuffer->Lock();
	memcpy(v, ptr, vrtx_sz * mesh.num_vertices);
	vbuffer->Unlock();

	ptr += vrtx_sz * mesh.num_vertices;

	ibuffer = en_core.Render()->CreateIndexBuffer(mesh.num_indices * sizeof(word), _FL_);

	int* indx = (int*)ibuffer->Lock();
	memcpy(indx, ptr, sizeof(word) * mesh.num_indices);											
	ibuffer->Unlock();
	
	file->Release();

	char path[512];
	String::ExtractPath(name, path, false);

	char txt[512];

	for (int i=0;i<mesh.num_materials;i++)
	{
		for (int j=0;j<mesh.materials[i].num_tex;j++)
		{
			String::Copy(txt, 256, path);
			String::Cat(txt, 256, &mesh.materials[i].textures_names[32*j]);

			mesh.materials[i].textures[j] = en_core.Render()->LoadTexture(txt, false, _FL_);
		}
	}

	return true;
}

#ifdef PC
void ModelRes::Save(const char* name, Platform platform)
{
	IFile* file = core->Files()->OpenFile(name, IFile::write);

	if (file)
	{
		file->Write(&Mesh::geomMagicID, sizeof(Mesh::geomMagicID));
		file->Write(&Mesh::geomSubID, sizeof(Mesh::geomSubID));

		file->Write(&mesh, sizeof(Mesh::MeshHolder));
		
		if (mesh.num_meshes>0)
		{
			file->Write(mesh.meshes, mesh.num_meshes * sizeof(Mesh::Mesh));
		}

		if (mesh.num_materials>0)
		{
			file->Write(mesh.materials, mesh.num_materials * sizeof(Mesh::Material));
		}

		if (mesh.num_bones>0)
		{
			file->Write(mesh.bones, mesh.num_bones * sizeof(Mesh::Bone));
		}

		if (mesh.num_vertices)
		{
			byte* v = null;

			if (mesh.type == Mesh::static_mesh)
			{
				v = (byte*)malloc(mesh.num_vertices * sizeof(Mesh::Vertex));
				memcpy(v, mesh.vertices, mesh.num_vertices * sizeof(Mesh::Vertex));

				if (platform == platform_ios ||
					platform == platform_android)
				{
					for (int i=0;i<mesh.num_vertices;i++)
					{
						Mesh::Vertex* vtx = (Mesh::Vertex*)&v[sizeof(Mesh::Vertex) * i];
						vtx->uv.y = 1-vtx->uv.y;
					}
				}

				file->Write(v, mesh.num_vertices * sizeof(Mesh::Vertex));
			}
			else
			{
				v = (byte*)malloc(mesh.num_vertices * sizeof(Mesh::VertexAnim));
				memcpy(v, mesh.vertices_anim, mesh.num_vertices * sizeof(Mesh::VertexAnim));

				if (platform == platform_ios ||
					platform == platform_android)
				{
					for (int i=0;i<mesh.num_vertices;i++)
					{
						Mesh::VertexAnim* vtx = (Mesh::VertexAnim*)&v[sizeof(Mesh::VertexAnim) * i];
						vtx->uv.y = 1-vtx->uv.y;
					}
				}

				file->Write(v, mesh.num_vertices * sizeof(Mesh::VertexAnim));
			}

			delete[] v;
		}

		if (mesh.num_indices)
		{
			file->Write(mesh.indices, mesh.num_indices * 2);
		}
		
		file->Release();
	}
}
#endif

ModelInstance* ModelRes::CreateReference()
{
	ref_count++;
	ModelInstance* md = new ModelInstance(this);
				
	return md;
}

void ModelRes::Draw(Matrix& transform, RenderTechnique* rt)
{
	//en_core.Render()->SetRenderTechnique(rt);

	Matrix view_inv;
	en_core.Render()->Display()->GetInvView(view_inv);

	Matrix view_proj;
	en_core.Render()->Display()->GetViewProj(view_proj);	

#ifdef PC
	transform.Vz() = -transform.Vz();
#endif
	
	view_proj = transform * view_proj;


	Matrix tr = transform;
	tr.Pos() = 0.0f;
	
	rt->VS_SetMatrix("modelMat", tr);

#ifdef PC
	transform.Vz() = -transform.Vz();
#endif	
	
	rt->PS_SetVector4("viewVec", Vector4(-view_inv.vz_x,-view_inv.vz_y,-view_inv.vz_z,0.0f));
	
	rt->VS_SetMatrix("modelViewProj", view_proj);

	en_core.Render()->SetVertexBuffer(0, vbuffer);	
	en_core.Render()->SetIndexBuffer(ibuffer);

	for (int i=0;i<mesh.num_meshes;i++)
	{
		Mesh::Material* mtr = &mesh.materials[mesh.meshes[i].material];
		
		for (int j=0;j<4;j++)
		{
			TextureInstance* tex = null;
			
			if (j<mtr->num_tex) tex = mtr->textures[j];
			
			if (j==0)
			{
				rt->PS_SetTexture("slot0", tex);
			}
			else
			if (j==1)
			{
				rt->PS_SetTexture("slot1", tex);
			}
			else
			if (j==2)
			{
				rt->PS_SetTexture("slot2", tex);
			}
			else
			if (j==3)
			{
				rt->PS_SetTexture("slot3", tex);
			}						
		}
			
		en_core.Render()->DrawIndexedPrimitive( RenderService::trianglesList, mesh.meshes[i].indices_offset, mesh.meshes[i].triangleCount);		
	}
}

void ModelRes::Release()
{
	en_core.Models()->RemoveRes(this);

	RELEASE(vbuffer)
	RELEASE(ibuffer)

	if (mesh.num_meshes && !buffer)
	{
		if (mesh.num_bones)
		{
			delete[] mesh.bones;
			delete[] mesh.vertices_anim;
		}
		else
		{
			delete[] mesh.vertices;
		}

		delete[] mesh.meshes;
		delete[] mesh.materials;		
	}	

	delete[] buffer;

	delete this;
}
