
#pragma once

#include <vector>
#include "Common/Containers/BaseContainer.h"
#include "Common/CrossString.h"
#include "Common/Common.h"

template<class T>
class HashMap : public BaseContainer
{	
	struct MapIndex
	{
		unsigned int hash;

		char name[512];
		T* ptr;		
	};

	std::vector<MapIndex*> names;	

public:	
	
	HashMap(const char* fl, int ln)
	{
		String::Copy(file, sizeof( file ), fl);
		line = ln;
	}

	inline T* Add(const char* id)
	{		
		//T* ptr = (T*) Alloc(sizeof(T));
		//new (ptr) T();	

		T* ptr = new T();	

		Add(id, ptr);		
				
		return ptr;
	}

	inline void Add(const char* id, T* ptr)
	{		
		MapIndex* m_index = (MapIndex*) Alloc(sizeof(MapIndex));
		new (m_index) MapIndex;

		String::Copy(m_index->name, sizeof( m_index->name ), id);
		m_index->hash = HashValue(id);
		m_index->ptr = ptr;
		names.push_back(m_index);	
	}

	inline T* Get(int index)
	{	
		return names[index]->ptr;
	}

	inline T* Get(unsigned int hash)
	{							
		for (int i=0;i<(int)names.size();i++)
		{
			if (names[i]->hash == hash)
			{
				return names[i]->ptr;
			}
		}

		return NULL;
	}

	inline const char* GetName(int index)
	{
		return names[index]->name;
	}	

	inline const char* GetName(unsigned int hash)
	{							
		for (int i=0;i<(int)names.size();i++)
		{
			if (names[i]->hash == hash)
			{
				return names[i]->name;
			}
		}

		return NULL;
	}

	inline const char* GetName(T* ptr)
	{							
		for (int i=0;i<(int)names.size();i++)
		{
			if (names[i]->ptr == ptr)
			{
				return names[i]->name;
			}
		}

		return NULL;
	}

	inline const char* FindName(T* ptr)
	{		
		for (int i=0;i<(int)names.size();i++)
		{
			if (names[i]->ptr == ptr)
			{
				return names[i]->name;
			}
		}

		return NULL;
	}

	inline T* Get(const char* id)
	{				
		return Get(HashValue(id));
	}	

	inline const T* Get(const char* id) const
	{				
		unsigned int hash = HashValue(id);

		for (int i=0;i<(int)names.size();i++)
		{
			if (names[i]->hash == hash)
			{
				return names[i]->ptr;
			}
		}

		return NULL;
	}	

	inline int GetIndex(const char* id) const
	{				
		unsigned int hash = HashValue(id);

		for (int i=0;i<(int)names.size();i++)
		{
			if (names[i]->hash == hash)
			{
				return i;
			}
		}

		return -1;
	}	

	inline void Delete(unsigned int hash, bool destroy = true)
	{		
		for (int i=0;i<(int)names.size();i++)
		{
			if (names[i]->hash == hash)
			{
				Delete(i, destroy);
				return;				
			}
		}		
	}

	inline void Delete(int i, bool destroy = true)
	{		
		//if (destroy) Dealloc(names[i]->ptr);
		if (destroy) delete names[i]->ptr;
		
		names[i]->~MapIndex();
		Dealloc(names[i]);

		//delete names[i];

		for (int j=i;j<(int)names.size()-1;j++)
		{
			names[j] = names[j+1];
		}
		names.pop_back();
	}

	inline void Delete(T* ptr, bool destroy = true)
	{		
		const char* nm = FindName(ptr);

		if (nm)
		{
			Delete(HashValue(nm), destroy);
		}
	}

	inline int Size()
	{
		return (int)names.size();
	}

	inline void Clear(bool destroy = true)
	{				
		for (int j=0;j<(int)names.size();j++)
		{
			if (destroy) delete names[j]->ptr;

			names[j]->~MapIndex();
			Dealloc(names[j]);

			//delete names[j];
		}
		
//#ifdef _DEBUGLEAKS
		std::vector<MapIndex*> _tmp;
		names.swap(_tmp);
//#else
//		names.clear();
//#endif
	}	
};

