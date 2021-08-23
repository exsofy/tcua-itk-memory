#ifndef XFY_POM_HELPERS_H_
#define XFY_POM_HELPERS_H_

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

XFY_API int XFY_POM_is_object_a ( tag_t tObject, const char *pszClassName, logical *isA, tag_t *tClassID = NULL );

XFY_API int XFY_POM_is_object_a ( tag_t tObject, tag_t tClassID, logical *isA );

XFY_API int XFY_POM_dump_object ( const char *name, tag_t tObject );

XFY_API int XFY_dump_error_stack ( const char *file, int line );

#ifdef XFY_API
#undef XFY_API
#endif


#endif /* XFY_POM_HELPERS_H_ */
