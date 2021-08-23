
#include "FxRes.h"
#include "Common/CrossString.h"
#include "Core/Files/StreamLoader.h"
#include "Core/Files/StreamSaver.h"

/*
bool CFxRes::Load( const char* _fileName)
{
	String::Copy(m_fileName, sizeof(m_fileName), _fileName);

	CLuaTable table;
	if (!table.LoadScript(m_fileName))
		return false;

	m_Descs.reserve(3);

	if (table.GetTable("fx"))
	{
		for(dword i = 0; i < table.GetNumElems(); ++i)
		{
			m_Descs.push_back(SEmiterDesc());
			table.GetTable(i);

			const char* texName;
			table.Get("TextureName", texName);
			String::Copy(m_Descs.back().TextureName, 256, texName);
			table.Get("Looped", m_Descs.back().Looped);
			table.Get("ParticlesCount", m_Descs.back().ParticlesCount);
			table.Get("SpawnRate", m_Descs.back().SpawnRate);
			table.Get("SpawnTime", m_Descs.back().SpawnTime);
			table.Get("SizeMax", m_Descs.back().SizeMax);
			table.Get("SizeMin", m_Descs.back().SizeMin);
			table.Get("LifeTimeMin", m_Descs.back().LifeTimeMin);
			table.Get("LifeTimeMax", m_Descs.back().LifeTimeMax);
			//Matrix Transform;
			table.GetVector("PositionMin", m_Descs.back().PositionMin);
			table.GetVector("PositionMax", m_Descs.back().PositionMax);
			table.GetVector("VelocityMin", m_Descs.back().VelocityMin);
			table.GetVector("VelocityMax", m_Descs.back().VelocityMax);
			table.GetVector("AccelerationMin", m_Descs.back().AccelerationMin);
			table.GetVector("AccelerationMax", m_Descs.back().AccelerationMax);

			table.Return();

		}
	}

	return true;
}
*/
bool CFxRes::Load( const char* _fileName)
{
	String::Copy(m_fileName, sizeof(m_fileName), _fileName);

	StreamLoader* loader = new StreamLoader();
	if (!loader->Init(_fileName, "fx", StreamLoader::xml))
		return false;

	int num_emiters = 0;
	loader->Read("num_emiters", num_emiters);
	m_Descs.reserve(num_emiters + 1000);

	loader->Read("type", (int&)m_type);
	


	for(int i = 0; i < num_emiters; ++i)
	{
		char blockName[64];
		sprintf( blockName, "emiter%d", i );
		if ( loader->EnterBlock(blockName) )
		{
			m_Descs.push_back(SEmiterDesc());

			loader->Read("TextureName", m_Descs.back().TextureName, 256);
			loader->Read("Looped", m_Descs.back().Looped);
			loader->Read("LifeTime", m_Descs.back().LifeTime);
			loader->Read("ParticlesCount", m_Descs.back().ParticlesCount);
			loader->Read("SpawnRate", m_Descs.back().SpawnRate);
			loader->Read("SpawnTime", m_Descs.back().SpawnTime);
			loader->Read("SizeMaxX", m_Descs.back().SizeMaxX);
			loader->Read("SizeMinX", m_Descs.back().SizeMinX);
			loader->Read("SizeMaxY", m_Descs.back().SizeMaxY);
			loader->Read("SizeMinY", m_Descs.back().SizeMinY);
			loader->Read("LifeTimeMin", m_Descs.back().LifeTimeMin);
			loader->Read("LifeTimeMax", m_Descs.back().LifeTimeMax);
			//Matrix Transform;
			loader->Read("PositionMin", m_Descs.back().PositionMin);
			loader->Read("PositionMax", m_Descs.back().PositionMax);
			loader->Read("VelocityMin", m_Descs.back().VelocityMin);
			loader->Read("VelocityMax", m_Descs.back().VelocityMax);
			loader->Read("AccelerationMin", m_Descs.back().AccelerationMin);
			loader->Read("AccelerationMax", m_Descs.back().AccelerationMax);
			loader->Read("Fade", m_Descs.back().Fade);
			loader->Read("Color0", m_Descs.back().Color[0]);
			loader->Read("Color1", m_Descs.back().Color[1]);
			loader->Read("Color2", m_Descs.back().Color[2]);
			loader->Read("Color3", m_Descs.back().Color[3]);
			loader->Read("scaleSizePerSec", m_Descs.back().scaleSizePerSec);

			m_Descs.back().type = m_type;

			loader->LeaveBlock();

		}
	}

	loader->Release();
	return true;
}

bool CFxRes::Save( const char* _fileName)
{
	StreamSaver* saver = new StreamSaver();
	if (!saver->Init(_fileName, StreamSaver::xml))
		return false;

	saver->MarkBeginBlock("fx");

	int num_emiters = m_Descs.size();
	saver->Write("num_emiters", num_emiters);
	saver->Write("type", m_type);


	for(int i = 0; i < num_emiters; ++i)
	{
		char blockName[64];
		sprintf( blockName, "emiter%d", i );
		saver->MarkBeginBlock(blockName);
		SEmiterDesc& desc =  m_Descs[i];

		saver->Write("TextureName", desc.TextureName);
		saver->Write("Looped", desc.Looped);
		saver->Write("LifeTime", desc.LifeTime);
		saver->Write("ParticlesCount", desc.ParticlesCount);
		saver->Write("SpawnRate", desc.SpawnRate);
		saver->Write("SpawnTime", desc.SpawnTime);
		saver->Write("SizeMaxX", desc.SizeMaxX);
		saver->Write("SizeMinX", desc.SizeMinX);
		saver->Write("LifeTimeMin", desc.LifeTimeMin);
		saver->Write("SizeMaxY", desc.SizeMaxY);
		saver->Write("SizeMinY", desc.SizeMinY);

		saver->Write("LifeTimeMax", desc.LifeTimeMax);
		//Matrix Transform;
		saver->Write("PositionMin", desc.PositionMin);
		saver->Write("PositionMax", desc.PositionMax);
		saver->Write("VelocityMin", desc.VelocityMin);
		saver->Write("VelocityMax", desc.VelocityMax);
		saver->Write("AccelerationMin", desc.AccelerationMin);
		saver->Write("AccelerationMax", desc.AccelerationMax);
		saver->Write("Fade", desc.Fade);
		saver->Write("Color0", desc.Color[0]);
		saver->Write("Color1", desc.Color[1]);
		saver->Write("Color2", desc.Color[2]);
		saver->Write("Color3", desc.Color[3]);
		saver->Write("scaleSizePerSec", desc.scaleSizePerSec);

		saver->MarkEndBlock(blockName);

	}

	saver->MarkEndBlock("fx");

	saver->Release();

	return true;
}


void CFxRes::Release()
{

}




