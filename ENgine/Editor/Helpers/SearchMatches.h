#pragma once
#include <vector>
#include <string>
#include "Common\Containers\HashMap.h"
struct SearchMatch
{
	string sLine;

	int nStart;
	int nEnd;
	int nRow;
	int nCol;
	int vSelectionIndexs[2];
	int vCutSelection[2];
	std::vector<char> vOldStyles;
	SearchMatch():nStart(-1),nEnd(-1),nRow(-1),nCol(-1){Init();}
	SearchMatch(int _start,int _end):nStart(_start),nEnd(_end),nRow(-1),nCol(-1){Init();}
	SearchMatch(int _start,int _end,int _row,int _col):nStart(_start),nEnd(_end),nRow(_row),nCol(_col){ Init();}
	SearchMatch(int _start,int _end,int _row,int _col,const string& _line):nStart(_start),nEnd(_end),nRow(_row),nCol(_col),sLine(_line){ Init();}
	SearchMatch(const SearchMatch& other){Copy(other);}
	const SearchMatch& operator = (const SearchMatch& other)
	{
		Copy(other);
		return *this;
	}
	const SearchMatch& operator += (int nCount)
	{
		nStart += nCount;
		nEnd += nCount;
		if(vCutSelection[0] != -1)
			vCutSelection[0] += nCount;
		if(vCutSelection[1] != -1)
			vCutSelection[1] += nCount;
		return *this;
	}
private:
	void Init()
	{
		vCutSelection[0] = vCutSelection[1] = vSelectionIndexs[0] = vSelectionIndexs[1] = -1;
	}
	void Copy(const SearchMatch& other)
	{
		sLine = other.sLine;
		nRow = other.nRow;
		nCol = other.nCol;
		nStart = other.nStart;
		nEnd = other.nEnd;
		memcpy(vSelectionIndexs,other.vSelectionIndexs,2 * sizeof(int));
		memcpy(vCutSelection,other.vCutSelection,2 * sizeof(int));
		vOldStyles.assign(other.vOldStyles.begin(),other.vOldStyles.end());
	}

};

typedef vector<SearchMatch> TMatches;
typedef HashTable<string,TMatches> TMatchesMap;