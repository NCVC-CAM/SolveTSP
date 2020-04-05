// CustomClass.h: ¶½ÀÑ¸×½
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CUSTOMCLASS_H__66CF5DF6_13BB_48FA_AD68_B25CF189DC8C__INCLUDED_)
#define AFX_CUSTOMCLASS_H__66CF5DF6_13BB_48FA_AD68_B25CF189DC8C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
//	CTypedPtrArrayEx : Šg’£ CTypedPtrArray

template<class BASE_CLASS, class TYPE>
class CTypedPtrArrayEx : public CTypedPtrArray<BASE_CLASS, TYPE>
{
public:
	BOOL	IsEmpty(void) {
		return GetSize() == 0 ? TRUE : FALSE;
	}
	TYPE	GetHead(void) {
		ASSERT( !IsEmpty() );
		return GetAt(0);
	}
	TYPE	GetTail(void) {
		ASSERT( !IsEmpty() );
		return GetAt(GetUpperBound());
	}
	void	MakeReverse(void);
};

template<class BASE_CLASS, class TYPE>
void CTypedPtrArrayEx<BASE_CLASS, TYPE>::MakeReverse(void)
{
	if ( GetSize() <= 1 )	// µÌÞ¼Þª¸Ä1‚ÂˆÈ‰º‚È‚çŒðŠ·‚Ì•K—v‚È‚µ
		return;
	TYPE	pObject;
	int	ii = GetSize() >> 1;
	for ( int i=0, j=GetUpperBound(); i<ii; i++, j-- ) {
		pObject = GetAt(j);
		SetAt(j, GetAt(i));
		SetAt(i, pObject);
	}
}

//////////////////////////////////////////////////////////////////////
//	CSortArray : Sort‹@”\‚ðŽ‚Á‚½ CObArrayŒnºÝÃÅ

template<class BASE_CLASS, class TYPE>
class CSortArray : public CTypedPtrArrayEx<BASE_CLASS, TYPE>
{
public:
	typedef	int	(*PFNCOMPARE)(TYPE pFirst, TYPE pSecond);
private:
	void	QSort(PFNCOMPARE pfnCompare, int nFirst, int nLast);

public:
	void	Sort(PFNCOMPARE pfnCompare) {
		if ( GetSize() > 1 )
			QSort( pfnCompare, 0, GetUpperBound());
	}
};

template<class BASE_CLASS, class TYPE>
void CSortArray<BASE_CLASS, TYPE>::QSort(PFNCOMPARE pfnCompare, int nFirst, int nLast)
{
	if ( nFirst >= nLast )
		return;

	int		i = nFirst + 1, j = nLast;
	TYPE*	pObj = (TYPE *)GetData();
	TYPE	pObjWork;

	while ( i <= j ) {
		// Šî€’l‚ª‘å‚«‚¢ŠÔŒJ‚è•Ô‚µ
		for ( ; i <= j && (*pfnCompare)(pObj[i], pObj[nFirst]) <= 0; i++ );
		// Šî€’l‚ª¬‚³‚¢ŠÔŒJ‚è•Ô‚µ
		for ( ; i <= j && (*pfnCompare)(pObj[j], pObj[nFirst]) >= 0; j-- );
		//
		if ( i < j ) {
			pObjWork = pObj[i];
			pObj[i] = pObj[j];
			pObj[j] = pObjWork;
			i++;	j--;
		}
	}

	pObjWork = pObj[j];
	pObj[j] = pObj[nFirst];
	pObj[nFirst] = pObjWork;

	QSort( pfnCompare, nFirst, j-1 );
	QSort( pfnCompare, j+1, nLast );
}

#endif // !defined(AFX_CUSTOMCLASS_H__66CF5DF6_13BB_48FA_AD68_B25CF189DC8C__INCLUDED_)
