
#include "Core/Core.h"
#include "Packer.h"

ThreadLocker rec_locker;
vector<FileService::FileRecord> records;

//rec_locker.Init();

void RegisterExportedFile(const char* in, const char* out, const char* ext, Platform platform)
{	
	IFile* file = core->Files()->OpenFile(out, IFile::read);

	if (file)
	{						
		rec_locker.Lock();

		records.push_back(FileService::FileRecord());		

		FileService::FileRecord* rec = &records[records.size() - 1];

		String::Printf(rec->filename, 256, "%s", in);	

		String::LowerCase(rec->filename);

		rec->hash = HashValue(rec->filename);

		rec->size = file->GetSize();

		if (records.size()>1)
		{
			rec->offset = records[records.size() - 2].offset + records[records.size() - 2].size;

			float sz = (float)(records[records.size() - 2].size)/4.0f;

			int need = 4 - (records[records.size() - 2].size - (int)sz * 4);

			if (need < 4)
			{								
				rec->offset += need;
			}					
		}
		else
		{
			rec->offset = 0;
		}

		rec_locker.UnLock();

		file->Release();					
	}	
}

void AddFile(FILE* storage, const char* input_file)
{
	FILE* fl = fopen(input_file, "rb");

	fseek(fl, 0, SEEK_END);
	int sz = ftell(fl);
	fwrite(&sz, 4, 1, storage);

	byte* buffer = (byte*)malloc(sz);

	fseek(fl, 0, SEEK_SET);
	fread(buffer, sz, 1, fl);

	fclose(fl);	

	fwrite(buffer, sz, 1, storage);			

	free(buffer);

	DeleteFile(input_file);
}

void ScanDirectory(const char* dir, const char* local_dir, const char* pack_local_dir, vector<FileService::FileRecord>& records)
{
	char cur_dir[521];
	String::Printf(cur_dir, 512, "%s/*.*", dir);

	WIN32_FIND_DATA data;
	HANDLE h = FindFirstFile(cur_dir, &data);
	
	if( h!=INVALID_HANDLE_VALUE ) 
	{
		do
		{
			if (!String::IsEqual(data.cFileName, ".") && !String::IsEqual(data.cFileName, ".."))
			{
				if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					char sub_dir[521];
					String::Printf(sub_dir, 512, "%s/%s", dir, data.cFileName);

					char sub_local_dir[521];
					String::Printf(sub_local_dir, 512, "%s/%s", local_dir, data.cFileName);
					
					char sub_pack_local_dir[521];

					if (pack_local_dir[0])
					{
						String::Printf(sub_pack_local_dir, 512, "%s/%s", pack_local_dir, data.cFileName);
					}
					else
					{
						String::Printf(sub_pack_local_dir, 512, "%s", data.cFileName);
					}				

					ScanDirectory(sub_dir, sub_local_dir, sub_pack_local_dir, records);
				}
				else
				{
					records.push_back(FileService::FileRecord());

					FileService::FileRecord* rec = &records[records.size() - 1];

					if (pack_local_dir[0])
					{
						String::Printf(rec->filename, 256, "%s/%s", pack_local_dir, data.cFileName);									
					}
					else
					{
						String::Printf(rec->filename, 256, "%s", data.cFileName);									
					}
				
					String::LowerCase(rec->filename);

					rec->hash = HashValue(rec->filename);

					char file_name[256];
					String::Printf(file_name, 256, "%s/%s", local_dir, data.cFileName);

					IFile* file = core->Files()->OpenFile(file_name, IFile::read);

					if (file)
					{						
						rec->size = file->GetSize();

						if (records.size()>1)
						{
							rec->offset = records[records.size() - 2].offset + records[records.size() - 2].size;

							float sz = (float)(records[records.size() - 2].size)/4.0f;

							int need = 4 - (records[records.size() - 2].size - (int)sz * 4);

							if (need < 4)
							{								
								rec->offset += need;
							}					
						}
						else
						{
							rec->offset = 0;
						}
					
						file->Release();					
					}
				}
			}			
		}

		while(FindNextFile(h,&data));
	} 
	
	FindClose(h);
}

/*if (platform != Platform::platform_ios && !gen_package)
	{				
		SetProgressExportText("Generating resource pack... 0%s", "%");

		String::Copy(OutDataPath,511,ApplicationDir);
		String::Cat(OutDataPath,511,"\\Export\\");
		String::Cat(OutDataPath,511,platform_str);
		String::Cat(OutDataPath,511,"\\Data\\");	

		vector<FileService::FileRecord> records;
	
		char search_str[256];
		String::Printf(search_str, 256, "Export/%s/Data", platform_str);

		ScanDirectory(OutDataPath, search_str, "", records);
		
		String::Cat(OutDataPath,511,"Resource.pack");
	
		IFile* file = core->Files()->OpenFile(OutDataPath, IFile::write);

		if (file)
		{
			file->Write(&FileService::packMagicID, sizeof(dword));

			int count = records.size();
			file->Write(&count, sizeof(dword));
	
			int offset = sizeof(dword) + sizeof(dword) + sizeof(FileService::FileRecord) * count;
			
			for (int i=0; i<count; i++)
			{
				records[i].offset += offset;
			}

			file->Write(&records[0], sizeof(FileService::FileRecord) * count);			

			int index = 0;

			for (int i=0; i<count; i++)
			{	
				char file_path[256];
				String::Printf(file_path, 256, "%s/%s", search_str, records[i].filename);

 				IFileBuffer* fbuffer = core->Files()->LoadFile(file_path);

				if (fbuffer)
				{
					file->Write(fbuffer->GetBuffer(), fbuffer->GetSize());										

					float sz = (float)(fbuffer->GetSize())/4.0f;

					int need = 4 - (fbuffer->GetSize() - (int)sz * 4);

					if (need < 4)
					{
						file->Write(&need, need);
					}

					fbuffer->Release();
				}				

				index++;

				if (index > 10)
				{
					index = 0;
					int p = ((float)i / (float)count) * 100.0f;

					SetProgressExportText("Generating resource pack... %i%s", p, "%");
				}
			}

			SetProgressExportText("Generating resource pack... 100%s", "%");

 			file->Release();
		}
	}*/