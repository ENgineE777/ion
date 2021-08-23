
#include "Core/Core.h"
#include "AndroidXML.h"

void AndroidXML::TraceStringPool(byte* &ptr)
{
	ResStringPool_header* rsp_header = (ResStringPool_header*)ptr;			

	bool utf8 = false;

	if (rsp_header->flags & ResStringPool_header::UTF8_FLAG)
	{
		utf8 = true;
		core->TraceTo("app", " pool in utf-8");				
	}

	byte* ptr_indices = ptr + sizeof(ResStringPool_header);
	byte* ptr_strings = ptr + rsp_header->stringsStart;

	for (size_t i=0;i<rsp_header->stringCount;i++)
	{					
		int index = *((int*)&ptr_indices[i*4]);

		if (utf8)
		{
			int len = ptr_strings[index + 1];
			core->TraceTo("app", " len %i: %s", len, (const char*)&ptr_strings[index + 2]);				
		}
		else
		{			
			word len = *((word*)&ptr_strings[index]);
			wstring wstr = (wchar_t*)&ptr_strings[index + 2];
			string dest;
			String::Utf16toUtf8(dest, wstr.c_str());

			core->TraceTo("app", " len %i: %s", len, dest.c_str());				
		}
	}

	ptr += rsp_header->header.size;
}

void AndroidXML::TraceXML(const char* fl_name)
{
	IFileBuffer* fb = core->Files()->LoadFile(fl_name);

	byte* ptr = fb->GetBuffer();	

	ResChunk_header* chunk = (ResChunk_header*)ptr;

	if (chunk->type == RES_XML_TYPE)
	{			
		core->TraceTo("app", "RES_XML_TYPE");

		ptr += sizeof(ResChunk_header);

		TraceStringPool(ptr);
	}

	if (chunk->type == RES_TABLE_TYPE)
	{
		core->TraceTo("app", "RES_TABLE_TYPE");

		ResTable_header* header = (ResTable_header*)(ptr);
		ptr += sizeof(ResTable_header);

		TraceStringPool(ptr);

		for (size_t k=0; k<header->packageCount;k++)
		{
			ResTable_package* rt_header = (ResTable_package*)ptr;			

			byte* chunk_ptr = ptr + sizeof(ResTable_package);

			TraceStringPool(chunk_ptr);

			TraceStringPool(chunk_ptr);

			ptr += rt_header->header.size;
		}
	}

	fb->Release();
}

AndroidXML::AndroidXML()
{
	fb = null;
	fb_ptr = null;
	utf8 = false;
	indices = 0;
	len_strings = 0;
	strings = 0;
	res_header = null;
	restable_header = null;
	align_byte = 0;
	attr_ext = null;
}

void AndroidXML::Init(byte* ptr)
{
	ResStringPool_header* rsp_header = (ResStringPool_header*)ptr;		
	header = *rsp_header;

	utf8 = header.flags & ResStringPool_header::UTF8_FLAG;

	indices = (byte*)malloc(header.stringCount * 4);
	memcpy(indices, ptr + sizeof(ResStringPool_header), header.stringCount * 4);

	len_strings = header.header.size - header.stringsStart;

	strings = (byte*)malloc(len_strings);
	memcpy(strings, ptr + header.stringsStart, len_strings);
}

