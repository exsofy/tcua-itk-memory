#include <string.h>
#include <memory.h>

#include <base_utils/mem.h>
#include <tc/preferences.h>

#include "../trace/xfy_trace_itk.h"
#include "xfy_itk_string.h"

namespace XFY {

char * XFY_ITK_string_copy( const char* orig )
{
  if ( orig == NULL ) return NULL;
  return MEM_string_copy( orig );
}

char * XFY_ITK_string_ncopy( const char* orig, unsigned int count )
{
  if ( orig == NULL ) return NULL;
  size_t iLen = strlen ( orig );
  if ( iLen > count ) iLen = count;
  char *pszResult = (char*)memcpy ( MEM_alloc ( (int)( ( iLen + 1 ) * sizeof ( char ) ) ),
                                    orig,
                                    iLen  * sizeof ( char ) );
  pszResult[iLen] = 0; // terminate string
  return pszResult;
}

void XFY_ITK_string_append( char* &orig, const char *append )
{
  if ( ( append == NULL ) || ( *append == 0 ) )
  { // nothing to do
    return;
  }

  size_t iLen = (orig == NULL ) ? 0 : strlen ( orig );
  size_t iAppend = strlen ( append ) + 1; // includes terminator 0
  orig = (char *)MEM_realloc ( orig, (int)(iLen + iAppend) ); // lengt together + 0
  memcpy ( orig + iLen, append, iAppend * sizeof ( char )  ); // copy append + 0
}

void XFY_ITK_string_nappend( char* &orig, const char *append, unsigned int count  )
{
  if ( ( append == NULL ) || ( *append == 0 ) || ( count <= 0 ) )
  { // nothing to do
    return;
  }

  size_t iLen = strlen ( orig );
  size_t iAppend = strlen ( append );
  if ( iAppend > count ) iAppend = count;
  orig = (char *)MEM_realloc ( orig, (int)(iLen + iAppend + 1) ); // lengt together + 0
  memcpy ( orig + iLen, append, iAppend * sizeof (char) ); // copy append
  orig[iLen + iAppend] = 0; // terminate string
}


ITKString& ITKString::getPreference ( const char *preference )
{
  XFY_TFCE_NAME( ITKString::getPreference );
  XFY_TPAR_1( preference, I );

  this->operator =(NULL); // reset the value

  XFY_TCALL_V( PREF_ask_char_value(preference, 0, &(*this)), *this );

  return *this;
};

ITKString& ITKString::encodeXML ()
{
  XFY_TFCE_NAME( ITKString::encodeXML );
  XFY_TPAR_0();

  char *pszEncoded = XFY_ITK_string_xml_encode ( (const char*)this->m_pszValue );

  this->operator = (pszEncoded); // replace the value  return *this;

  return *this;
};

char * XFY_ITK_string_xml_encode ( const char* orig )
{
  if ( orig == NULL )
    return NULL;

  size_t newLen = 0;
  
  const char *pszRun = orig;

  while ( *pszRun != 0 )
  {
    switch ( *pszRun++ ) 
    {
      case '&' : newLen += 5; break;
      case '<' : newLen += 4; break;
      case '>' : newLen += 4; break;
      case '\'' : newLen += 6; break;
      case '"' : newLen += 6; break;
      default : newLen++; break;
    }
  }
  
  char *pszResult;

  if ( newLen == ( pszRun - orig ) )
  {
    pszResult = XFY_ITK_string_copy ( orig );
  }
  else
  {
    // new space
    pszResult = (char *)MEM_alloc ( (int)( ( newLen + 1 ) * sizeof ( char ) ) );
    char *pszResRun = pszResult;

    // second run
    pszRun = orig;

    while ( *pszRun != 0 )
    {
      switch ( *pszRun ) 
      {
        case '&'  : memcpy ( pszResRun, "&amp;", 5 );  pszResRun += 5; break;
        case '<'  : memcpy ( pszResRun, "&lt;", 4 );   pszResRun += 4; break;
        case '>'  : memcpy ( pszResRun, "&gt;", 4 );   pszResRun += 4; break;
        case '\'' : memcpy ( pszResRun, "&apos;", 6 ); pszResRun += 6; break;
        case '"'  : memcpy ( pszResRun, "&quot;", 6 ); pszResRun += 6; break;
        default   : *pszResRun = *pszRun;              pszResRun++; break;
      }
      pszRun++;
    }

    *pszResRun = 0; // EOS
  }

  XFY_TMSG ( "XML Encode %s to %s (%d)\n", orig, pszResult, newLen );

  return pszResult;
}

}
