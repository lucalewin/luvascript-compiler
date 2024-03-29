#include <util/io/file.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <libgen.h>
#include <dirent.h>

#include <util/arraylist.h>
#include <util/logging/logger.h>

char *read_file(const char* path) {
    FILE *file = fopen(path, "r");
    if (!file) {
        log_error("could not open file '%s'\n", path);
        return NULL;
    }

    // get file size
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // read file
    char *buffer = (char*) malloc(sizeof(char) * (size + 1));
	// ignore return value of fread
    if (fread(buffer, 1, size, file));
    buffer[size] = '\0';
    fclose(file);

    return buffer;
}

int file_write(const char* path, const char *text) {
    FILE *file = fopen(path, "w");
    if (!file) {
        // log_error("write_file(): could not open file '%s'\n", path);
        return 0;
    }
    fputs(text, file);
    fclose(file);
	return 1;
}

int file_exists(const char *path) {
	FILE *file = fopen(path, "r");
	if (file) {
		fclose(file);
		return 1;
	}
	return 0;
}

int file_remove(const char* path)
{
	if (remove(path) != 0)
	{
		log_error("could not remove file '%s'\n", path);
		return 0;
	}
	return 1;
}

// list all files in the current directory
ArrayList *list_files(const char *directory) {
	ArrayList *files = arraylist_create();
	DIR *dir = opendir(directory);
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_type == 0x08) { // _A_VOLID = 0x08 --> file
			arraylist_add(files, entry->d_name);
		}
	}
	closedir(dir);
	return files;
}

ArrayList *list_dirs(const char *directory) {
	ArrayList *dirs = arraylist_create();
	DIR *dir = opendir(directory);
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_type == 0x04) { // directory
			arraylist_add(dirs, entry->d_name);
		}
	}
	closedir(dir);
	return dirs;
}

const char *get_filename_extension(const char *filename) {
	const char *dot = strrchr(filename, '.');
	if(!dot || dot == filename) return "";
	return dot + 1;
}

char *to_absolute_path(const char *path) {
	return realpath(path, NULL);
}

char *get_absolute_dirname_from_file(const char *file) {
	return dirname(to_absolute_path(file));
}

char *path_combine(const char *path, const char *file) {
	size_t len = strlen(path) + strlen(file) + 2;
	char *combined = (char*) malloc(sizeof(char) * len);
	strcpy(combined, path);
	strcat(combined, "/");
	strcat(combined, file);
	return combined;
}