void AndroidXML::ModifyPackageName(const char* packagename, const char* new_packagename)
{
	byte* ptr = fb_ptr;

	if (res_header->type == RES_TABLE_TYPE)
	{
		wstring wstr;			
		String::Utf8toUtf16(wstr, packagename);

		for (int k=0; k<restable_header->packageCount;k++)
		{
			ResTable_package* rt_header = (ResTable_package*)ptr;			

			wstring wstr2;

			word* str_ptr = (word*)&rt_header->name[0];

			while ((*str_ptr) != 0)
			{
				wchar_t w[2];
				w[0] = *str_ptr;
				w[1] = 0;

				wstr2.append(w);
				str_ptr++;
			}

			if (wcscmp(wstr2.c_str(), wstr.c_str()) == 0)
			{
				String::Utf8toUtf16(wstr, new_packagename);	

				int len = wcslen(wstr.c_str());

				int index = 0;

				for (int index=0; index<len; index++)
				{
					word w = wstr.c_str()[index];

					rt_header->name[index] = w;
				}

				rt_header->name[index] = 0;					
			}

			byte* chunk_ptr = ptr + sizeof(ResTable_package);				

			ptr += rt_header->header.size;
		}
	}
}

void AndroidXML::ModifyVersionCode(int version_code)
{
	wstring wstr;
	string dest;

	wstring wstr2;

	if (res_header->type == RES_XML_TYPE)
	{		
		//ResXMLTree_attribute* attr = (ResXMLTree_attribute*)ptr;

		for (int i=0; i<attr_ext->attributeCount; i++)
		{												
			int index = *((int*)&indices[attr->name.index * 4]);							

			if (!utf8)
			{
				word* str_ptr = (word*)&strings[index + 2];

				while ((*str_ptr) != 0)
				{
					wchar_t w[2];
					w[0] = *str_ptr;
					w[1] = 0;

					wstr2.append(w);
					str_ptr++;
				}								

				String::Utf16toUtf8(dest, wstr2.c_str());					
			}
			else
			{
				dest = (char*)&strings[index + 2];					
			}				

			core->TraceTo("app", dest.c_str());

			if (String::IsEqual(dest.c_str(), "versionCode"))
			{
				attr->typedValue.data = version_code;
				break;
			}

			attr++;
		}
	}
}

void AndroidXML::Modify(const char* find, const char* replace)
{
	wstring wstr;
	wstring wstr2;

	if (!utf8)
	{
		String::Utf8toUtf16(wstr, find);
	}

	int shift_index = -1;

	for (int i=0;i<header.stringCount;i++)
	{					
		int index = *((int*)&indices[i*4]);			

		if (!utf8)
		{			
			wstr2.clear();

			word* str_ptr = (word*)&strings[index + 2];

			while ((*str_ptr) != 0)
			{
				wchar_t w[2];
				w[0] = *str_ptr;
				w[1] = 0;

				wstr2.append(w);
				str_ptr++;
			}

		}

		if ((utf8 && String::IsEqual(find, (char*)&strings[index + 2]) ) ||
			(!utf8 && wcscmp(wstr.c_str(), wstr2.c_str()) == 0) )
		{				
			shift_index = i;
			//core->TraceTo("app2", "finded %i %s", shift_index, find);
			break;
		}			
	}		

	if (shift_index != -1)
	{	
		int shift = *((int*)&indices[shift_index*4]);

		int len1 = strlen(find);						
		int len2 = strlen(replace);			

		int delta = len2 - len1;

		if (delta <= 0)
		{
			byte* tmp = &strings[shift];

			if (!utf8)
			{					
				wstring wstr;
				String::Utf8toUtf16(wstr, replace);

				*((word*)&tmp[0]) = len2;
				tmp += 2;

				for (int i=0;i<len2;i++)
				{
					word w = wstr.c_str()[i];
					*((word*)&tmp[i*2]) = w;										
				}

				tmp += len2 * 2;

				tmp[0] = 0;
				tmp[1] = 0;						
			}
			else
			{
				tmp[0] = CalcUtf8Len(replace);
				tmp[1] = len2;

				tmp += 2;

				memcpy(tmp, replace, len2);

				tmp += len2;								

				tmp[0] = 0;								
			}

			return;
		}

		if (!utf8)
		{
			delta *= 2;				
		}

		res_header->size += delta;
		header.header.size += delta;

		byte* tmp_strings = (byte*)malloc(len_strings + delta);																																						

		memcpy(tmp_strings, strings, shift);

		byte* tmp = &tmp_strings[shift];

		if (utf8)
		{				
			tmp[0] = CalcUtf8Len(replace);
			tmp[1] = len2;

			tmp += 2;

			memcpy(tmp, replace, len2);

			tmp += len2;								

			tmp[0] = 0;			
			tmp ++;
		}
		else
		{				
			wstring wstr;
			String::Utf8toUtf16(wstr, replace);

			*((word*)&tmp[0]) = len2;
			tmp += 2;

			for (int i=0;i<len2;i++)
			{
				word w = wstr.c_str()[i];
				*((word*)&tmp[i*2]) = w;										
			}

			tmp += len2 * 2;

			tmp[0] = 0;
			tmp[1] = 0;
			tmp += 2;
		}			

		shift = *((int*)&indices[(shift_index + 1)*4]);

		int def = (shift + delta) - (tmp - tmp_strings);


		memcpy(tmp, strings + shift, len_strings - shift);

		len_strings += delta;

		for (int i = shift_index + 1; i<header.stringCount; i++)
		{
			*((int*)&indices[i*4]) += delta;
		}

		free(strings);
		strings = tmp_strings;
	}
}

