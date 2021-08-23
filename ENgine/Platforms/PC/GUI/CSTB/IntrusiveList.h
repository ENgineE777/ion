# ifndef CSTB_INTRUSIVE_LIST_H
# define CSTB_INTRUSIVE_LIST_H

# include "IntrusiveWeakList.h"


namespace CSTB
{



/// An intrusive list owns the elements (nodes) it contains. They will be deleted when the list
/// is deleted or cleared. The main advantage of an intrusive list over a regular list is that
/// element pointers are used directly to iterate the list instead of using iterators.
template <typename T, size_t offset>
class IntrusiveListT
{
protected:
	typedef IntrusiveWeakListT<T, offset> WeakListType;
	WeakListType	m_weakList;


public:
	IntrusiveListT(){}
	IntrusiveListT( const IntrusiveListT& rhs ) { *this = rhs; }
	IntrusiveListT( T* pStart, T* pEnd ) { for( T* p = pStart; p!=pEnd; p = Next(p) ) Insert(*p); }

	~IntrusiveListT() { Clear(); }


	IntrusiveListT& operator = (const IntrusiveListT& rhs )
	{
		if( this != &rhs )
		{
			Clear();
			for( T* p = rhs.First(); p != 0; p = rhs.Next( p ) )
				Insert( *p );
		}
		return *this;
	}

	bool		Empty() const { return m_weakList.Empty(); }
	int			Size() const { return m_weakList.Size(); }
	T*			First() const { return m_weakList.First(); }
	T*			Last() const { return m_weakList.Last(); }
	static T*	Next( const T* p ) { return WeakListType::Next(p); }
	static T*	Prev( const T* p ) { return WeakListType::Prev(p); }

	T* Insert( const T& cpy, T* pInsertBefore = NULL )
	{
		T* pNew = new T( cpy );
		m_weakList.Add( pNew, pInsertBefore );
		return pNew;
	}

	T* Insert( T* pInsertBefore = NULL )
	{
		T* pNew = new T;
		m_weakList.Add( pNew, pInsertBefore );
		return pNew;
	}

	void Remove( T* p )
	{
		m_weakList.Remove( p );
		delete p;
	}

	void Move( T* p, T* pInsertBefore = 0 )
	{ m_weakList.Move( p, pInsertBefore ); }

	void Move( T* p, IntrusiveListT& otherList, T* pInsertPos = 0 )
	{ m_weakList.Move( p, otherList.m_weakList, pInsertPos ); }

	void Move( T* pFirst, T* pLast, IntrusiveListT& otherList, T* pInsertPos = 0 )
	{ m_weakList.Move( pFirst, pLast, otherList.m_weakList, pInsertPos ); }

	void MoveAll( IntrusiveListT& otherList, T* pInsertPos = 0 )
	{ Move( First(), Last(), otherList, pInsertPos ); }

	void Swap( IntrusiveListT& rhs ) { m_weakList.Swap( rhs.m_weakList ); }

	void Clear()
	{
		while( Last() )
			Remove( Last() );
	}
};


# define INTRUSIVE_LIST( type, hookMember ) CSTB::IntrusiveListT< type, offsetof(type, type::hookMember) >


}; // namespace CSTB

# endif // include guard