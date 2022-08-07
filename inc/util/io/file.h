#ifndef LUVA_FILE_H
#define LUVA_FILE_H

#ifndef _DEFAULT_SOURCE
    // This is required the `realpath` function
    // used in `to_absolute_path`
	#define _DEFAULT_SOURCE
#endif

#include <util/arraylist.h>

char *read_file(const char* path);
int file_write(const char* path, const char *text);
int file_exists(const char* path);
int file_remove(const char* path);
ArrayList *list_files(const char *dir);
ArrayList *list_dirs(const char *directory);
const char *get_filename_extension(const char *filename);
char *to_absolute_path(const char *path);
char *get_absolute_dirname_from_file(const char *file);
char *path_combine(const char *path, const char *file);

#endif // LUVA_FILE_H