void AndroidXML::Write(IFile* file)
{
	file->Write(&header, sizeof(ResStringPool_header));
	file->Write(indices, header.stringCount * 4);			
	file->Write(strings, len_strings);		

	byte zero[4];
	zero[0] = zero[1] = zero[2] = zero[3] = 0;

	if (align_byte > 0)
	{
		file->Write(zero, align_byte);	
	}
}

void AndroidXML::Open(const char* file_name)
{
	fb = core->Files()->LoadFile(file_name);

	fb_ptr = fb->GetBuffer();	

	res_header = (ResChunk_header*)fb_ptr;	

	if (res_header->type == RES_TABLE_TYPE)
	{
		restable_header = (ResTable_header*)(fb_ptr);
		fb_ptr += sizeof(ResTable_header);

		Init(fb_ptr);	

		ResChunk_header* sub_chunk = (ResChunk_header*)fb_ptr;
		fb_ptr += sub_chunk->size;
	}
	else
		if (res_header->type == RES_XML_TYPE)
		{
			fb_ptr += sizeof(ResChunk_header);

			Init(fb_ptr);			

			ResChunk_header* sub_chunk = (ResChunk_header*)fb_ptr;
			fb_ptr += sub_chunk->size;


			byte* ptr = fb_ptr;

			sub_chunk = (ResChunk_header*)ptr;
			ptr += sub_chunk->size;

			sub_chunk = (ResChunk_header*)ptr;
			ptr += sub_chunk->size;


			sub_chunk = (ResChunk_header*)ptr;

			attr_ext = (ResXMLTree_attrExt*)(ptr + sizeof(ResXMLTree_node));			

			attr = (ResXMLTree_attribute*)(ptr + sizeof(ResXMLTree_node) + sizeof(ResXMLTree_attrExt));
		}		
}

void AndroidXML::Write(const char* file_name)
{						
	if (res_header->type == RES_TABLE_TYPE || res_header->type == RES_XML_TYPE)
	{			
		align_byte = ((int)((float)(len_strings)/4.0f) + 1)* 4 - len_strings;
		if (align_byte == 4) align_byte = 0;

		res_header->size += align_byte;
		header.header.size += align_byte;

		IFile* file = core->Files()->OpenFile(file_name, IFile::write);

		if (res_header->type == RES_TABLE_TYPE)
		{
			file->Write(restable_header, sizeof(ResTable_header));
		}
		else
		{
			file->Write(res_header, sizeof(ResChunk_header));
		}

		Write(file);

		file->Write(fb_ptr, (res_header->size - (fb_ptr - fb->GetBuffer())));		

		file->Release();	
	}

	fb->Release();	

	free(indices);		
	free(strings);			
}