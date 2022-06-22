#include <stdio.h>
#include <stddef.h>

//#include <tc/tc.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/tctype.h>

#include "../trace/xfy_trace_itk.h"

#include "xfy_pom_helpers.h"

int XFY_POM_is_object_a ( tag_t tObject, const char *pszClassName, logical *isA, tag_t *ptClassID )
{
  XFY_TFCE_P4 ( tObject, I, pszClassName, I, isA, O, ptClassID, O );

  tag_t tClassID;

  if ( ptClassID == NULL || *ptClassID == NULLTAG ) {
	  POM_class_id_of_class ( pszClassName, &tClassID );
	  if ( ptClassID != NULL ) *ptClassID = tClassID;
  }
  else {
	  tClassID = *ptClassID;
  }

  XFY_TRET ( XFY_POM_is_object_a ( tObject, tClassID, isA ) );
}

int XFY_POM_is_object_a ( tag_t tObject, tag_t tClassID, logical *isA )
{
  XFY_TFCE_P3 ( tObject, I, tClassID, I, isA, O );

  tag_t tObjectClass;
  XFY_CALL ( POM_class_of_instance ( tObject, &tObjectClass ) );

  XFY_TRET ( POM_is_descendant ( tClassID, tObjectClass, isA ) );
}

int XFY_AOM_dump_object(const char *name, tag_t tObject)
{
	XFY_TFCE_P1(tObject, I);

	if (tObject == NULLTAG) {
		printf("%s: %d\n", name, tObject);
		XFY_TRET(ITK_ok);
	}

	int propCount;
	char **propNames;
	AOM_ask_prop_names(tObject, &propCount, &propNames);

	for (int iProp = 0; iProp < propCount; iProp++) {
		int numValues;
		char **values;
		AOM_ask_displayable_values(tObject, propNames[iProp], &numValues, &values);

		printf("\t%s: %s\n", propNames[iProp], numValues > 0 ? values[0] : "");

		MEM_free(values);
		MEM_free(propNames[iProp]);
	}

	MEM_free(propNames);

	XFY_TRET_OK;
}

