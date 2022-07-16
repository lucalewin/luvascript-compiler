#include <parsing/header.h>

#include <stdio.h>
#include <string.h>

#include <lexing/token.h>
#include <util/util.h>
#include <util/arraylist.h>
#include <parsing/nodes/package.h>

#include <util/logging/logger.h>

ArrayList *header_parse(ArrayList *tokens)
{
	Token *current;

	ArrayList *package_templates = arraylist_create();

	for (size_t i = 0; i < tokens->size; i++)
	{
		current = arraylist_get(tokens, i);
		if (current->type != TOKEN_KEYWORD && strcmp(current->data, "package") == 0)
		{
			printf("%s:%d:%d: " RED "error: " RESET "expected 'package' keyword\n", "unknown", current->line, current->pos);
			for (size_t j = 0; j < package_templates->size; j++)
			{
				struct package_template *package_template = arraylist_get(package_templates, j);
				package_template_free(package_template);
			}
			arraylist_free(package_templates);
			return NULL;
		}

		current = arraylist_get(tokens, ++i);
		if (current->type != TOKEN_IDENTIFIER)
		{
			printf("%s:%d:%d: " RED "error: " RESET "expected package name\n", "unknown", current->line, current->pos);
			for (size_t j = 0; j < package_templates->size; j++)
			{
				struct package_template *package_template = arraylist_get(package_templates, j);
				package_template_free(package_template);
			}
			arraylist_free(package_templates);
			return NULL;
		}

		struct package_template *pkg_tmpl = calloc(1, sizeof(struct package_template));
		pkg_tmpl->name = strdup(current->data);

		current = arraylist_get(tokens, ++i);

		while (!(current->type == TOKEN_KEYWORD && strcmp(current->data, "package") == 0))
		{
			if (current->type == TOKEN_KEYWORD && strcmp(current->data, "function") == 0) // function
			{

			}
			else if (current->type == TOKEN_KEYWORD && strcmp(current->data, "extern") == 0) // extern function
			{

			}
			else
			{
				printf("global variables in static libraries are not yet supported!\n");
				for (size_t j = 0; j < package_templates->size; j++)
				{
					struct package_template *package_template = arraylist_get(package_templates, j);
					package_template_free(package_template);
				}
				arraylist_free(package_templates);
			}
		}
	}

	return package_templates;
}