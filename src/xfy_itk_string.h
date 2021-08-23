#ifndef XFY_ITK_STRING_H
#define XFY_ITK_STRING_H

#include <base_utils/Mem.h>

#ifdef XFYUSELIB
	#ifdef XFYLIB
		#if defined(__lint)
			#define XFY_API  __export(Avf2foundation)
		#elif defined(_WIN32)
			#define XFY_API __declspec(dllexport)
		#else
			#define XFY_API
		#endif
	#else
		#if defined(__lint)
			#define XFY_API  __export(Avf2foundation)
		#elif defined(_WIN32)
			#define XFY_API __declspec(dllimport)
		#else
			#define XFY_API
		#endif
	#endif
#else
	#define XFY_API
#endif

namespace XFY {

// encode original string for XML
XFY_API char * XFY_ITK_string_xml_encode ( const char* orig );


class XFY_API ITKString
{
  char *m_pszValue;

public:
  ITKString() : m_pszValue (NULL)  {} // empty constructor
  ITKString( const ITKString &cisString ) {
	  m_pszValue = ( cisString.m_pszValue != NULL ? MEM_string_copy ( cisString.m_pszValue ) : NULL );
  }; // copy constructor
  ITKString( const char *String ) {
	  m_pszValue = ( String != NULL ? MEM_string_copy ( String ) : NULL );
  }  // string constructor
  ITKString( const char *String, const int iLen )  // Substring constructor
  {
	if ( String == NULL ) {
		m_pszValue = NULL;
	} else {
		if ( iLen > 0 ) {
			m_pszValue =  MEM_string_ncopy ( String, iLen );
		} else if ( iLen == 0 ) {
			m_pszValue =  MEM_string_copy ( "" );
		} else {
			m_pszValue =  MEM_string_copy ( String );
		}
	}
  }
  
  ~ITKString() { if (m_pszValue) MEM_free(m_pszValue); }; // destructor with memory free

  char** operator &() { return &m_pszValue; };
  operator const char*() const { return m_pszValue; };
  const char* c_str() const { return m_pszValue; };
  char& operator [](const int iIndex) { return m_pszValue[iIndex]; };
  const char& operator [](const int iIndex) const { return m_pszValue[iIndex]; };

  ITKString& operator = (const char *String )
  {
    if ( m_pszValue != NULL ) MEM_free ( m_pszValue );
	m_pszValue = ( String != NULL ? MEM_string_copy ( String ) : NULL );

    return *this;
  };

  ITKString& operator = (const ITKString& String) { return operator= ( String.m_pszValue ); };

  ITKString& operator += (const char *String )
  {
    if ( String == NULL || *String == 0 ) return *this;

	if ( m_pszValue == NULL ) m_pszValue = MEM_string_copy ( String );
	else                      m_pszValue = MEM_string_append( m_pszValue, String );

	return *this;
  };

  ITKString& operator += (const ITKString &String ) { return operator += ( String.m_pszValue ); };

  ITKString& append ( const ITKString &String, const ptrdiff_t Count = -1 ) {
	  return append ( String.c_str(), Count );
  }

  ITKString& append ( const char *String, const ptrdiff_t Count = -1 )
  {
    if ( String == NULL || *String == 0 ) return *this;
    if ( Count < 0 ) {
      return operator+=(String);
    };
	if ( m_pszValue == NULL ) {
		if ( Count == 0 ) {
			m_pszValue = MEM_string_copy ( "" );
		} else {
			m_pszValue = MEM_string_ncopy ( String, (int)Count );
		}
	} else {
		if ( Count > 0  ) {
           m_pszValue = MEM_string_nappend ( m_pszValue, String, (int)Count );
		}
	}
	return *this;
  };

  char *detach () { 
	  char *pszRet = m_pszValue;
	  m_pszValue = NULL;
	  return pszRet;
  };

  ITKString& attach ( char *String )
  {
    if ( m_pszValue != NULL ) MEM_free ( m_pszValue );
    m_pszValue = String;
	return *this;
  };

  ITKString& encodeXML ();

  ITKString& getPreference ( const char *preference );

  bool empty() { return m_pszValue == NULL || *m_pszValue == 0; }
};

}

#ifdef XFY_API
#undef XFY_API
#endif

#endif /* XFY_ITK_STRING_H */
