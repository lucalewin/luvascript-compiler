#ifndef LUVA_FILE_H
#define LUVA_FILE_H

#include <util/arraylist.h>

char *read_file(const char* path);
int file_write(const char* path, const char *text);
int file_exists(const char* path);
int file_remove(const char* path);
ArrayList *list_files(const char *dir);
ArrayList *list_dirs(const char *directory);
const char *get_filename_extension(const char *filename);

#endif // LUVA_FILE_H