
#include "Platforms/PC/Runner.h"
#include "Core/ICore.h"

char** CommandLineToArgvA(const char* CmdLine, int* _argc)
{
	PCHAR* argv;
    PCHAR  _argv;
    ULONG   len;
    ULONG   argc;
    CHAR   a;
    ULONG   i, j;

    BOOLEAN  in_QM;
    BOOLEAN  in_TEXT;
    BOOLEAN  in_SPACE;

    len = strlen(CmdLine);
    i = ((len+2)/2)*sizeof(PVOID) + sizeof(PVOID);

    argv = (PCHAR*)GlobalAlloc(GMEM_FIXED, i + (len+2)*sizeof(CHAR));

    _argv = (PCHAR)(((PUCHAR)argv)+i);

    argc = 0;
    argv[argc] = _argv;
    in_QM = FALSE;
    in_TEXT = FALSE;
    in_SPACE = TRUE;
    i = 0;
    j = 0;

    while( a = CmdLine[i] )
	{
		if (in_QM)
		{
			if (a == '\"')
			{
				in_QM = FALSE;
            }
			else
			{
				_argv[j] = a;
                j++;
            }
        }
		else
		{
			switch(a)
			{
                case '\"':

                    in_QM = TRUE;
                    in_TEXT = TRUE;
                    
					if (in_SPACE)
					{
                        argv[argc] = _argv+j;
                        argc++;
                    }

                    in_SPACE = FALSE;

                    break;
                case ' ':
                case '\t':
                case '\n':
                case '\r':

                    if (in_TEXT)
					{
                        _argv[j] = '\0';
                        j++;
                    }
                    in_TEXT = FALSE;
                    in_SPACE = TRUE;
                    
					break;

                default:
                    in_TEXT = TRUE;
                    
					if (in_SPACE)
					{
                        argv[argc] = _argv+j;
                        argc++;
                    }

                    _argv[j] = a;
                    j++;
                    in_SPACE = FALSE;

                    break;
            }
        }

        i++;
    }

    _argv[j] = '\0';
    argv[argc] = NULL;

    (*_argc) = argc;

    return argv;
}

int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int showCmd )
{		
	char** szArglist;
   
	int nArgs;    
    szArglist = CommandLineToArgvA(cmdLine, &nArgs);		

	core->TraceTo("App", cmdLine);

	for (int i=0;i<nArgs;i++)
	{
		core->TraceTo("App", szArglist[i]);
	}

	char buff[9];
	memset(buff,0,9);
	String::Copy(buff,8,cmdLine); 
	if (String::IsEqual(buff, "-editor"))
	{
		return RunEditor(hInstance, nArgs, szArglist);
	}

	if (cmdLine[0] == '-' && cmdLine[1] == 'e' && cmdLine[2] == 'x')
	{		
		return RunExport(nArgs, szArglist);
	}

	RunENgine(hInstance, nArgs, szArglist);
	
	return 0;
}
