
#include "AnimationRes.h"
#include "AnimationInstance.h"
#include "AnimationService.h"

#if defined(ANIM_STANDALONE)
#include "tinyxml/tinyxml.h"

const char* ReadXML(TiXmlNode* node, const char* name)
{
	TiXmlElement* elem = node->FirstChildElement(name);
	if (elem)
	{
		if (elem->GetText())
		{
			return elem->GetText();
		}		
	}

	return "";
}

bool ReadBool(TiXmlNode* node, const char* name)
{
	TiXmlElement* elem = node->FirstChildElement(name);
	if (elem)
	{
		if (elem->GetText())
		{
			if (stricmp(elem->GetText(),"1") == 0) return true;			
		}		
	}

	return false;
}

float ReadFloat(TiXmlNode* node, const char* name)
{
	TiXmlElement* elem = node->FirstChildElement(name);
	if (elem)
	{
		if (elem->GetText())
		{
			return atof(elem->GetText());			
		}		
	}

	return -1.0f;
}

int ReadInt(TiXmlNode* node, const char* name)
{
	TiXmlElement* elem = node->FirstChildElement(name);
	if (elem)
	{
		if (elem->GetText())
		{
			return atoi(elem->GetText());			
		}		
	}

	return -1;
}
#endif

#if defined(ANIM_STANDALONE)
#include "windows.h"
#endif

#if !defined(ANIM_STANDALONE)
/*void ReadStr64(CLuaTable& table, const char* name, AnimationRes::Str64& str)
{
	table.Get(name, str.s, 64);	
}

void ReadStr1024(CLuaTable& table, const char* name, AnimationRes::Str1024& str)
{
	table.Get(name, str.s, 1024);	
}*/
#endif

AnimationRes::AnimationRes() : controllers(_FL_), nodes(_FL_), namesTables(_FL_), affectors(_FL_)
{
	ref_counter = 0;
}

void AnimationRes::AddRef()
{
	ref_counter++;
}

bool AnimationRes::DecRef()
{
	ref_counter--;
	if (ref_counter==0) return true;

	return false;
}

