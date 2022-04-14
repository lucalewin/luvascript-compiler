#ifndef __LUVA_IMPORT_H__
#define __LUVA_IMPORT_H__

#include <util/arraylist.h>

typedef struct _ImportDeclaration ImportDeclaration;

/**
 * @brief 
 * 
 */
struct _ImportDeclaration {
	ArrayList *package_names; 		// ArrayList<String>
	ArrayList *type_identifiers; 	// ArrayList<String>
};

/**
 * @brief 
 * 
 * @return ImportDeclaration* 
 */
ImportDeclaration *import_declaration_new();

/**
 * @brief 
 * 
 * @param import_declaration 
 * @return ImportDeclaration* 
 */
ImportDeclaration *import_declaration_copy(ImportDeclaration *import_declaration);

/**
 * @brief 
 * 
 * @param import_declarations 
 * @return ArrayList* 
 */
ArrayList *compact_import_declarations(ArrayList *import_declarations);

/**
 * @brief 
 * 
 * @param import_declaration 
 */
void import_declaration_free(ImportDeclaration *import_declaration);

#endif // __LUVA_IMPORT_H__