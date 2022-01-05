#ifndef __LUVA_IMPORT_H__
#define __LUVA_IMPORT_H__

typedef struct {
	char *package_name;
	char *type_identifier;
} import_stmt_t;

void import_stmt_free(import_stmt_t *stmt);

#endif // __LUVA_IMPORT_H__