/*
 * Copyright (c) 2014, 2016 Stephen Heumann
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
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#ifdef __GNO__
#include <gno/gno.h>
#endif

/* Like vfork, but calls fn(arg) in the child instead of returning.
 * This is designed to match the semantics of GNO's fork2 call.
 */
#ifndef __GNO__

pid_t vfork_and_run(void (*fn)(void*) /*NORETURN*/, void *arg) {
	/* Use fork rather than vfork because vfork is problematic on OS X. */
	pid_t pid = fork();

	if (pid == 0) {
		fn(arg);
	}

	return pid;
}

#else

# include <signal.h>
# include <gno/kvm.h>
# include <orca.h>

/* Turn off all ORCA/C stack repair code to avoid corruption. */
# ifdef __ORCAC__
#  pragma optimize 72
# endif

# pragma databank 1
void fork_thunk(void (*fn)(void*) /*NORETURN*/, void *arg, long sigmask) {
	sigsetmask(sigmask);
	fn(arg);
}
# pragma databank 0

const char *forked_child_name = "telnetd pty slave proc";
#define CHILD_STACKSIZE 1024

pid_t vfork_and_run(void (*fn)(void*) /*NORETURN*/, void *arg) {
	/* GNO's fork2 call will return immediately and allow the parent and 
	 * child processes to execute concurrently using the same memory
	 * space.  To prevent them stomping on each other, we want to get
	 * behavior like a traditional vfork() implementation, where the
	 * parent blocks until the child terminates or execs.
	 *
	 * Our approach is to check the process tables to make sure the
	 * child has actually finished or exec'd.  If not, we loop and try again.
	 * We can't just rely on the fact that the child signaled us, because
	 * it may still be running in libc's implementation of exec*.
	 */
	
	long oldmask;
	pid_t pid;
	kvmt *kvm_context;
	struct pentry *proc_entry;
	int done = 0;
	
	/* Isolate child process's environment from parent */
	if (environPush() != 0)
		return  -1;
	
	/* Block all signals for now */
	oldmask = sigblock(-1);
	
	pid = fork2(fork_thunk, CHILD_STACKSIZE, 0, forked_child_name, 
				(sizeof(fn) + sizeof(arg) + sizeof(oldmask) + 1) / 2, 
				fn, arg, oldmask);
	if (pid < 0) 
		goto ret;
	
	while (!done) {
		/* Wait for ~100 ms.  If procsend worked, the child could send a 
		 * message with it to end the waiting earlier, but this isn't 
		 * possible in GNO 2.0.6 because procsend is broken.  This isn't
		 * too big an issue, since 100ms isn't very long to wait anyhow. */
		procrecvtim(1);
		
		/* Check if the child is really dead or forked by inspecting
		 * the kernel's process entry for it. */
		kvm_context = kvm_open();
		if (kvm_context == NULL)
			break;
		proc_entry = kvmgetproc(kvm_context, pid);
		if (proc_entry == NULL 
			|| (proc_entry->args != NULL 
				&& strcmp(forked_child_name, proc_entry->args + 8) != 0))
			done = 1;
		kvm_close(kvm_context);
	}
	
ret:
	sigsetmask(oldmask);
	environPop();
	return pid;
}

#endif
