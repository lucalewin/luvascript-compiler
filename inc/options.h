#ifndef LUVA_OPTIONS_H
#define LUVA_OPTIONS_H

#include <util/arraylist.h>

typedef struct commandline_options {
	char *input_file_name;
	ArrayList *source_files;
	char *output_file_name;
	ArrayList *library_paths;
	unsigned int link : 1;
	unsigned int generate_assembly : 1;
} CommandlineOptions;

CommandlineOptions *parse_commandline_arguments(int argc, char *argv[]);

void options_free(CommandlineOptions *options);

#endif