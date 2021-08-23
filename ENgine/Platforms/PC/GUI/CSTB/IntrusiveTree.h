/**
Intrusive tree header.
Inspired from http://www.altdevblogaday.com/2011/12/06/intrusive-lists/

Copyright Adel Amro - http://code-section.com
*/

# ifndef CSTB_INTRUSIVE_TREE_H
# define CSTB_INTRUSIVE_TREE_H

# include "IntrusiveWeakTree.h"


namespace CSTB
{


template <typename T, size_t offset>
class IntrusiveTreeT
{
protected:
	typedef IntrusiveWeakTreeT<T, offset> WeakTreeType;
	typedef IntrusiveTreeT< T, offset > thisType;

	WeakTreeType m_weakTree;

public:
	IntrusiveTreeT() { }
	IntrusiveTreeT( const IntrusiveTreeT& rhs ) { *this = rhs; }

	~IntrusiveTreeT() { Clear(); }

	/* This is wrong
	IntrusiveTreeT& operator = (const IntrusiveTreeT& rhs )
	{
		if( this != &rhs )
		{
			Clear();
			for( T* p = rhs.First(); p; p = rhs.LogicalNext(p) )
				Add( *p, Parent(p) );
		}
		return *this;
	}
	*/

	/// Adds a copy of an existing element.
	T* Add( const T& cpy, T* parent = 0, T* pos = 0 )
	{
		T* pNew = new T(cpy);
		m_weakTree.Add( pNew, parent, pos );
		return pNew;
	}


	/// Adds a new element.
	T* Add( T* parent = 0, T* pos = 0 )
	{
		T* pNew = new T;
		m_weakTree.Add( pNew, parent, pos );
		return pNew;
	}

	void Remove( T* p )
	{
		while( FirstChild( p ) )
			Remove( FirstChild( p ) );
		m_weakTree.Remove( p );
		delete p;
	}

	void Clear() { while( First() ) Remove( First() ); }
	void Swap( IntrusiveTreeT& rhs ) { m_weakTree.Swap( rhs.m_weakTree ); }
	bool Empty() const { return m_weakTree.Empty(); }
	int Size() const { return m_weakTree.Size(); }

	T* First() const { return m_weakTree.First(); }
	T* Last() const { return m_weakTree.Last(); }
	static T* Parent(const T* p) { return WeakTreeType::Parent(p); }
	static T* Next(const T* p) { return WeakTreeType::Next(p); }
	static T* Prev(const T* p) { return WeakTreeType::Prev(p); }
	static T* FirstChild(const T* p) { return WeakTreeType::FirstChild(p); }
	static T* LastChild(const T* p) { return WeakTreeType::LastChild(p); }
	static T* LogicalNext(const T* p) { return WeakTreeType::LogicalNext(p); }
	static T* LogicalPrev(const T* p) { return WeakTreeType::LogicalPrev(p); }
};

}; // namespace CSTB


# endif // inclusion guard