bool AnimationRes::Load(const char* name)
{
	String::Copy(startNode.s, 64, "Node");
	movementBone.s[0] = 0;	
	startTable.s[0] = 0;

	AnimNode* node = nodes.Add("Node");
	node->name.Copy("Node");

	node->getMovement = 0;
	node->topLayersPlayMode = 0;	

	Str64* nm = node->topLayers.Add("Layer");
	nm->Copy("Layer");

			
	ClipDesc* desc = node->layers.Add("Layer");

	desc->name.Copy("Layer");

	desc->clip_name.Init(name, "");				
	desc->numLoops = 9000;
			
	desc->playSpeed = 1.0f;	
	desc->mix = 0;
	desc->timeIn = 0.0f;
	desc->timeOut = 0.0f;
	desc->playMode = 0;		
	desc->blendMode = 0;
	desc->startWeight = 1.0f;
	desc->additiveBlend = 0;
						
	for (int k = 0;k<desc->clip_name.GetNum();k++)
	{
		if (!desc->clip_name.IsVirtual(k))
		{
			const char* clipName = desc->clip_name.GetName(k);
			
			if ( !clipName ) continue;
						
			if ( !clipName[0] )
			{
				//manager->Trace("%s: animgraph: \"%s\" node: \"%s\" - empty clip name\n", __FUNCTION__, graph_name.s, name.c_str());
				continue;
			}

			LoadClip(clipName, desc);
		}
	}

#if !defined(ANIM_STANDALONE)

/*	CLuaTable table;
		
	if (!table.LoadScript(name, _FL_, LUALIB_DOFILE, g_GameProxy.PackLoadMode ? "ag" : "lua"))
	{
		manager->Trace( "Wrong anim graph name \"%s\"", name );
		return false;	
	}

	graph_name.Copy(name);

	const char* SkeletonName = null;

	if (!table.GetCnst("Skeleton",SkeletonName))
	{
		manager->Trace( "Anim graph \"%s\" don't have skeleton name", name );
		return false;	
	}

	if ( SkeletonName[0] )
	{
		if (!PreLoad(SkeletonName)) return false;
	}

	if (table.GetTable("AnimGraph"))
	{		
		if(table.GetFirstTable())
		{
			do
			{
				std::string tableName = table.GetTableName();

				if (tableName == "Affectors")
				{
					if(table.GetFirstTable())
					{
						do
						{		
							std::string name = table.GetTableName();
							AffectorParam* affector = affectors.Add(name.c_str());
							affector->name.Copy(name.c_str());

							if(table.GetFirstTable())
							{
								do
								{
									std::string name = table.GetTableName();
									Param* param = affector->params.Add(name.c_str());
									param->name.Copy(name.c_str());

									table.Get("type",param->type);	

									if (param->type == 0)
									{
										table.Get("val",param->flt_val);
									}
									else
									{
										ReadStr64(table, "val",param->str_val);
									}									
								}
								while(table.GetNextTable());
							}																												
						}
						while(table.GetNextTable());
					}

				}
				else
					if (tableName == "Controllers")
					{
						if(table.GetFirstTable())
						{
							do
							{
								std::string id = table.GetTableName();
								Controller* cntrl = controllers.Add(id.c_str());
								cntrl->id.Copy(table.GetTableName().c_str());
								cntrl->boneName.Copy(cntrl->id.s);
								ReadStr64(table, "BoneName", cntrl->boneName);
								ReadStr64(table, "OffsetBone", cntrl->boneOffsetName);

								bool ik = false;
								if (table.Get("IK", ik))
								{
									if (ik) cntrl->type = IAnimationController::ik;
								}

								if (table.GetTable("MinConstraints"))
								{													
									table.Get("x",cntrl->min.x);								
									table.Get("y",cntrl->min.y);								
									table.Get("z",cntrl->min.z);								

									table.Return();
								}								

								if (table.GetTable("MaxConstraints"))
								{													
									table.Get("x",cntrl->max.x);								
									table.Get("y",cntrl->max.y);								
									table.Get("z",cntrl->max.z);								

									table.Return();
								}

								if (table.GetTable("PreRotation"))
								{
									table.Get("m11",cntrl->postRotMat._11);
									table.Get("m12",cntrl->postRotMat._12);
									table.Get("m13",cntrl->postRotMat._13);

									table.Get("m21",cntrl->postRotMat._21);
									table.Get("m22",cntrl->postRotMat._22);
									table.Get("m23",cntrl->postRotMat._23);

									table.Get("m31",cntrl->postRotMat._31);
									table.Get("m32",cntrl->postRotMat._32);
									table.Get("m33",cntrl->postRotMat._33);								

									table.Return();
								}	

								if (table.GetTable("BendDirection"))
								{
									table.Get("x",cntrl->bendDir.x);								
									table.Get("y",cntrl->bendDir.y);								
									table.Get("z",cntrl->bendDir.z);								

									table.Return();
								}

								table.Get("Speed",cntrl->speed);	
								table.Get("Active",cntrl->active);
							}
							while(table.GetNextTable());
						}
					}
					else
						if (tableName == "NamesTables")	
						{
							if(table.GetFirstTable())
							{						
								do
								{			
									std::string nm = table.GetTableName();
									NamesTable* nm_table = namesTables.Add(nm.c_str());
									nm_table->name.Copy(nm.c_str());							

									if(table.GetFirstTable())
									{
										do
										{		

											ClipDesc desc;

											if (table.GetTable("Channels"))
											{
												int index = 0;
												char str[32];
												sprintf(str, "Ch%i", index);		
												
												Str64 tmp;

												while (table.Get(str,tmp.s, 64))
												{													
													desc.channels.push_back(tmp);

													index++;
													sprintf(str, "Ch%i", index);
												}

												table.Return();
											}	

											Str64* clip = nm_table->clips.Add(table.GetTableName().c_str());
											ReadStr64(table, "clip", *clip);
											// elemets in namestable may hasnot clip
											if ( clip->s[0] )
											{
												LoadClip(clip->s, &desc);
											}
										}
										while(table.GetNextTable());
									}
								}
								while(table.GetNextTable());
							}
						}
						else
							if (tableName == "Nodes")					
							{			
								ReadStr64(table, "StartNode",startNode);								
								ReadStr64(table, "MovementBone", movementBone);					
								ReadStr64(table, "StartTable", startTable);

								if(table.GetFirstTable())
								{						
									do
									{																
										ReadNode(table);
									}
									while(table.GetNextTable());
								}
							}
			}
			while(table.GetNextTable());
		}
	}

	if (!ClipsLoaded())
	{
		manager->Trace("not loaded any clips %s", name);
		return false;
	}*/

#endif

#if defined(ANIM_STANDALONE)

	TiXmlDocument doc( name );

	if (!doc.LoadFile())
	{
		manager->Trace("Can't load file");
		return false;
	}

	TiXmlNode* animNode = doc.FirstChildElement("AnimNode");

	if (!animNode)
	{
		manager->Trace("Can't find Anim Node in xml");
		return false;
	}

	if (!PreLoad(ReadXML(animNode, "skeletonName")))
	{
		manager->Trace("Can't load xac file");
		return false;
	}

	movementBone.Copy(ReadXML(animNode, "movementBone"));

	startNode.Copy("node");
	AnimNode* node = nodes.Add("node");

	node->topLayersPlayMode = ReadBool(animNode, "Sequnce");
	node->getMovement = ReadBool(animNode, "Movement");

	TiXmlNode* controllerNode = animNode->FirstChild("Controller");

	while (controllerNode)
	{		
		std::string id = ReadXML(controllerNode, "id");

		Controller* cntrl = controllers.Add(id.c_str());
		cntrl->id.Copy(id.c_str());
		cntrl->boneName.Copy(ReadXML(controllerNode, "bone"));
		cntrl->boneOffsetName.Copy(ReadXML(controllerNode, "bone_offset"));

		cntrl->type = (IAnimationController::ControllerType)ReadInt(controllerNode, "type");

		cntrl->active = ReadBool(controllerNode, "Active");
		cntrl->speed = ReadFloat(controllerNode, "Speed");

		cntrl->min.x = ReadFloat(controllerNode, "min_x");
		cntrl->min.y = ReadFloat(controllerNode, "min_y");
		cntrl->min.z = ReadFloat(controllerNode, "min_z");

		cntrl->max.x = ReadFloat(controllerNode, "max_x");
		cntrl->max.y = ReadFloat(controllerNode, "max_y");
		cntrl->max.z = ReadFloat(controllerNode, "max_z");

		cntrl->postRotMat._11 = ReadFloat(controllerNode, "m11");
		cntrl->postRotMat._12 = ReadFloat(controllerNode, "m12");
		cntrl->postRotMat._13 = ReadFloat(controllerNode, "m13");		

		cntrl->postRotMat._21 = ReadFloat(controllerNode, "m21");
		cntrl->postRotMat._22 = ReadFloat(controllerNode, "m22");
		cntrl->postRotMat._23 = ReadFloat(controllerNode, "m23");		

		cntrl->postRotMat._31 = ReadFloat(controllerNode, "m31");
		cntrl->postRotMat._32 = ReadFloat(controllerNode, "m32");
		cntrl->postRotMat._33 = ReadFloat(controllerNode, "m33");		

		animNode->RemoveChild(controllerNode);
		controllerNode = animNode->FirstChild("Controller");
	}

	TiXmlNode* layerNode = animNode->FirstChild("Layer");

	while (layerNode)
	{		
		ClipDesc* desc = node->layers.Add(ReadXML(layerNode, "name"));

		desc->name.Copy(ReadXML(layerNode, "name"));				
		desc->clip_name.Init(ReadXML(layerNode, "clip_name"));

		desc->playSpeed = ReadFloat(layerNode, "playSpeed");			
		desc->timeIn = ReadFloat(layerNode, "timeIn");
		desc->timeOut = ReadFloat(layerNode, "timeOut");		
		desc->startWeight = ReadFloat(layerNode, "startWeight");		
		desc->playMode = ReadBool(layerNode, "backwardPlay");

		if (!ReadBool(layerNode, "looped"))
		{
			desc->numLoops = 1;
		}
		else
		{
			desc->numLoops = 10000;
		}

		//"looped", data.layers[j].looped)

		//table.Get("BlendMode",desc->blendMode);
		//table.Get("StartWeight",desc->startWeight);
		//table.Get("AdditiveBlend",desc->additiveBlend);		

		//XML.WriteXMLInt(textWriter, "mixType", data.layers[j].mixType);		
		//XML.WriteXMLInt(textWriter, "blendMode", data.layers[j].blendMode);
		//XML.WriteXMLFloat(textWriter, "startWeight", data.layers[j].startWeight);

		if (ReadBool(layerNode,"isTopLayer"))
		{
			Str64* elem = node->topLayers.Add(desc->name.s);			
			elem->Copy(desc->name.s);
		}		


		TiXmlNode* chNode = layerNode->FirstChild("Channel");

		while (chNode)
		{
			std::string ch;
			ch = ReadXML(chNode, "name");

			Str64 s;
			s.Copy(ch.c_str());

			desc->channels.push_back(s);	

			layerNode->RemoveChild(chNode);			
			chNode = layerNode->FirstChild("Channel");		
		}


		manager->Trace(desc->name.s);
		//manager->Trace(desc->clip_name.s);

		for (int k = 0;k<desc->clip_name.GetNum();k++)
		{
			LoadClip(desc->clip_name.GetName(k), desc);
		}

		animNode->RemoveChild(layerNode);
		layerNode = animNode->FirstChild("Layer");		
	}	

	manager->Trace(ReadXML(animNode, "skeletonName"));
	manager->Trace(movementBone.s);

#endif

#ifdef ANIM_PSP

	byte* file = OpenFile(name);
	byte* filePtr = file;	

	if (file)
	{			
		//dword FileSize = GetFileSize(file, NULL);

		//if (FileSize>0)
		{			
			//SetFilePointer(hFile, 0, NULL, 0);

			byte hi = 0;
			byte low = 0;

			ReadFile(file, &hi,  sizeof(hi));
			ReadFile(file, &low, sizeof(low));

			if (hi != 1 && low != 0)
			{
				CloseFile(filePtr);
				return false;
			}		

			int index = strlen(name) - 1;

			while (name[index] != '\\' && name[index] != '/' && index > 0)
			{
				if (name[index - 1] == '\\' || name[index - 1] == '/') break;
				index--;
			}

			char path[512];			
			strncpy(path,name,index);
			path[index] = 0;

			ReadFile(file, &startNode,  sizeof(startNode));
			ReadFile(file, &movementBone,  sizeof(movementBone));
			ReadFile(file, &startTable,  sizeof(startTable));

			int len = 0;
			ReadFile(file, &len,  sizeof(len));

			for (int i=0; i<len; i++)
			{
				Str64 nm;
				ReadFile(file, &nm,  sizeof(nm));

				NamesTable* nm_table = namesTables.Add(nm.s);
				nm_table->name.Copy(nm.s);							

				int len_tmp = 0;
				ReadFile(file, &len_tmp,  sizeof(len_tmp));

				for (int j=0; j<len_tmp; j++)
				{				
					ReadFile(file, &nm,  sizeof(nm));
					Str64* clip = nm_table->clips.Add(nm.s);
					ReadFile(file, clip,  sizeof(*clip));

					LoadClip(clip->s, NULL);
				}				
			}

			ReadFile(file, &len,  sizeof(len));

			for (int i=0; i<len; i++)
			{
				Str64 nm;
				ReadFile(file, &nm, sizeof(nm));

				AnimNode* node = nodes.Add(nm.s);
				node->name.Copy(nm.s);

				ReadFile(file, &node->getMovement,  sizeof(node->getMovement));


				int len_tmp = 0;
				ReadFile(file, &len_tmp,  sizeof(len_tmp));

				for (int j=0; j<len_tmp; j++)
				{
					ReadFile(file, &nm, sizeof(nm));

					NodeConstant* cnst = node->constants.Add(nm.s);
					ReadFile(file, &cnst->value, sizeof(cnst->value));
				}				

				ReadFile(file, &len_tmp,  sizeof(len_tmp));

				if (len_tmp>1)
				{
					node->topLayersPlayMode = 1;
				}

				for (int j=0; j<len_tmp; j++)
				{				
					char str[32];			
					sprintf(str, "Lr%i", j);

					TopLayerElem* elem = node->topLayers.Add(str);
					elem->Init(str);

					ClipDesc* desc = node->layers.Add(str);
					desc->name.Copy(str);

					ReadFile(file, &desc->clip_name,  64);

					char full_path[512];	
					strcpy(full_path, path);
					strcat(full_path, desc->clip_name.s);
					strcat(full_path, ".mda");

					desc->clip_name.Copy(full_path);

					ReadFile(file, &desc->numLoops,  sizeof(desc->numLoops));
					ReadFile(file, &desc->playSpeed,  sizeof(desc->playSpeed));
					ReadFile(file, &desc->playMode,  sizeof(desc->playMode));
					ReadFile(file, &desc->timeIn,  sizeof(desc->timeIn));
					ReadFile(file, &desc->timeOut,  sizeof(desc->timeOut));
					ReadFile(file, &desc->blendMode,  sizeof(desc->blendMode));

					LoadClip(desc->clip_name.s, desc);					

					int len_tmp2 = 0;
					ReadFile(file, &len_tmp2,  sizeof(len_tmp2));

					if (len_tmp2>0)
					{
						desc->events.reserve(len_tmp2);

						for (int j=0; j<len_tmp2; j++)
						{			
							desc->events.push_back(Event());
							Event* evnt = &desc->events[j];

							ReadFile(file, &evnt->id,  sizeof(evnt->id));
							ReadFile(file, &evnt->param,  sizeof(evnt->param));
							ReadFile(file, &evnt->time,  sizeof(evnt->time));
						}
					}	
				}

				bool has_def_link = false;
				ReadFile(file, &has_def_link,  sizeof(has_def_link));

				if (has_def_link)
				{
					ReadFile(file, &node->defLink.destination,  sizeof(node->defLink.destination));
					ReadFile(file, &node->defLink.blendTime,  sizeof(node->defLink.blendTime));
				}

				ReadFile(file, &len_tmp,  sizeof(len_tmp));

				for (int j=0; j<len_tmp; j++)
				{									
					ReadFile(file, &nm, sizeof(nm));
					AnimLink* link = node->links.Add(nm.s);

					ReadFile(file, &link->destination,  sizeof(link->destination));
					ReadFile(file, &link->blendTime,  sizeof(link->blendTime));
					ReadFile(file, &link->activeTime,  sizeof(link->activeTime));
					ReadFile(file, &link->syncTiming,  sizeof(link->syncTiming));
				}
			}

			CloseFile(filePtr);
		}
	}
	else
	{
		return false;

	}			

#endif	

	return true;
}

