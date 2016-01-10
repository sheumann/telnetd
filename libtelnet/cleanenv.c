/*-
 * Copyright (c) 2016 Stephen Heumann
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdlib.h>
#include <string.h>

extern char **environ;

/*
 * UNSETENV_REORDERS_ENVIRON must be set to 1 if unsetenv() may
 * change entries in environ _prior to_ the one being removed.
 * The implementation in GNO 2.0.6's libc doesn't do this,
 * which allows us to optimize things a bit.
 */
#ifdef __GNO__
# define UNSETENV_REORDERS_ENVIRON 0
#else
# define UNSETENV_REORDERS_ENVIRON 1
#endif

/*
 * Remove all environment entries except those indicated by retain,
 * which is a null-terminated list of environment variable names.
 */
int cleanenv(const char *const retain[])
{
	int index = 0;
	const char *entry;
	const char *const *keep;
	while ((entry = environ[index]) != NULL) {
		char *equals = strchr(entry, '=');
		char *name;
		
		if (equals) {
			int match = 0;
			size_t len = equals - entry + 1;
			name = malloc(len);
			if (name == NULL)
				return -1;
			strncpy(name, entry, len-1);
			name[len-1] = 0;
			for (keep = retain; *keep != NULL; keep++) {
				if (strcmp(*keep, name) == 0) {
					match = 1;
					break;
				}
			}
			if (match) {
				index++;
				free(name);
				continue;
			}
		} else {
			name = strdup(entry);
			if (name == NULL)
				return -1;
		}
		
		unsetenv(name);
		free(name);
#if UNSETENV_REORDERS_ENVIRON
		index = 0;
#endif
	}

	return 0;
}


#ifdef DEBUG_CLEANENV

#include <stdio.h>

void dump_env(const char *s)
{
	int index;
	const char *entry;
	
	puts(s);
	for (index = 0; (entry = environ[index]) != NULL; index++) {
		puts(entry);
	}
}

int main(void)
{
	const char *retain[] = {"TERM", "USER", "PATH", "FOO", NULL};
	
	dump_env("Before:");
	cleanenv(retain);
	dump_env("\nAfter:");
}

#endif
