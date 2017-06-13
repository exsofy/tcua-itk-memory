#ifndef _XFY_ITK_ARRAY_H_INCLUDED_
#define _XFY_ITK_ARRAY_H_INCLUDED_

#include "xfy_itk_string.h"

template < class T > class XFYITKArray
{
protected:
  T* pArray;
  int iCount;
  int iCapacity;            // reserved capacity
  void* pCapacityReference; // pointer for which is the capacity valid
  int iGrowBy;              // capacity increment
public:
  XFYITKArray( const T* pa, const int pc) { pCapacityReference = pArray = NULL; iCapacity = 0; iGrowBy = 32; iCount = 0; Add ( pa, pc ); }
  XFYITKArray() { pCapacityReference = pArray = NULL; iCount = 0; iCapacity = 0; iGrowBy = 32; }
  XFYITKArray( int Size, int Grow = 32 ) { pCapacityReference = pArray = ( ( Size > 0 ) ? (T*)MEM_alloc ( sizeof ( T ) * Size ) : NULL ); iCount = Size; iCapacity = Size; iGrowBy = Grow; }
//  ITKArray( const ITKArray<T> &source ) : ITKArray() { Add ( &source, source.iCount ); } // SCO add
  ~XFYITKArray() { if (pArray) MEM_free(pArray); pArray = NULL; };

  T** operator &() { return &pArray; };
  operator T*() const { return pArray; };
  T &operator [](const int iIndex)  const { return pArray[iIndex]; };

  int *PtrSize() { return &iCount; };

  const int GetSize() const { return iCount; };
  const int GetUpperBound() const { return iCount - 1; };

  const T* GetData() const { return pArray; }

  void SetSize ( int nNewSize, int nGrowBy = -1 )
  {
    if ( nGrowBy > 0 ) iGrowBy = nGrowBy;
    if ( ( nNewSize > iCapacity ) || ( pCapacityReference != pArray ) )
    {
      Resize( nNewSize );
    }
    iCount = nNewSize;
  }

  /* Add new element to the array */
  int Add( const T &newElement)
  {
    if ( ( iCount >= iCapacity ) || ( pCapacityReference != pArray ) )
    {
      Resize();
    }

    pArray[iCount] = newElement;
    return iCount++;
  }
  
  /* Add iLen new elements to the array */
  int Add( const T *newElement, const int iLen )
  {
    if ( ( iCount + iLen >= iCapacity ) || ( pCapacityReference != pArray ) )
    {
      Resize( iCount + iLen );
    }
    for ( int iI = 0; iI < iLen; iI++ )
    {
      pArray[iCount+iI] = newElement[iI];
    }
    iCount += iLen;
    return iCount;
  }

  /* Add iLen copies of the element */
  int Add( const T newElement, const int iLen )
  {
    if ( ( iCount + iLen >= iCapacity ) || ( pCapacityReference != pArray ) )
    {
      Resize( iCount + iLen );
    }
    
    for ( int iI = 0; iI < iLen; iI++ )
    {
      pArray[iCount+iI] = newElement;
    }

    iCount += iLen;
    return iCount;
  }

  void RemoveAt( const int iIndex, const int iSize = 1 )
    { if ( iCount - ( iIndex + iSize ) > 0 )
        memmove ( (unsigned char *)pArray + iIndex * sizeof ( T ), 
                  (unsigned char *)pArray + ( iIndex + iSize ) * sizeof ( T ),
                  ( iCount - ( iIndex + iSize ) ) * sizeof ( T ) );
      iCount -= iSize; }

  void RemoveAll() { iCount = 0; Resize(0); };

  void Detach ( T *&outArray, int &count ) { outArray = pArray; count = iCount; pCapacityReference = pArray = NULL; iCount = 0; iCapacity = 0; };  // detach  array for external use
  void Attach ( T *inArray, int count ) { RemoveAll(); pCapacityReference = pArray = inArray; iCount = iCapacity = count; };  // attach array from existing memory place

protected:

  void Resize ( int nNewSize = -1 )
  {
    if ( pCapacityReference != pArray ) 
    { // for data pointer changed outwards
      iCapacity = iCount;
      pCapacityReference = pArray; 
    }
    if ( nNewSize < 0 ) nNewSize = iCapacity + iGrowBy;
    if ( nNewSize > 0 )
    {
      iCapacity = nNewSize; // define the new size
      pCapacityReference = pArray = (T*)MEM_realloc ( pArray, sizeof ( T ) * ( iCapacity ) );
    }
    else
    {
      iCapacity = 0; // define the new size
      MEM_free ( pArray );
      pCapacityReference = pArray = NULL;
    }
  }

};

typedef XFYITKArray<tag_t>  XFYITKTagArray;
typedef XFYITKArray<int>    XFYITKIntArray;
typedef XFYITKArray<logical>    XFYITKLogicalArray;

#ifdef GRM_H
typedef XFYITKArray<GRM_relation_t> XFYITKRelationArray;
#endif

#ifdef ITEM_H
typedef XFYITKArray<ITEM_attached_object_t> XFYITKITEMAttachedArray;
#endif

#ifdef WORKSPACEOBJECT_H
typedef XFYITKArray<WSO_status_t> XFYITKWSOStatusArray;
#endif

#ifdef AE_H
typedef XFYITKArray<AE_reference_type_t> XFYITKAEReferenceTypeArray;
#endif

/* borrow the old function untill a MEM_length is not available */
int SM_length( const void * );

class XFYITKStringArray : public XFYITKArray<char *>
{
public:
	~XFYITKStringArray() { if (pArray) { freeElements ( 0, iCount ); MEM_free(pArray); pArray = NULL; } };

	void RemoveAt( const int iIndex, const int iSize = 1 )
	{
	  freeElements ( iIndex, iSize );
	  XFYITKArray<char *>::RemoveAt ( iIndex, iSize );
	}

	void RemoveAll()
	{
	  freeElements ( 0, iCount );
      XFYITKArray<char *>::RemoveAll();
        }

  int Add( const char * const &newElement)
  { // add copy
    return XFYITKArray<char *>::Add ( XFY_ITK_string_copy ( newElement ) );
  }

protected:
  
  // release the memory used by the elements
  void freeElements ( const int iIndex, const int iSize = 1 )
  { 
    if (pArray)
    {
      long lSize = SM_length ( pArray );

      for ( int iPos = iIndex; // from index
	      ( iPos < iIndex + iSize ) && ( iPos < iCount ); // until size or count reached
	      iPos++ )
      { // free the memory attached to the element
  		  if ( ( pArray[iPos] != NULL ) && !( ( (void *)(pArray[iPos]) > pArray ) && ( (void *)(pArray[iPos]) < pArray + lSize ) ) ) 
        { // not null and not inside of the array
	        MEM_free ( pArray[iPos] );
        }
        pArray[iPos] = NULL;
      }
    }
  }

  void Resize ( int nNewSize = -1 )
  {
    if ( nNewSize == 0 )
	{ // set size to 0, removes all elemets
	  freeElements ( 0, iCount );
	}
	XFYITKArray<char *>::Resize( nNewSize );
  }
};



#endif /* _XFY_ARRAY_ITK_H_INCLUDED_ */
