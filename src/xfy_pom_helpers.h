#ifndef __XFY_POM_HELPERS_H_
#define __XFY_POM_HELPERS_H_

int XFY_POM_is_object_a ( tag_t tObject, const char *pszClassName, logical *isA, tag_t *tClassID = NULL );

int XFY_POM_is_object_a ( tag_t tObject, tag_t tClassID, logical *isA );

int XFY_POM_dump_object ( const char *name, tag_t tObject );

int XFY_dump_error_stack ( const char *file, int line );

#endif /* __XFY_POM_HELPERS_H_ */