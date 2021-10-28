#ifndef LUVA_OPTIONS_H
#define LUVA_OPTIONS_H

typedef struct commandline_options {
	char *input_file_name;
	char *output_file_name;
	unsigned int link : 1;
	unsigned int generate_assembly : 1;
} CommandlineOptions;

CommandlineOptions *parse_commandline_arguments(int argc, char *argv[]);

void options_free(CommandlineOptions *options);

// implementations

void options_free(CommandlineOptions *options) {
	free(options->input_file_name);
	free(options->output_file_name);
	free(options);
}

#endif