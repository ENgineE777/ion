#include "Core/Core.h"
#include "JSONFile.h"

JSONFile::JSONFile() : allocator(1 << 10),data(NULL),fb(NULL)
{
#ifdef EDITOR
	need_log_warnings = false;
#endif
}

bool JSONFile::Init(const char* name,bool bMemory)
{	
    char *errorPos = 0;
    char *errorDesc = 0;
    int errorLine = 0;	
    if(data)
        free(data);
    data = NULL;fb = NULL;
    if(bMemory)
    { 
      int len  = strlen(name);
      data = (char*)malloc(len + 1);
      memset(data,0,len + 1);
      memcpy(data,name,len);
      root = json_parse(data, &errorPos, &errorDesc, &errorLine, &allocator);
    }
    else
    {
	    fb = core->Files()->LoadFile(name);
	    if (!fb) return false;
	    root = json_parse((char*)fb->GetBuffer(), &errorPos, &errorDesc, &errorLine, &allocator);
    }
	if (!root)
	{
		core->TraceTo("Files", "JSON Error %s at %s, %i", errorDesc, errorPos, errorLine);
		return false;
	}

	cur_depth = 0;
	nodes[cur_depth] = root;	

	cur_node = nodes[cur_depth];

	String::Copy(fl_name, 256, name);

	return true;
}

bool JSONFile::EnterBlock(const char* name)
{			
	nodes[cur_depth+1] = FindValue(name);

	if (nodes[cur_depth+1])
	{
		if (nodes[cur_depth+1]->type == JSON_OBJECT)
		{
			cur_depth++;
			cur_node = nodes[cur_depth];

			return true;
		}
		else
		if (nodes[cur_depth+1]->type == JSON_ARRAY)
		{
			if (nodes[cur_depth+1]->first_child)
			{			
				cur_depth++;			
				nodes[cur_depth] = nodes[cur_depth]->first_child;
				cur_node = nodes[cur_depth];

				return true;
			}
		}
	}

	return false;
}

void JSONFile::LeaveBlock()
{
	if (cur_depth == 0) return;

	if (nodes[cur_depth]->parent->type == JSON_ARRAY)
	{
		nodes[cur_depth]->parent->first_child = nodes[cur_depth]->next_sibling;
	}	
	
	cur_depth--;	
	cur_node = nodes[cur_depth];
}

bool JSONFile::ReadString(const char* name, char* val, int val_len)
{	
	json_value* node = FindValue(name);
	
	if (node)
	{
		if (node->type == JSON_STRING)
		{		
			String::Copy(val, val_len, node->string_value);
			return true;
		}
		else
		if (node->type == JSON_BOOL || node->type == JSON_INT)
		{
			String::Printf(val, 63, "%i", node->int_value);
			return true;						
		}
		else
		if (node->type == JSON_FLOAT)
		{
			String::Printf(val, 63, "%4.4f", node->float_value);
			return true;			
		}		
		else
		if (node->type == JSON_NULL)
		{
			String::Printf(val, 63, "");
			return true;
		}
	}
	else
	{
#ifdef EDITOR
		if (need_log_warnings) core->TraceTo("Files", "Warning: in %s absent field %s", fl_name, name);
#endif
	}

	return false;	
}

bool JSONFile::ReadBool(const char* name, bool& val)
{
	json_value* node = FindValue(name);

	if (node)
	{
		if (node->type == JSON_BOOL)
		{
			if (node->int_value > 0)
			{
				val = true;
			}						
			else
			{
				val = false;
			}

			return true;
		}
		else
		if (node->type == JSON_STRING)
		{
			if (atoi(node->string_value) > 0)
			{
				val = true;
			}						
			else
			{
				val = false;
			}

			return true;
		}
		else
		if (node->type == JSON_NULL)
		{
			val = false;

			return true;
		}
	}
	else
	{
#ifdef EDITOR
		if (need_log_warnings) core->TraceTo("Files", "Warning: in %s absent field %s", fl_name, name);
#endif
	}

	return false;
}

bool JSONFile::ReadFloat(const char* name, float& val)
{
	json_value* node = FindValue(name);

	if (node)
	{
		if (node->type == JSON_FLOAT)
		{
			val = node->float_value;
			return true;
		}		
		else
		if (node->type == JSON_INT)
		{
			val = node->int_value;
			return true;
		}
		else
		if (node->type == JSON_STRING)
		{
			val = atof(node->string_value);
			return true;
		}
	}
	else
	{
#ifdef EDITOR
		if (need_log_warnings) core->TraceTo("Files", "èíòðåWarning: in %s absent field %s", fl_name, name);
#endif
	}

	return false;
}

