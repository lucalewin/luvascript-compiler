# Luva Compiler Flags

		-o [filename]			specify output filename (default=a.out)
		-S						generate assembly code
		-c						disable linking
		-shared					generate shared library
		-static					generate static library
		--modules:__,__,..		specify (global or local installed) modules to link
		--src-root [dir]		specify source root directory
		-nostdlib				disable standard library