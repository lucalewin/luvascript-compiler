#include <options.h>

#include <string.h>

#include <util/util.h>
#include <logging/logger.h>

// default filename of the generated binary file if no output filename is specified
#define DEFAULT_BINARY_NAME "a.out"
#define VERSION "0.3.0-alpha-1"

void print_help();

/**
 * @brief parses the command line arguments
 * 
 * @param argc the number of arguments
 * @param argv the arguments
 * @return CommandlineOptions* 
 */
CommandlineOptions *parse_commandline_arguments(int argc, char *argv[]) {
	CommandlineOptions *options = calloc(1, sizeof(CommandlineOptions));

	// assign default values to options
	options->input_file_name = NULL;
	options->source_files = arraylist_create();
	options->output_file_name = NULL;
	options->library_paths = arraylist_create();
	options->link = 1; // true
	options->generate_assembly = 0; // false
	options->is_library = 0; // false

	// parse command line arguments
	for (size_t i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			// parse command line arguments
			if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
				print_help();
				options_free(options);
				exit(0);
			} else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
				printf("lvc version %s\n", VERSION);
				exit(0);
			} else if (strcmp(argv[i], "-o") == 0) {
				// output file name
				if (i + 1 < argc) {
					options->output_file_name = argv[i + 1];
					i++;
				} else {
					log_error("no output file specified\n");
					log_error("type 'lvc -h' for help\n");
					options_free(options);
					exit(1);
				}
			} else if (strcmp(argv[i], "-S") == 0) {
				// generate assembly
				options->generate_assembly = 1;
			} else if (strcmp(argv[i], "-c") == 0) {
				// compile to object file
				options->link = 0;
			} else if (strcmp(argv[i], "-shared") == 0) {
				// compile to shared library
				options->is_library = 1;
			} else {
				log_error("unknown option '%s'\n", argv[i]);
				log_error("type 'lvc -h' for help\n");
				options_free(options);
				exit(1);
			}
		} else {
			// input file name
			// options->input_file_name = argv[i];
			arraylist_add(options->source_files, argv[i]);
		}
	}

	if (options->output_file_name == NULL) {
		options->output_file_name = strdup(DEFAULT_BINARY_NAME);
	}

	return options;
}

/**
 * @brief 
 * TODO: add help for compiler flags
 */
void print_help() {
	printf(YELLOW "usage" RESET ": lvc <inputfile>\n");
}