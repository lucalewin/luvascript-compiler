#include <compiler.h>

#include <options.h>
#include <util/util.h>
#include <util/cmd.h>

char *assembler = "nasm";
char *file_format = "-f elf64";
char *linker = "ld";

void compile(const char *filename, CommandlineOptions *options) {
	// compile asm file
	if (options->link) {
		// `-c` compiler flag is specified
		char *obj_file = straddall(strdup(filename), ".o", NULL);
		// compile to object file
		exec(assembler, file_format, filename, "-o", obj_file, NULL);
		// link to binary file
		char *linker_flags = "-g";

		if (options->is_library == 1) {
			// `-shared` linker flag is specified
			linker_flags = stradd(linker_flags, " -shared");
		}

		exec(linker, linker_flags, obj_file, "-o", options->output_file_name, NULL);
		// remove generated assembly file
		if (!options->generate_assembly) {
			exec("rm", filename, NULL);
		}

		// remove object file
		exec("rm", obj_file, NULL);

		// free allocated memory for char*
		free(obj_file);
	} else {
		// compile to object file
		exec(assembler, file_format, filename, "-o", options->output_file_name, NULL);

		// remove generated assembly file
		if (!options->generate_assembly) {
			exec("rm", filename, NULL);
		}
	}
}
