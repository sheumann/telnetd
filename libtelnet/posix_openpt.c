/*-
 * Copyright (c) 2015 Stephen Heumann
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

#ifdef __GNO__

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

/* Check if a given fd is the master side of a pty */
static int is_pty(int fildes)
{
	char *name;
	static const char pty_prefix[] = ".ptyq";
	
	name = ttyname(fildes);
	if (strncmp(pty_prefix, name, strlen(pty_prefix)) == 0)
		return 1;

	return 0;
}

/* Open the master size of a pty */
int posix_openpt(int oflag)
{
	char ptyname[7];
	int i;
	int master_fd, saved_errno;
	
	saved_errno = errno;
	oflag &= O_RDWR | O_NOCTTY;
	for(i = 0; i < 16; i++) {
		snprintf(ptyname, sizeof(ptyname), ".ptyq%x", i);
		master_fd = open(ptyname, oflag);
		if (master_fd > 0)
			break;
	}
	
	if (master_fd < 1) {
		if (errno != EMFILE)
			errno = EAGAIN;
		return -1;
	}
	
	errno = saved_errno;
	return master_fd;
}

/* Grant access to slave side of pty -- no-op on GNO */
int grantpt(int fildes)
{
	if (!is_pty(fildes)) {
		errno = EINVAL;
		return -1;
	}
	
	return 0;
}

/* Unlock a pty -- no-op on GNO */
int unlockpt(int fildes)
{
	if (!is_pty(fildes)) {
		errno = EINVAL;
		return -1;
	}
	
	return 0;
}

static char namebuf[7]; 

char *ptsname(int fildes)
{
	if (!is_pty(fildes)) {
		errno = EINVAL;
		return NULL;
	}
	
	strncpy(namebuf, ttyname(fildes), sizeof(namebuf));
	namebuf[1] = 't';	/* Convert ".ptyqX" to ".ttyqX" */
	namebuf[sizeof(namebuf)-1] = 0;
	
	return namebuf;
}

#endif
