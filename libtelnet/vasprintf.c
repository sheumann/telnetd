/*-
 * Copyright (c) 2006 Verdens Gang AS
 * Copyright (c) 2006-2007 Linpro AS
 * All rights reserved.
 *
 * Author: Dag-Erling Smørgrav <des@linpro.no>
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
 *
 * $Id$
 *
 */

#ifdef __GNO__

#define va_copy(a,b) memcpy((&a),(&b),sizeof(b))

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
vasprintf(char **strp, const char *fmt, va_list ap)
{
	va_list aq;
	int ret;
	char c;

	va_copy(aq, ap);
	ret = vsnprintf(&c, 1, fmt, ap);
	/* va_end(ap); -- Don't do this on ORCA/C; it messes up the stack */
	if ((*strp = malloc(ret + 1)) == NULL)
		return (-1);
	ret = vsnprintf(*strp, ret + 1, fmt, aq);
	return (ret);
}

#endif