bool JSONFile::ReadInt(const char* name, int& val)
{    
	json_value* node = FindValue(name);

	if (node)
	{
		if (node->type == JSON_INT)
		{
			val = node->int_value;

			return true;
		}
		else
		if (node->type == JSON_NULL)
		{
			val = 0;

			return true;
		}
		else
		if (node->type == JSON_STRING)
		{
			val = atoi(node->string_value);
			return true;
		}
	}
	else
	{
#ifdef EDITOR
		if (need_log_warnings) core->TraceTo("Files", "Warning: in %s absent field %s", fl_name, name);
#endif
	}

	return false;
}

bool JSONFile::ReadInt64(const char* name, int64& val) 
{
	json_value* node = FindValue(name);

	if (node)
	{
		if (node->type == JSON_INT)
		{
			val = node->int64_value;
			return true;
		}
		else
		if (node->type == JSON_NULL)
		{
			val = 0;

			return true;
		}
		else
		if (node->type == JSON_STRING)
		{
			val = atol(node->string_value);
			return true;
		}
	}
	else
	{
#ifdef EDITOR
		if (need_log_warnings) core->TraceTo("Files", "Warning: in %s absent field %s", fl_name, name);
#endif
	}

	return false;
}

bool JSONFile::ReadDword(const char* name, dword& val)
{
	json_value* node = FindValue(name);

	if (node)
	{
		if (node->type == JSON_INT)
		{
			val = node->int_value;
			return true;
		}
		else
		if (node->type == JSON_NULL)
		{
			val = 0;

			return true;
		}
	}
	else
	{
#ifdef EDITOR
		if (need_log_warnings) core->TraceTo("Files", "Warning: in %s absent field %s", fl_name, name);
#endif
	}

	return false;
}

bool JSONFile::ReadMatrix(const char* name, Matrix& mat)
{
	json_value* node = FindValue(name);

	if (node)
	{
		if (node->type == JSON_OBJECT)
		{
			bool res = true;

			if (EnterBlock(name))
			{
				res &= ReadFloat("m11", mat._11);
				res &= ReadFloat("m12", mat._12);
				res &= ReadFloat("m13", mat._13);

				res &= ReadFloat("m21", mat._21);
				res &= ReadFloat("m22", mat._22);
				res &= ReadFloat("m23", mat._23);

				res &= ReadFloat("m31", mat._31);
				res &= ReadFloat("m32", mat._32);
				res &= ReadFloat("m33", mat._33);
		
				res &= ReadFloat("m41", mat._41);
				res &= ReadFloat("m42", mat._42);
				res &= ReadFloat("m43", mat._43);

				LeaveBlock();		

				return res;
			}
		}
	}
	else
	{
#ifdef EDITOR
		if (need_log_warnings) core->TraceTo("Files", "Warning: in %s absent field %s", fl_name, name);
#endif
	}

	return false;
}

bool JSONFile::ReadVector(const char* name, Vector& val)
{
	json_value* node = FindValue(name);

	if (node)
	{
		if (node->type == JSON_OBJECT)
		{
			bool res = true;

			if (EnterBlock(name))
			{
				res &= ReadFloat("x", val.x);
				res &= ReadFloat("y", val.y);
				res &= ReadFloat("z", val.z);			
				
				LeaveBlock();	

				return res;
			}			
		}
	}
	else
	{
#ifdef EDITOR
		if (need_log_warnings) core->TraceTo("Files", "Warning: in %s absent field %s", fl_name, name);
#endif
	}

	return false;
}

int JSONFile::GetStringSize(const char* name)
{
	json_value* node = FindValue(name);

	if (node)
	{
		if (node->type == JSON_STRING)
		{
			return strlen(node->string_value);
		}
		else
		{
			return 128;
		}
	}

	return 0;
}

void JSONFile::Release()
{
    if(data)
        free(data);
    if(fb)
        fb->Release();
    fb = NULL;
    data = NULL;
	allocator.free();
	delete this;
}

json_value* JSONFile::FindValue(const char* name)
{	
	if (!name[0])
	{
		return cur_node;
	}

	for (json_value *it = cur_node->first_child; it; it = it->next_sibling)
	{
		if (String::IsEqual(name, it->name))
		{
			return it;
		}		
	}

	return null;
}