////////////////////////////////HashTable class template //////////////////////////////////////////
template < typename TKey, typename TValue>
class HashTable
{
    template < typename TObjectKey,  typename TObject>
    class TableIterator
    {
        friend class HashTable;
    protected:
        TObjectKey  pCurrentKey;
        TObject*    pCurrent;
        int         nCurrent;
        int         nCurrentIndex;
        int         nCurrentListIndex;
        HashTable*  pOwner;
        TableIterator< TObjectKey,TObject>& operator = (const TableIterator< TObjectKey,TObject>& other);
        TableIterator(const  TableIterator< TObjectKey,TObject>& other){}
    public:
        TableIterator():pCurrent(NULL),nCurrent(0),nCurrentIndex(0),nCurrentListIndex(0){}
        inline void SetOwner(HashTable*  owner){pOwner = owner;}
        inline void Reset(){pCurrent = NULL;nCurrent = nCurrentIndex = nCurrentListIndex = 0;}
        inline void Reset(unsigned current){pCurrent = NULL;nCurrent = current;nCurrentIndex = nCurrentListIndex = 0;}

        inline bool operator == (const TableIterator<TObjectKey,TObject>& other){return nCurrent > other.nCurrent;}
        inline bool operator != (const TableIterator<TObjectKey,TObject>& other){return nCurrent <= other.nCurrent;}
        inline const TableIterator<TObjectKey,TObject>& operator++()
        {
			++nCurrent;
			for(unsigned i = nCurrentIndex;i < pOwner->table.size();++i,++nCurrentIndex)
			{
				if(!pOwner->table[i].empty())
				{
					pCurrent = &pOwner->table[i][nCurrentListIndex].second;
					pCurrentKey = pOwner->table[i][nCurrentListIndex].first;
					++nCurrentListIndex;
					if(nCurrentListIndex >= (int)pOwner->table[i].size())
					{
						nCurrentListIndex = 0;
						++nCurrentIndex;
					}
					return *this;
				}
			}
            return *this;
        }
        inline TKey     GetKey(){return pCurrentKey;} 
        inline TObject* operator*(){return pCurrent;}

    };

	template < typename TObjectKey,  typename TObject>
	class TableReverseIterator
	{
		friend class HashTable;
	protected:
		TObjectKey  pCurrentKey;
		TObject*    pCurrent;
		int         nCurrent;
		int         nCurrentIndex;
		int         nCurrentListIndex;
		HashTable*  pOwner;
	protected:
		TableReverseIterator< TObjectKey,TObject>& operator = (const TableReverseIterator< TObjectKey,TObject>& other);
		TableReverseIterator(const  TableReverseIterator< TObjectKey,TObject>& other){}
	public:
		TableReverseIterator():pCurrent(NULL),nCurrent(0),nCurrentIndex(0),nCurrentListIndex(0){}
		inline void SetOwner(HashTable*  owner){pOwner = owner;}
		inline bool operator == (const TableReverseIterator<TObjectKey,TObject>& other){return nCurrent < other.nCurrent;}
		inline bool operator != (const TableReverseIterator<TObjectKey,TObject>& other){return nCurrent >= other.nCurrent;}
		inline void Reset(){pCurrent = NULL;nCurrent = nCurrentIndex = nCurrentListIndex = 0;}
		inline void Reset(unsigned current)
		{
			pCurrent = NULL;
			nCurrent = current;
			nCurrentIndex = pOwner->table.size() - 1;
			for(;nCurrentIndex >= 0;--nCurrentIndex)
			{
				if(!pOwner->table[nCurrentIndex].empty())
				{
					nCurrentListIndex = (int)pOwner->table[nCurrentIndex].size() - 1;
					return;
				}
			}
		}
		inline const TableReverseIterator<TObjectKey,TObject>& operator++()
		{
			--nCurrent;
			for(;nCurrentIndex >= 0;--nCurrentIndex)
			{
				if(!pOwner->table[nCurrentIndex].empty())
				{
					pCurrent = &pOwner->table[nCurrentIndex][nCurrentListIndex].second;
					pCurrentKey = pOwner->table[nCurrentIndex][nCurrentListIndex].first;
					--nCurrentListIndex;
					if(nCurrentListIndex < 0)
					{
						--nCurrentIndex;
						for(;nCurrentIndex >= 0;--nCurrentIndex)
						{
							if(!pOwner->table[nCurrentIndex].empty())
							{
								nCurrentListIndex = (int)pOwner->table[nCurrentIndex].size() - 1;
								break;
							}
						}
					}
					return *this;
				}
			}
			return *this;
		}
		inline TKey     GetKey(){return pCurrentKey;} 
		inline TObject* operator*(){return pCurrent;}
	};



