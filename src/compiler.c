#include <compiler.h>

#include <stdbool.h>

#include <options.h>
#include <util/util.h>
#include <util/cmd.h>
#include <util/file.h>
#include <logging/logger.h>

void compile_to_object_file(const char *asm_filename, const char *obj_filename);
void link_to_shared_library(const char *obj_filename, CommandlineOptions *options);
void link_to_static_library(const char *obj_filename, CommandlineOptions *options);
void link_to_binary_executable(const char *obj_filename, CommandlineOptions *options);

void compile(const char *assembly_code, CommandlineOptions *options)
{
	// create temporary file
	char *asm_filename = stradd(options->output_file_name, ".asm");
	
	// write assembly code to file
	if (file_write(asm_filename, assembly_code) == false)
	{
		log_error("could not write assembly code to file\n");
		free(asm_filename);
		return;
	}

	char *obj_filename = NULL;
	
	if (options->output_file_name != NULL) {
		obj_filename = strdup(options->output_file_name);
	} else {
		stradd(options->output_file_name, ".o");
	}
	

	compile_to_object_file(asm_filename, obj_filename);
	
	if (options->link)
	{
		if (options->is_shared_library)
		{
			link_to_shared_library(obj_filename, options);
		}
		else if (options->is_static_library)
		{
			link_to_static_library(obj_filename, options);
		}
		else
		{
			link_to_binary_executable(obj_filename, options);
		}
	}

	// check if we should delete the temporary file
	if (!options->generate_assembly)
	{
		// remove temporary file
		if (file_remove(asm_filename) == false)
		{
			log_error("could not delete temporary file\n");
		}
	}

	free(asm_filename);
	free(obj_filename);
}

void compile_to_object_file(const char *in_filename, const char *out_filename)
{
	const char *assembler = "nasm";
	const char *assembler_flags = "-f elf64";

	exec(assembler, assembler_flags, in_filename, "-o", out_filename, NULL);
}

void link_to_shared_library(const char *filename, CommandlineOptions *options)
{
	const char *linker = "ld";
	const char *linker_flags = "-shared";

	exec(linker, linker_flags, filename, "-o", options->output_file_name, NULL);
}

void link_to_static_library(const char *filename, CommandlineOptions *options) {
	const char *archiver = "ar";
	const char *archiver_flags = "rcs";

	// link to static library
	exec(archiver, archiver_flags, filename, "-o", options->output_file_name, NULL);
}

void link_to_binary_executable(const char *obj_filename, CommandlineOptions *options)
{
	const char *linker = "ld";
	const char *linker_flags = "-g";

	exec(linker, linker_flags, obj_filename, "-o", options->output_file_name, NULL);
}
