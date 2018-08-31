#ifndef XFY_ITK_STRING_H
#define XFY_ITK_STRING_H

#ifdef XFYUSELIB
#ifdef XFYLIB
#define XFY_API __declspec(dllexport)
#else
#define XFY_API __declspec(dllimport)
#endif
#else
#define XFY_API
#endif

namespace XFY {

// copy a string into a new (persistent) memory
XFY_API char * XFY_ITK_string_copy( const char* orig );
// copy first n characters of string into a new (persistent) memory
XFY_API char * XFY_ITK_string_ncopy( const char* orig, unsigned int count );
// append second string to the first string with memory reallocation
XFY_API void XFY_ITK_string_append( char* &orig, const char *append );
// append n characters of the second string to the first string with memory reallocation
XFY_API void XFY_ITK_string_nappend( char* &orig, const char *append, unsigned int count  );
// encode original string for XML
XFY_API char * XFY_ITK_string_xml_encode ( const char* orig );


class XFY_API ITKString
{
  char *m_pszValue;

public:
  ITKString() { m_pszValue = NULL; } // empty constructor
  ITKString( const ITKString &cisString ) { m_pszValue = NULL; operator= ((const char *)cisString); }; // copy constructor
  //XFYITKString( const char *String ) { m_pszValue = SM_string_copy_persistent ( String ); }  // string constructor
  // Deactivated for update TCEng2005
  ITKString( const char *String ) { m_pszValue = ( String != NULL ? XFY_ITK_string_copy ( String ) : NULL ); }  // string constructor
  ITKString( const char *String, const int iLen )  // Substring constructor
  {
	m_pszValue = ( iLen >= 0 ) ? XFY_ITK_string_ncopy ( String, iLen )
                               : XFY_ITK_string_copy ( String );
  }
  
  ~ITKString() { if (m_pszValue) MEM_free(m_pszValue); }; // destructor with memory free

  char** operator &() { return &m_pszValue; };
  operator const char*() const { return m_pszValue; };
  const char* c_str() const { return m_pszValue; };
  char operator [](const int iIndex) { return m_pszValue[iIndex]; };

  const ITKString& operator = (const char *String )
  {
    if ( m_pszValue != NULL ) MEM_free ( m_pszValue );
	m_pszValue = ( String != NULL ? XFY_ITK_string_copy ( String ) : NULL );

    return *this;
  };

  const ITKString& operator = (const ITKString& String) { return operator= ( (const char *)String ); };

  const ITKString& operator += (const char *String )
  {
    if ( String != NULL )
    {
	  if ( m_pszValue == NULL ) m_pszValue = XFY_ITK_string_copy ( String );
      else                      XFY_ITK_string_append ( m_pszValue, String );
    }
  return *this;
  };

  void append ( const char *String, const ptrdiff_t Count = -1 )
  {
    if ( String == NULL ) return;
    if ( Count < 0 )
    {
      operator+=(String); return;
    };
	if ( m_pszValue == NULL ) m_pszValue = XFY_ITK_string_ncopy ( String, (int)Count );
    else                      XFY_ITK_string_nappend ( m_pszValue, String, (int)Count );
  };

  char *detach () { char *pszRet = m_pszValue; m_pszValue = NULL; return pszRet; };

  void attach ( char *String )
  {
    if ( m_pszValue != NULL ) MEM_free ( m_pszValue );
    m_pszValue = String;
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
