#ifndef XFY_ITK_ARRAY_H_
#define XFY_ITK_ARRAY_H_

#include <string.h>
#include "xfy_itk_string.h"

namespace XFY {

template < class T > class ITKArray
{
protected:
  T* pArray;
  int iCount;
  int iCapacity;            // reserved capacity
  void* pCapacityReference; // pointer for which is the capacity valid
  int iGrowBy;              // capacity increment
public:
  ITKArray( const T* pa, const int pc) { pCapacityReference = pArray = NULL; iCapacity = 0; iGrowBy = 32; iCount = 0; Add ( pa, pc ); }
  ITKArray() { pCapacityReference = pArray = NULL; iCount = 0; iCapacity = 0; iGrowBy = 32; }
  ITKArray( int Size, int Grow = 32 ) { pCapacityReference = pArray = ( ( Size > 0 ) ? (T*)MEM_alloc ( sizeof ( T ) * Size ) : NULL ); iCount = Size; iCapacity = Size; iGrowBy = Grow; }
//  ITKArray( const ITKArray<T> &source ) : ITKArray() { Add ( &source, source.iCount ); } // SCO add
  ~ITKArray() { if (pArray) MEM_free(pArray); pArray = NULL; };

  T** operator &() { return &pArray; };
  operator T*() const { return pArray; };
  T &operator [](const int iIndex)  const { return pArray[iIndex]; };

  int *sizePtr() { return &iCount; };

  const int size() const { return iCount; };

  const T* data() const { return pArray; }

  void resize ( int nNewSize, int nGrowBy = -1 )
  {
    if ( nGrowBy > 0 ) iGrowBy = nGrowBy;
    if ( ( nNewSize > iCapacity ) || ( pCapacityReference != pArray ) )
    {
      Resize( nNewSize );
    }
    iCount = nNewSize;
  }

  /* Add new element to the array */
  int push_back( const T &newElement)
  {
    if ( ( iCount >= iCapacity ) || ( pCapacityReference != pArray ) )
    {
      Resize();
    }

    pArray[iCount] = newElement;
    return iCount++;
  }
  
  /* Add iLen new elements to the array */
  int insert( const T *newElement, const int iLen )
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
  int insert( const T newElement, const int iLen )
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

  void erase( const int iIndex, const int iSize = 1 )
    { if ( iCount - ( iIndex + iSize ) > 0 )
        memmove ( (unsigned char *)pArray + iIndex * sizeof ( T ), 
                  (unsigned char *)pArray + ( iIndex + iSize ) * sizeof ( T ),
                  ( iCount - ( iIndex + iSize ) ) * sizeof ( T ) );
      iCount -= iSize; }

  void erase() { iCount = 0; Resize(0); };

  void detach ( T *&outArray, int &count ) { outArray = pArray; count = iCount; pCapacityReference = pArray = NULL; iCount = 0; iCapacity = 0; };  // detach  array for external use
  void attach ( T *inArray, int count ) { erase(); pCapacityReference = pArray = inArray; iCount = iCapacity = count; };  // attach array from existing memory place

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

typedef ITKArray<tag_t>  ITKTagArray;
typedef ITKArray<int>    ITKIntArray;
typedef ITKArray<logical>    ITKLogicalArray;

#ifdef GRM_H
typedef ITKArray<GRM_relation_t> ITKRelationArray;
#endif

#ifdef ITEM_H
typedef ITKArray<ITEM_attached_object_t> ITKITEMAttachedArray;
#endif

#ifdef WORKSPACEOBJECT_H
typedef ITKArray<WSO_status_t> ITKWSOStatusArray;
#endif

#ifdef AE_H
typedef ITKArray<AE_reference_type_t> ITKAEReferenceTypeArray;
#endif

/* borrow the old function until a MEM_length is not available */
extern "C" int SM_length( const void * );

class ITKStringArray : public ITKArray<char *>
{
public:
	operator const char**() const { return (const char**)pArray; };
	operator char**() const { return pArray; };
	
	~ITKStringArray() { if (pArray) { freeElements ( 0, iCount ); MEM_free(pArray); pArray = NULL; } };

	void erase( const int iIndex, const int iSize = 1 )
	{
	  freeElements ( iIndex, iSize );
	  ITKArray<char *>::erase ( iIndex, iSize );
	}

	void erase()
	{
	  freeElements ( 0, iCount );
      ITKArray<char *>::erase();
        }

  int insert( const char * const &newElement)
  { // add copy
	  return ITKArray<char *>::push_back ( MEM_string_copy ( newElement ) );
  }

  int push_back ( const XFY::ITKString &newElement )
  {
	   // add copy
	  return ITKArray<char *>::push_back ( MEM_string_copy ( newElement.c_str() ) );
  }

protected:
  
  // release the memory used by the elements
  void freeElements ( const int iIndex, const int iSize = 1 )
  { 
    if (pArray)
    {
/*
 * Not supported any more, there might be problems with <F> by arrays
 *       long lSize = SM_length ( pArray );
 */

      for ( int iPos = iIndex; // from index
	      ( iPos < iIndex + iSize ) && ( iPos < iCount ); // until size or count reached
	      iPos++ )
      { // free the memory attached to the element
  		  if ( ( pArray[iPos] != NULL ) && !( ( (void *)(pArray[iPos]) > pArray )
  			/* && ( (void *)(pArray[iPos]) < pArray + lSize )*/ ) )
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
	ITKArray<char *>::Resize( nNewSize );
  }
};

}

#endif /* XFY_ARRAY_ITK_H_ */