    template < typename TObjectKey,  typename TObject>
    class Pair
    {
    public:
        TObjectKey first;
        TObject    second;
        Pair(TObjectKey key,TObject value):first(key),second(value){}
    };

    typedef std::vector< Pair< TKey,TValue> > TLinkedList;


private:
    unsigned static const defaultCapacity = 1024;
    std::vector< TLinkedList > table;
    unsigned capacity;
    unsigned count;

    TableIterator<TKey,TValue> iterator;
    TableIterator<TKey,TValue> end;

	TableReverseIterator<TKey,TValue> reIterator;
	TableReverseIterator<TKey,TValue> reEnd;

    unsigned HashKey(void* key) const;
    unsigned HashKey(int key) const;
    unsigned HashKey(const char* key) const;
    unsigned HashKey(const string& key) const;
  
    std::vector< TValue* > vBuffer;
    void  Copy(const HashTable<TKey,TValue>& copy);
   
public:
    typedef TableIterator<TKey,TValue>&			Iterator;
    typedef TableReverseIterator<TKey,TValue>&  ReversIterator;

	HashTable();
    HashTable(unsigned divisor);
    HashTable(const  HashTable& other){Copy(other);}
    ~HashTable();

    void                Insert(const TKey& key_val,const TValue& el_value);
    void                Add(const TKey& key_val,const TValue& el_value);
    const TValue*       Find(const TKey& key) const;
    TValue*             Find(const TKey& key);
    void                Delete(TKey key);
    void                Delete(TableIterator<TKey,TValue>& iterator);
    void                Clear();
    inline Iterator     Begin(){iterator.Reset();++iterator;return iterator;} 
    inline Iterator     End(){return end;}

	inline ReversIterator    ReBegin(){reIterator.Reset(count);++reIterator;return reIterator;} 
	inline ReversIterator    ReEnd(){return reEnd;}

    bool                IsEmpty()const {return count == 0;}
    unsigned            Size()const {return count;}

    const  std::vector<TValue*>& GetList(const TKey& key);

    TValue& operator[](TKey key);
    HashTable< TKey,TValue>& operator = (const HashTable<TKey,TValue>& other);
};


template < typename TKey, typename TValue>
void HashTable<TKey, TValue>::Copy(const HashTable<TKey,TValue>& copy)
{
    Clear();
    table.resize(copy.table.size());
    capacity = copy.capacity;
    count = copy.count;
    for(unsigned i = 0; i < copy.table.size(); ++i)
    {
        if(!copy.table[i].empty())
            table[i].assign(copy.table[i].begin(),copy.table[i].end());
    }
    iterator.Reset();
    end.Reset(count);
}

template < typename TKey, typename TValue>
const std::vector<TValue*>& HashTable<TKey, TValue>::GetList(const TKey& key)
{
    vBuffer.clear();
    unsigned pos = HashKey(key);
    TLinkedList& list = table.at(pos);
    for(unsigned i = 0; i < list.size();++i)
        if(key == list[i].first)
            vBuffer.push_back(&list[i].second);
    return vBuffer;
}

template < typename TKey, typename TValue>
HashTable< TKey,TValue>& HashTable<TKey, TValue>::operator=(const HashTable<TKey,TValue>& other)
{
    Copy(other);
    return *this;
}

template < typename TKey, typename TValue>
unsigned HashTable<TKey, TValue>::HashKey(void* key) const
{
    static char sAdress[64];
    String::Printf(sAdress,64,"%d",key);
    return HashKey(sAdress) % capacity;
}

template < typename TKey, typename TValue>
unsigned HashTable< TKey,TValue>::HashKey(int key) const
{
    return key % capacity;
}

template < typename TKey, typename TValue>
unsigned HashTable< TKey,TValue>::HashKey(const char* key) const
{
    return HashValue(key) % capacity;
}
template < typename TKey, typename TValue>
unsigned HashTable< TKey,TValue>::HashKey(const string& key) const
{
    return HashValue(key.c_str()) % capacity;
}

