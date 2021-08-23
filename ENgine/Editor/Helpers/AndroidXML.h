
#pragma once

#include "Common/Common.h"

class IFileBuffer;

class AndroidXML
{
public:

	struct ResChunk_header
	{
		// Type identifier for this chunk.  The meaning of this value depends
		// on the containing chunk.
		word type;

		// Size of the chunk header (in bytes).  Adding this value to
		// the address of the chunk allows you to find its associated data
		// (if any).
		word headerSize;

		// Total size of this chunk (in bytes).  This is the chunkSize plus
		// the size of any data associated with the chunk.  Adding this value
		// to the chunk allows you to completely skip its contents (including
		// any child chunks).  If this value is the same as chunkSize, there is
		// no data associated with the chunk.
		dword size;
	};

	enum
	{
		RES_NULL_TYPE               = 0x0000,
		RES_STRING_POOL_TYPE        = 0x0001,
		RES_TABLE_TYPE              = 0x0002,
		RES_XML_TYPE                = 0x0003,

		// Chunk types in RES_XML_TYPE
		RES_XML_FIRST_CHUNK_TYPE    = 0x0100,
		RES_XML_START_NAMESPACE_TYPE= 0x0100,
		RES_XML_END_NAMESPACE_TYPE  = 0x0101,
		RES_XML_START_ELEMENT_TYPE  = 0x0102,
		RES_XML_END_ELEMENT_TYPE    = 0x0103,
		RES_XML_CDATA_TYPE          = 0x0104,
		RES_XML_LAST_CHUNK_TYPE     = 0x017f,
		// This contains a uint32_t array mapping strings in the string
		// pool back to resource identifiers.  It is optional.
		RES_XML_RESOURCE_MAP_TYPE   = 0x0180,

		// Chunk types in RES_TABLE_TYPE
		RES_TABLE_PACKAGE_TYPE      = 0x0200,
		RES_TABLE_TYPE_TYPE         = 0x0201,
		RES_TABLE_TYPE_SPEC_TYPE    = 0x0202
	};

	struct ResTable_header
	{
		struct ResChunk_header header;

		// The number of ResTable_package structures.
		dword packageCount;
	};

	struct ResStringPool_header
	{    
		ResChunk_header header;

		// Number of strings in this pool (number of uint32_t indices that follow
		// in the data).
		dword stringCount;

		// Number of style span arrays in the pool (number of uint32_t indices
		// follow the string indices).
		dword styleCount;

		// Flags.
		enum
		{
			// If set, the string index is sorted by the string values (based
			// on strcmp16()).
			SORTED_FLAG = 1<<0,

			// String pool is encoded in UTF-8
			UTF8_FLAG = 1<<8
		};

		dword flags;

		// Index from header of the string data.
		dword stringsStart;

		// Index from header of the style data.
		dword stylesStart;
	};

	struct ResTable_package
	{
		struct ResChunk_header header;

		// If this is a base package, its ID.  Package IDs start
		// at 1 (corresponding to the value of the package bits in a
		// resource identifier).  0 means this is not a base package.
		dword id;

		// Actual name of this package, \0-terminated.
		word name[128];

		// Offset to a ResStringPool_header defining the resource
		// type symbol table.  If zero, this package is inheriting from
		// another base package (overriding specific values in it).
		dword typeStrings;

		// Last index into typeStrings that is for public use by others.
		dword lastPublicType;

		// Offset to a ResStringPool_header defining the resource
		// key symbol table.  If zero, this package is inheriting from
		// another base package (overriding specific values in it).
		dword keyStrings;

		// Last index into keyStrings that is for public use by others.
		dword lastPublicKey;
	};

	struct ResStringPool_ref
	{
		// Index into the string pool table (uint32_t-offset from the indices
		// immediately after ResStringPool_header) at which to find the location
		// of the string data in the pool.
		dword index;
	};

	struct ResXMLTree_node
	{
		struct ResChunk_header header;

		// Line number in original source file at which this element appeared.
		dword lineNumber;

		// Optional XML comment that was associated with this element; -1 if none.
		struct ResStringPool_ref comment;
	};

	struct ResXMLTree_attrExt
	{
		// String of the full namespace of this element.
		struct ResStringPool_ref ns;

		// String name of this node if it is an ELEMENT; the raw
		// character data if this is a CDATA node.
		struct ResStringPool_ref name;

		// Byte offset from the start of this structure where the attributes start.
		word attributeStart;

		// Size of the ResXMLTree_attribute structures that follow.
		word attributeSize;

		// Number of attributes associated with an ELEMENT.  These are
		// available as an array of ResXMLTree_attribute structures
		// immediately following this node.
		word attributeCount;

		// Index (1-based) of the "id" attribute. 0 if none.
		word idIndex;

		// Index (1-based) of the "class" attribute. 0 if none.
		word classIndex;

		// Index (1-based) of the "style" attribute. 0 if none.
		word styleIndex;
	};

	struct Res_value
	{
		// Number of bytes in this structure.
		word size;

		// Always set to 0.
		byte res0;

		// Type of the data value.

		// ... elided

		byte dataType;

		// ... elided

		// The data for this item, as interpreted according to dataType.
		dword data;

		// ... elided
	};

	struct ResXMLTree_attribute
	{
		// Namespace of this attribute.
		struct ResStringPool_ref ns;

		// Name of this attribute.
		struct ResStringPool_ref name;

		// The original raw string value of this attribute.
		struct ResStringPool_ref rawValue;

		// Processesd typed value of this attribute.
		struct Res_value typedValue;
	};

	static void TraceStringPool(byte* &ptr);
	static void TraceXML(const char* fl_name);
	
	IFileBuffer* fb;
	byte*		 fb_ptr;

	ResChunk_header*     res_header;
	ResStringPool_header header;
	bool utf8;

	ResXMLTree_attrExt* attr_ext;
	ResXMLTree_attribute* attr;

	ResTable_header* restable_header;
	byte* indices;

	int   len_strings;
	byte* strings;

	int align_byte;

	AndroidXML();

	void Init(byte* ptr);	
	void ModifyPackageName(const char* packagename, const char* new_packagename);	
	void ModifyVersionCode(int version_code);	
	void Modify(const char* find, const char* replace);
	void Write(IFile* file);	
	void Open(const char* file_name);	
	void Write(const char* file_name);	
};
