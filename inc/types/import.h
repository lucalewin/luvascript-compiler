#ifndef __LUVA_IMPORT_H__
#define __LUVA_IMPORT_H__

#include <util/arraylist.h>
#include <stdbool.h>

typedef struct _ImportDeclaration ImportDeclaration;

/**
 * @brief 
 * 
 */
struct _ImportDeclaration {
	ArrayList *package_names; 		// ArrayList<String>
	char *package_name;				// String
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
 * @return true 
 */
bool import_declaration_should_import_all(ImportDeclaration *import_declaration);

/**
 * @brief 
 * 
 * @param type_ident 
 * @return true  
 */
bool import_declaration_contains(ImportDeclaration *import_declaration, char *type_ident);

/**
 * @brief 
 * 
 * @param import_declaration 
 */
void import_declaration_free(ImportDeclaration *import_declaration);

#endif // __LUVA_IMPORT_H__