#if !defined(ANIM_PSP) && !defined(ANIM_STANDALONE)

/*void AnimationRes::ReadNode(CLuaTable& table)
{		
	std::string name = table.GetTableName();

	AnimNode* node = nodes.Add(name.c_str());
	node->name.Copy(name.c_str());

	table.Get("Movement",node->getMovement);
	table.Get("Sequence",node->topLayersPlayMode);	

	if (table.GetTable("TopLayers"))
	{
		int index = 0;
		char str[32];
		sprintf(str, "Lr%i", index);		
		Str64 tmp;

		while (table.Get(str,tmp.s, 64))
		{
			Str64* elem = node->topLayers.Add(tmp.s);
			elem->Copy(tmp.s);

			index++;
			sprintf(str, "Lr%i", index);
		}		

		table.Return();
	}	

	if (table.GetTable("Controllers"))
	{
		if(table.GetFirstTable())
		{
			do
			{
				std::string name = table.GetTableName();
				ControllerParam* lcmp = node->controllerParams.Add(name.c_str());
				lcmp->name.Copy(name.c_str());

				table.Get("Active",lcmp->active);

				if (lcmp->active != -1)
				{
					lcmp->act_set = true;
				}				

				table.Get("Speed",lcmp->speed);

				if (lcmp->speed != -1)
				{
					lcmp->speed_set = true;
				}	

				if (table.GetTable("MinConstraints"))
				{					
					table.Get("x",lcmp->min.x);
					table.Get("y",lcmp->min.y);
					table.Get("z",lcmp->min.z);					

					lcmp->constr_set = true;

					table.Return();
				}								

				if (table.GetTable("MaxConstraints"))
				{					
					table.Get("x",lcmp->max.x);					
					table.Get("y",lcmp->max.y);					
					table.Get("z",lcmp->max.z);					

					lcmp->constr_set = true;

					table.Return();
				}

				if (table.GetTable("PreRotation"))
				{
					table.Get("m11",lcmp->postRotMat._11);
					table.Get("m12",lcmp->postRotMat._12);
					table.Get("m13",lcmp->postRotMat._13);									

					table.Get("m21",lcmp->postRotMat._21);
					table.Get("m22",lcmp->postRotMat._22);
					table.Get("m23",lcmp->postRotMat._23);

					table.Get("m31",lcmp->postRotMat._31);
					table.Get("m32",lcmp->postRotMat._32);
					table.Get("m33",lcmp->postRotMat._33);

					lcmp->rot_set = true;

					table.Return();
				}			

				if (table.GetTable("BendDirection"))
				{
					table.Get("x",lcmp->bendDir.x);								
					table.Get("y",lcmp->bendDir.y);								
					table.Get("z",lcmp->bendDir.z);								

					lcmp->bend_set = true;

					table.Return();
				}				
			}
			while(table.GetNextTable());
		}
		table.Return();
	}

	if (table.GetTable("Constants"))
	{
		if(table.GetFirstTable())
		{
			do
			{				
				NodeConstant* cnst = node->constants.Add(table.GetTableName().c_str());				

				ReadStr64(table, "Value",cnst->value);
			}
			while(table.GetNextTable());
		}

		table.Return();
	}

	if (table.GetTable("Layers"))
	{
		if(table.GetFirstTable())
		{
			do
			{
				ClipDesc* desc = node->layers.Add(table.GetTableName().c_str());				

				desc->name.Copy(table.GetTableName().c_str());

				Str1024 clip_name;
				ReadStr1024(table, "FileName",clip_name);

				Str64 virt_name;
				ReadStr64(table, "Virtual",virt_name);

				desc->clip_name.Init(clip_name.s, virt_name.s);				

				table.Get("NumLoops",desc->numLoops);

				if (desc->numLoops != 1)
				{
					desc->numLoops = 9000;
				}

				table.Get("Speed",desc->playSpeed);				
				table.Get("Mix",desc->mix);				
				table.Get("timeIn",desc->timeIn);
				table.Get("timeOut",desc->timeOut);				
				table.Get("PlayMode",desc->playMode);		
				table.Get("BlendMode",desc->blendMode);
				table.Get("StartWeight",desc->startWeight);
				table.Get("AdditiveBlend",desc->additiveBlend);

				if (table.GetTable("Events"))
				{
					if(table.GetFirstTable())
					{
						do
						{
							desc->events.push_back(Event());
							Event* evnt = &desc->events[desc->events.size()-1];

							evnt->id.Copy(table.GetTableName().c_str());

							ReadStr64(table, "ID",evnt->id);
							ReadStr64(table, "Param",evnt->param);
							table.Get("Time",evnt->time);

							int k = 0;
							table.Get("Unskeepable",k);	

							if (k>0) evnt->unskeepable = true;	
						}
						while(table.GetNextTable());
					}

					table.Return();


					for (int i = 0; i<(int)desc->events.size()-1; i++)
					{
						for (int j = i+1; j<(int)desc->events.size(); j++)
						{
							if (desc->events[i].time > desc->events[j].time)
							{
								Event tmp = desc->events[i];
								desc->events[i] = desc->events[j];
								desc->events[j] = tmp;
							}
						}
					}
				}

				//Legacy
				if (table.GetTable("Channels"))
				{
					for (dword j = 0; j < table.GetNumElems(); j++)
					{
						Str64 tmp;

						if (table.Get(j, tmp.s, 64))
						{									
							desc->channels.push_back(tmp);							
						}
					}

					if (desc->channels.size()==0)
					{			
						int index = 0;
						char str[32];
						sprintf(str, "Ch%i", index);		
						Str64 tmp;

						while (table.Get(str,tmp.s, 64))
						{							
							desc->channels.push_back(tmp);

							index++;
							sprintf(str, "Ch%i", index);
						}
					}

					table.Return();
				}				

				for (int k = 0;k<desc->clip_name.GetNum();k++)
				{
					if (!desc->clip_name.IsVirtual(k))
					{
						const char* clipName = desc->clip_name.GetName(k);
						if ( !clipName )
							continue;
						if ( !clipName[0] )
						{
							manager->Trace("%s: animgraph: \"%s\" node: \"%s\" - empty clip name\n", __FUNCTION__, graph_name.s, name.c_str());
							continue;
						}
						LoadClip(clipName, desc);
					}
				}
			}
			while(table.GetNextTable());
		}

		table.Return();
	}

	if (table.GetTable("DefLink"))
	{									
		ReadStr64(table, "LinkedTo", node->defLink.destination);
		table.Get("BlendTime", node->defLink.blendTime);		
		table.Get("GlobalBlend", node->defLink.blendInGlobal);

		table.Return();
	}								

	ReadStr64(table, "DefLink",node->defLink.destination);

	if (table.GetTable("Links"))
	{
		if(table.GetFirstTable())
		{
			do
			{	
				AnimLink* link = node->links.Add(table.GetTableName().c_str());

				ReadStr64(table, "LinkedTo", link->destination);

				table.Get("BlendTime", link->blendTime);
				table.Get("ActTime", link->activeTime);
				table.Get("SyncTiming", link->syncTiming);	
				table.Get("GlobalBlend", link->blendInGlobal);	
			}
			while(table.GetNextTable());
		}

		table.Return();
	}
}*/

#endif

AnimationInstance* AnimationRes::CreateReference()
{
	AddRef();
	AnimationInstance* anim = new AnimationInstance(this);
	anim->Init();

	return anim;
}

void AnimationRes::Release()
{	
	controllers.Clear();

	for (int i=0;i<nodes.Size();i++)
	{
		AnimNode* node = nodes.Get(i);

		node->topLayers.Clear();
		node->controllerParams.Clear();
		node->constants.Clear();
		node->layers.Clear();		
		node->links.Clear();
	}

	nodes.Clear();

	for (int i=0;i<namesTables.Size();i++)
	{
		NamesTable* table = namesTables.Get(i);

		table->clips.Clear();
	}

	namesTables.Clear();	

	affectors.Clear();
}
