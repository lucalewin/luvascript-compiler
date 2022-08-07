#include <parsing/nodes/module.h>

#include <stdio.h>
#include <string.h>

#include <lexing/lexer.h>
#include <parsing/header.h>

#include <util/util.h>
#include <util/arraylist.h>
#include <util/io/file.h>
#include <util/logging/logger.h>

#include <parsing/nodes/package.h>

struct module *module_load(const char *path)
{
	struct module *module;
	module = calloc(1, sizeof(struct module));
	module->name = strdup(strrchr(path, '/') + 1);
	// module->package_templates = arraylist_create();
	log_debug("module->name: %s\n", module->name);

	ArrayList *files = list_files(path);

	for (int i = 0; i < files->size; i++)
	{
		char *file = arraylist_get(files, i);
		if (!strendswith(file, ".lvh")) {
			continue;
		}

		printf("loading package template '%s'\n", file);

		char *source_code = read_file(file);

		ArrayList *tokens = tokenize(source_code, file);
		module->package_templates = header_parse(tokens);
	}

	arraylist_free(files);

	return module;
}

void module_free(struct module *module)
{
	if (module == NULL)
		return;
	free(module->name);
	if (module->package_templates != NULL)
	{
		for (size_t i = 0; i < module->package_templates->size; i++)
		{
			struct package_template *package_template = arraylist_get(module->package_templates, i);
			package_template_free(package_template);
		}
		arraylist_free(module->package_templates);
	}
	free(module);
}