#ifndef XFY_POM_HELPERS_H_
#define XFY_POM_HELPERS_H_

#if defined(XFYUSELIB) || defined (XFYLIB) 
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

#ifdef XFYMODULE
#define XFYNAMESPACESTART namespace XFYns { namespace XFYMODULE {
#define XFYNAMESPACEEND }}
#ifndef XFY
#define XFYNAMESPACE XFYns::XFYMODULE
#endif
#else
#define XFYNAMESPACESTART namespace XFYns {
#define XFYNAMESPACEEND }
#ifndef XFY
#define XFY XFYns
#endif
#endif

XFYNAMESPACESTART

XFY_API int POM_is_object_a ( tag_t tObject, const char *pszClassName, logical *isA, tag_t *tClassID = NULL );

XFY_API int POM_is_object_a ( tag_t tObject, tag_t tClassID, logical *isA );

XFY_API int AOM_dump_object(const char *name, tag_t tObject);

XFY_API int POM_dump_object ( const char *name, tag_t tObject );

XFY_API int dump_error_stack ( const char *file, int line );

XFYNAMESPACEEND

#ifdef XFY_API
#undef XFY_API
#endif

#ifdef 	XFYNAMESPACESTART
#undef 	XFYNAMESPACESTART
#endif

#ifdef 	XFYNAMESPACEEND
#undef 	XFYNAMESPACEEND
#endif

#endif /* XFY_POM_HELPERS_H_ */
