#ifndef LUVA_OPTIONS_H
#define LUVA_OPTIONS_H

#include <util/arraylist.h>

enum flags {
	LINK_SHARED_LIBRARY = 0x0001,
	LINK_STATIC_LIBRARY = 0x0002,
	LINK_BINARY_EXECUTABLE = 0x0004,
	ASSEMBLY_CODE = 0x0008,
};

typedef struct commandline_options {
	char *input_file_name;
	ArrayList *source_files;
	char *output_file_name;
	ArrayList *library_paths;
	unsigned int link : 1;
	unsigned int generate_assembly : 1;
	unsigned int is_shared_library : 1;
	unsigned int is_static_library : 1;
	unsigned int nostdlib : 1;
	long flags;
} CommandlineOptions;

CommandlineOptions *parse_commandline_arguments(int argc, char *argv[]);

void options_free(CommandlineOptions *options);

#endif