template < typename TKey, typename TValue>
HashTable< TKey,TValue>::HashTable()
{
    count=0;
    capacity=defaultCapacity;
    table.resize(defaultCapacity);
    iterator.SetOwner(this);
	reIterator.SetOwner(this);
    iterator.Reset();
    end.Reset();
	reIterator.Reset();
	reEnd.Reset();
}

template < typename TKey, typename TValue>
HashTable< TKey,TValue>::HashTable(unsigned divisor):capacity(divisor)
{
    count=0;
    table.resize(capacity);
    iterator.SetOwner(this);
	reIterator.SetOwner(this);
	reIterator.Reset();
	reEnd.Reset();
    iterator.Reset();
    end.Reset();
}

template < typename TKey, typename TValue>
HashTable< TKey,TValue>::~HashTable()
{
    table.clear();
}
template < typename TKey, typename TValue>
void    HashTable< TKey,TValue>::Clear()
{
    count = 0;
    for(unsigned i = 0;i < table.size();++i)table[i].clear();
    iterator.Reset();
    end.Reset();
	reIterator.Reset();
	reEnd.Reset();
}
template < typename TKey, typename TValue>
const TValue* HashTable< TKey,TValue>::Find(const TKey& key) const
{
    unsigned pos = HashKey(key);
    const TLinkedList& list = table.at(pos);
    if(!list.empty())
    {
        for(unsigned i = 0; i < list.size(); ++i) 
            if(list[i].first == key)
                return &(list[i].second); 
    }
    return NULL;
}
template < typename TKey, typename TValue>
TValue* HashTable< TKey,TValue>::Find(const TKey& key)
{
    unsigned pos = HashKey(key);
    TLinkedList& list = table.at(pos);
    if(!list.empty())
    {
        for(unsigned i = 0; i < list.size(); ++i) 
            if(list[i].first == key)
                return &(list[i].second); 
    }
    return NULL;
}


template < typename TKey, typename TValue>
void HashTable< TKey,TValue>::Insert(const TKey& key_val,const TValue& el_value)
{
    unsigned pos = HashKey(key_val);
    TLinkedList& list = table.at(pos);
    if(!list.empty()) 
    {
        for(unsigned i = 0; i < list.size(); ++i) 
        {
            if(list[i].first == key_val)
            {
                list[i].second = el_value;
                return;
            } 
        }
    }
    Pair< TKey,TValue> toInsert(key_val, el_value);
    table[pos].push_back(toInsert);
    ++count;
    end.Reset(count);

}
template < typename TKey, typename TValue>
void HashTable< TKey,TValue>::Add(const TKey& key_val,const TValue& el_value)
{
    unsigned pos = HashKey(key_val);
    Pair< TKey,TValue> toInsert(key_val, el_value);
    table[pos].push_back(toInsert);
    ++count;
    end.Reset(count);
}

template < typename TKey, typename TValue>
TValue& HashTable< TKey,TValue>::operator[](TKey key)
{
    unsigned pos=HashKey(key);
    TLinkedList& list = table.at(pos);
    if(!list.empty())
    {
        for(unsigned i = 0; i < list.size(); ++i) 
            if(list[i].first == key)
                return list[i].second; 
    }
    Insert(key,TValue());
    return operator[](key);
}
template < typename TKey, typename TValue>
void HashTable< TKey,TValue>::Delete(TKey key)
{
    unsigned pos=HashKey(key);
    TLinkedList& list = table.at(pos);
    if(!list.empty())
    {
       for(unsigned i = 0; i < list.size(); ++i) 
            if(list[i].first == key)
            {
                for(unsigned j = i; j < list.size() - 1; ++j) 
                    list[j] = list[j + 1];
                list.pop_back();
                --count;
                end.Reset(count);
            }
    }
}
template < typename TKey, typename TValue>
void  HashTable< TKey,TValue>::Delete(TableIterator<TKey,TValue>& iterator)
{
    Delete(iterator.GetKey());
    --(iterator.nCurrent);
    if(iterator.nCurrent < 0)iterator.nCurrent = 0;
    --(iterator.nCurrentListIndex);
    if(iterator.nCurrentListIndex < 0)iterator.nCurrentListIndex = 0;
    --(iterator.nCurrentIndex);
    if(iterator.nCurrentIndex < 0)iterator.nCurrentIndex = 0;
}