int XFY_POM_dump_object ( const char *name, tag_t tObject )
{
	XFY_TFCE_P1 ( tObject, I );

	if ( tObject == NULLTAG ) {
		printf ( "%s: %d\n", name, tObject );
		XFY_TRET ( ITK_ok );
	}

	tag_t tClass;
	XFY_CALL ( POM_class_of_instance ( tObject, &tClass ) );
	
	char *pszUID = NULL;
	XFY_CALL ( POM_tag_to_uid ( tObject, &pszUID ) );

	logical hasName = false;
	char *pszClassName = NULL;
	tag_t tNameAttrID = NULLTAG;
	XFY_CALL ( POM_name_of_class ( tClass, &pszClassName ) );
	printf ( "%s: %d/%x<%s> Class:%s", name, tObject, tObject, pszUID, pszClassName );
	XFY_CALL ( POM_attr_exists ( "object_name", pszClassName, &hasName ) );
	if ( hasName ) {
		XFY_CALL ( POM_attr_id_of_attr ( "object_name", pszClassName, &tNameAttrID ));
	} 
	else {
		XFY_CALL ( POM_attr_exists ( "name", pszClassName, &hasName ) );
		if ( hasName ) {
			XFY_CALL ( POM_attr_id_of_attr ( "name", pszClassName, &tNameAttrID ));
		}
		else {
			XFY_CALL ( POM_attr_exists ( "Name", pszClassName, &hasName ) );
			if ( hasName ) {
				XFY_CALL ( POM_attr_id_of_attr ( "Name", pszClassName, &tNameAttrID ));
			}
		}
	}
	MEM_free ( pszClassName );
	MEM_free ( pszUID );

	tag_t s_tWorkspaceObjectClass = NULLTAG;
	if ( s_tWorkspaceObjectClass == NULLTAG ) {
		XFY_CALL ( POM_class_id_of_class ( "WorkspaceObject", &s_tWorkspaceObjectClass ) );
	}


	logical isNew, isLoaded, isCached, isModifiable;
	XFY_CALL ( POM_is_newly_created ( tObject, &isNew ) );
	XFY_CALL ( POM_is_loaded ( tObject, &isLoaded ) );
	XFY_CALL ( POM_is_cached ( tObject, &isCached ) );
	XFY_CALL ( POM_modifiable ( tObject, &isModifiable ) );

	logical isWSO = false;
	if ( isLoaded ) {
		XFY_CALL ( POM_is_descendant ( s_tWorkspaceObjectClass, tClass, &isWSO ) );
		if ( isWSO ) {
			static tag_t s_tTypeAttrID = NULLTAG;
			if ( s_tTypeAttrID == NULLTAG ) {
				XFY_CALL ( POM_attr_id_of_attr ( "object_type", "WorkspaceObject", &s_tTypeAttrID ));
			}
			char * pszType = NULL;
			logical isNull, isEmpty;
			XFY_CALL ( POM_ask_attr_string ( tObject, s_tTypeAttrID, &pszType, &isNull, &isEmpty ) );
			printf ( "\tType:%s", pszType );
			MEM_free ( pszType );
		}
	}

	printf ( "\n" );

	printf ( "\t%s new\t\t%s loaded\t%s cached\t%s modifiable\n",
			isNew ? "Is" : "Not", isLoaded ? "Is" : "Not",
			isCached ? "Is" : "Not", isModifiable ? "Is" : "Not" );

	if ( isLoaded ) {

		tag_t s_tItemClass = NULLTAG;
		if ( s_tItemClass == NULLTAG ) {
			XFY_CALL ( POM_class_id_of_class ( "Item", &s_tItemClass ) );
		}

		tag_t s_tItemRevisionClass = NULLTAG;
		if ( s_tItemRevisionClass == NULLTAG ) {
			XFY_CALL ( POM_class_id_of_class ( "ItemRevision", &s_tItemRevisionClass ) );
		}

		tag_t tItem = NULLTAG;
		logical isItemLoaded = false;
		tag_t tRevision = NULLTAG;

		logical isRevision = false;
		XFY_CALL ( POM_is_descendant ( s_tItemRevisionClass, tClass, &isRevision ) );
		if ( isRevision ) {
			tRevision = tObject;
			static tag_t s_tItemRevItemAttrID = NULLTAG;
			if ( s_tItemRevItemAttrID == NULLTAG ) {
				XFY_CALL ( POM_attr_id_of_attr ( "items", "ItemRevision", &s_tItemRevItemAttrID ));
			}
			logical isNull, isEmpty;
			XFY_CALL ( POM_ask_attr_tag ( tObject, s_tItemRevItemAttrID, &tItem, &isNull, &isEmpty ) );
			XFY_CALL ( POM_is_loaded ( tItem, &isItemLoaded ) );
		}
		else {
			logical isItem = false;
			XFY_CALL ( POM_is_descendant ( s_tItemClass, tClass, &isItem ) );

			if ( isItem ) {
				tItem = tObject;
				isItemLoaded = isLoaded;
			}
		}

		if ( tItem != NULLTAG && isItemLoaded ) {
			static tag_t s_tItemIDAttrID = NULLTAG;
			if ( s_tItemIDAttrID == NULLTAG ) {
				XFY_CALL ( POM_attr_id_of_attr ( "item_id", "Item", &s_tItemIDAttrID ));
			}
			char * pszItemID = NULL;
			logical isNull, isEmpty;
			XFY_CALL ( POM_ask_attr_string ( tObject, s_tItemIDAttrID, &pszItemID, &isNull, &isEmpty ) );
			printf ( "\tItem:%s", pszItemID );
			MEM_free ( pszItemID );
		}

		if ( tRevision != NULLTAG ) {
			static tag_t s_tItemRevIDAttrID = NULLTAG;
			if ( s_tItemRevIDAttrID == NULLTAG ) {
				XFY_CALL ( POM_attr_id_of_attr ( "item_revision_id", "ItemRevision", &s_tItemRevIDAttrID ));
			}
			char * pszItemRevID = NULL;
			logical isNull, isEmpty;
			XFY_CALL ( POM_ask_attr_string ( tObject, s_tItemRevIDAttrID, &pszItemRevID, &isNull, &isEmpty ) );
			printf ( "\tRevision:%s", pszItemRevID );
			MEM_free ( pszItemRevID );
		}
	}

	if ( hasName && tNameAttrID != NULLTAG ) {
		printf ( "\tName:" );
		if ( isLoaded ) {
			char * pszName = NULL;
			logical isNull, isEmpty;
			XFY_CALL ( POM_ask_attr_string ( tObject, tNameAttrID, &pszName, &isNull, &isEmpty ) );
			printf ( "%s\n", pszName );
			MEM_free ( pszName );
		}
		else {
			printf ( "-- not loaded --\n" );
		}
	}

	XFY_TRET_OK;
}

int XFY_dump_error_stack ( const char *file, int line ) {
  
  int            n_ifails;      /**< (O) Number of errors in the error store */
  const int*     severities;    /**< (O) n_ifails #EMH_severity_error, #EMH_severity_warning, or #EMH_severity_information */
  const int*     ifails;        /**< (O) n_ifails Integer code for each error in the store */
  const char**   texts;          /**< (O) n_ifails Internationalized text strings for each error in the store */
  EMH_ask_errors ( &n_ifails, &severities, &ifails, &texts );
  printf ( "TP %s %d\n", file == NULL ? "" : file, line );
  for ( int iFail = 0; iFail < n_ifails; iFail++ ) {
	  printf ( "\t%d %d %d %s\n", iFail, severities[iFail], ifails[iFail], texts[iFail] );
  }

  return ITK_ok;
}
