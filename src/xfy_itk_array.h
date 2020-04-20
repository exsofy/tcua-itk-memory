#ifndef XFY_ITK_ARRAY_H_
#define XFY_ITK_ARRAY_H_

#include <string.h>

#include <common/tc_ver_info.h>

#include "xfy_itk_string.h"

namespace XFY {

template < class T > class ITKArray {
protected:
	T* pArray;
	int iCount;
	int iCapacity;            // reserved capacity
	void* pCapacityReference; // pointer for which is the capacity valid
	int iGrowBy;              // capacity increment
public:
  ITKArray( const T* pa, const int pc) { pCapacityReference = pArray = NULL; iCapacity = 0; iGrowBy = 32; iCount = 0; insert ( pa, pc ); }
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
			Resize(nNewSize);
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
			Resize(iCount + iLen);
		}
    for ( int iI = 0; iI < iLen; iI++ )
    {
			pArray[iCount + iI] = newElement[iI];
		}
		iCount += iLen;
		return iCount;
	}

	/* Add iLen copies of the element */
  int insert( const T newElement, const int iLen )
  {
    if ( ( iCount + iLen >= iCapacity ) || ( pCapacityReference != pArray ) )
    {
			Resize(iCount + iLen);
		}

    for ( int iI = 0; iI < iLen; iI++ )
    {
			pArray[iCount + iI] = newElement;
		}

		iCount += iLen;
		return iCount;
	}

  void erase( const int iIndex, const int iSize = 1 )
    { if ( iCount - ( iIndex + iSize ) > 0 )
			memmove((unsigned char *) pArray + iIndex * sizeof(T),
					(unsigned char *) pArray + (iIndex + iSize) * sizeof(T),
					(iCount - (iIndex + iSize)) * sizeof(T));
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
			MEM_free(pArray);
			pCapacityReference = pArray = NULL;
		}
	}

};

typedef ITKArray<tag_t> ITKTagArray;
typedef ITKArray<int> ITKIntArray;
typedef ITKArray<logical> ITKLogicalArray;

#ifdef GRM_H
typedef ITKArray<GRM_relation_t> ITKRelationArray;
#endif

#ifdef ITEM_H
typedef ITKArray<ITEM_attached_object_t> ITKITEMAttachedArray;
#endif

#if TC_MAJOR_VERSION < 12000
#ifdef WORKSPACEOBJECT_H
typedef ITKArray<WSO_status_t> ITKWSOStatusArray;
#endif
#endif

#ifdef AE_H
typedef ITKArray<AE_reference_type_t> ITKAEReferenceTypeArray;
#endif

class ITKStringArray : public ITKArray<char *> {
public:
	ITKStringArray(char* const * pa, const int pc) : isPacked(false), ITKArray<char *>(pa, pc) {};
	ITKStringArray() : isPacked(false), ITKArray<char *>() {};
	ITKStringArray(int Size, int Grow = 32) : isPacked(false), ITKArray<char *>(Size, Grow) {};

	operator const char**() const { return (const char**) pArray; };
	operator char**() const { return pArray; };

	~ITKStringArray() { if (pArray) { freeElements(0, iCount); MEM_free(pArray);pArray = NULL; } };

	// get size pointer to packed array - in one  block allocated/freed
	int *sizePackedPtr() { isPacked = true; return &iCount; }

	void erase(const int iIndex, const int iSize = 1) {
		freeElements(iIndex, iSize);
		ITKArray<char *>::erase(iIndex, iSize);
	}

	void erase() {
		freeElements(0, iCount);
		ITKArray<char *>::erase();
	}

	void resize( int nNewSize, int nGrowBy = -1 ) {
		if (isPacked) splitMemory();
		ITKArray<char *>::resize( nNewSize, nGrowBy );
	}

  int insert(const char * const &newElement) {
		// add copy
		if (isPacked) splitMemory();
		return ITKArray<char *>::push_back(MEM_string_copy(newElement));
	}

  int push_back(const XFY::ITKString &newElement) {
		// add copy
		if (isPacked) splitMemory();
		return ITKArray<char *>::push_back(MEM_string_copy(newElement.c_str()));
	}

protected:

	// release the memory used by the elements
	void freeElements(const int iIndex, const int iSize = 1) {
		if (isPacked) return; // in block - no elements release

		if (pArray) {

			if ( (char**)(pArray[0]) == pArray + iCount ) {
				// high probably a block allocated memory
				TC_write_syslog ( "ITKStringArray unallocated memory base: %p strings: %d [0]: %p %.40s", pArray, iCount, pArray[0], pArray[0] );
				return;
			}

			for ( int iPos = iIndex; // from index
			  	  (iPos < iIndex + iSize) && (iPos < iCount); // until size or count reached
				  iPos++ ) {
				// free the memory attached to the element
  				 if ( pArray[iPos] != NULL ) 
		  			/* && ( (void *)(pArray[iPos]) < pArray + lSize )*/ 
		        { 
					// not null and not inside of the array
					MEM_free(pArray[iPos]);
				}
				pArray[iPos] = NULL;
			}
		}
	}

  void Resize ( int nNewSize = -1 )
  {
    if ( nNewSize == 0 )
	{ // set size to 0, removes all elemets
			freeElements(0, iCount);
		}
		ITKArray<char *>::Resize(nNewSize);
	}

	// memory is allocated in one block
	bool isPacked;

	// divide memory into separated strings (de-block)
	void splitMemory() {
		if (pArray == NULL || !isPacked || iCount <= 0)
			return;

		char **pNewArray = (char **) MEM_alloc(iCount * sizeof(char *));

		// create new allocated entry for each string in array
		for (int iElement = 0; iElement < iCount; iElement++) {
			pNewArray[iElement] = MEM_string_copy(pArray[iElement]);
		}
		MEM_free(pArray);

		isPacked = false;
		iCapacity = iCount;
		pCapacityReference = pArray = pNewArray;
	}
  };
} /* Namespace XFY */

#endif /* XFY_ARRAY_ITK_H_ */
