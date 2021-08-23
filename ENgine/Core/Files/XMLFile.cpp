#include "Core/Core.h"
#include "XMLFile.h"

#ifdef IOS
#include "sys/xattr.h"
#endif

bool XMLFile::Init(const char* name, const char* enterBlock,bool bMemory)
{	
#ifdef EDITOR
	need_log_warnings = true;
#endif
    doc = null;
    doc = new tinyxml2::XMLDocument();
    if(bMemory)
    {
        if (doc->Parse(name,strlen(name)))
        {
            delete doc;
            doc = null;
            core->TraceTo("Files", "Can't load \"%s\"", name);
            return false;
        }
    }
    else
    {
        IFileBuffer* fb = core->Files()->LoadFile(name);
        if (!fb)
        {
            delete doc;
            doc = null;
            return false;	
        }
        if (doc->Parse((char*)fb->GetBuffer(),fb->GetSize()))
        {
            delete doc;
            doc = null;
            fb->Release();
            core->TraceTo("Files", "Can't load \"%s\"", name);
            return false;
        }
        fb->Release();
    }
	cur_depth = 0;
	nodes[cur_depth] = doc->FirstChildElement(enterBlock);

	cur_node = nodes[cur_depth];

	if (!cur_node)
	{
#ifdef EDITOR
		core->TraceTo("Files", "Can't find enterBlock - %s in scene %s", enterBlock, name);
#endif
		return false;
	}

	String::Copy(fl_name, 256, name);

	return true;
}

bool XMLFile::EnterBlock(const char* name)
{		
	nodes[cur_depth+1] = nodes[cur_depth]->FirstChildElement(name);

	if (nodes[cur_depth+1])
	{
		cur_depth++;
		cur_node = nodes[cur_depth];

		return true;
	}

	return false;
}

void XMLFile::LeaveBlock()
{
	if (cur_depth == 0) return;

	cur_depth--;
	nodes[cur_depth]->DeleteChild(nodes[cur_depth+1]);
	cur_node = nodes[cur_depth];
}

bool XMLFile::ReadString(const char* name, char* val, int val_len)
{	
	if (!XMLReadString(cur_node, name, val, val_len))
	{
#ifdef EDITOR
		if (need_log_warnings) core->TraceTo("Files", "Warning: in %s absent field %s", fl_name, name);
#endif
		return false;
	}

	return true;
}

bool XMLFile::ReadBool(const char* name, bool& val)
{
	if (!XMLReadBool(cur_node, name, val))
	{
#ifdef EDITOR
		if (need_log_warnings) core->TraceTo("Files", "Warning: in %s absent field %s", fl_name, name);
#endif
		return false;
	}

	return true;
}

bool XMLFile::ReadFloat(const char* name, float& val)
{
	if (!XMLReadFloat(cur_node, name, val))
	{
#ifdef EDITOR
		if (need_log_warnings) core->TraceTo("Files", "Warning: in %s absent field %s", fl_name, name);
#endif
		return false;
	}
	
	return true;
}

bool XMLFile::ReadInt(const char* name, int& val)
{
	if (!XMLReadInt(cur_node, name, val))
	{
#ifdef EDITOR
		if (need_log_warnings) core->TraceTo("Files", "Warning: in %s absent field %s", fl_name, name);
#endif
		return false;
	}

	return true;
}

bool XMLFile::ReadInt64(const char* name, int64& val)
{
	if (!XMLReadInt64(cur_node, name, val))
	{
#ifdef EDITOR
		if (need_log_warnings) core->TraceTo("Files", "Warning: in %s absent field %s", fl_name, name);
#endif
		return false;
	}

	return true;
}


bool XMLFile::ReadDword(const char* name, dword& val)
{
	if (!XMLReadDword(cur_node, name, val))
	{
#ifdef EDITOR
		if (need_log_warnings) core->TraceTo("Files", "Warning: in %s absent field %s", fl_name, name);
#endif
		return false;
	}

	return true;
}

bool XMLFile::ReadMatrix(const char* name, Matrix& mat)
{
	if (!XMLReadMatrix(cur_node, name, mat))
	{
#ifdef EDITOR
		if (need_log_warnings) core->TraceTo("Files", "Warning: in %s absent field %s", fl_name, name);
#endif
		return false;
	}

	return true;
}

bool XMLFile::ReadVector(const char* name, Vector& val)
{
	if (!XMLReadVector(cur_node, name, val))
	{
#ifdef EDITOR
		if (need_log_warnings) core->TraceTo("Files", "Warning: in %s absent field %s", fl_name, name);
#endif
		return false;
	}

	return true;
}

int XMLFile::GetStringSize(const char* name)
{
	return XMLGetStringSize(cur_node, name);
}

void XMLFile::Release()
{
	if (doc) delete doc;
	delete this;
}