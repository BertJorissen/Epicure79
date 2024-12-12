
/*
 * <license>
 * 
 * Hitmap v1.4
 * 
 * This software is provided to enhance knowledge and encourage progress in the scientific
 * community. It should be used only for research and educational purposes. Any reproduction
 * or use for commercial purpose, public redistribution, in source or binary forms, with or 
 * without modifications, is NOT ALLOWED without the previous authorization of the copyright 
 * holder. The origin of this software must not be misrepresented; you must not claim that you
 * wrote the original software. If you use this software for any purpose (e.g. publication),
 * a reference to the software package and the authors must be included.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * Copyright (c) 2007-2024, Trasgo Group, Universidad de Valladolid.
 * All rights reserved.
 * 
 * More information on http://trasgo.infor.uva.es/
 * 
 * </license>
*/

#include <stdio.h>
#include <stdlib.h>


#ifdef __linux__
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <unistd.h>
#endif


char * get_gdb_trace(){

// Only for LINUX
#ifdef __linux__

	// String trace
	static char trace[1024*10];

	// Pipe to connect parent and child processes
	int pipefd[2];

	// 1. CREATE THE PIPE
	if (pipe(pipefd) == -1) {
		fprintf(stderr,"get_trace: Error pipe\n");
		exit(EXIT_FAILURE);
	}

	// 2. GET THE PID AND NAME FOR THE PARENT
	char pid_buf[30];
	sprintf(pid_buf, "%d", getpid());
	char name_buf[512];
	ssize_t readed = readlink("/proc/self/exe", name_buf, 511);
	name_buf[readed]=0;

	// 3. FORK
	pid_t cpid = fork();
	if (cpid == -1) {
		fprintf(stderr,"get_trace: Error fork\n");
		exit(EXIT_FAILURE);
	}

	// 3.1. CHILD PROCESSOR
	if (cpid == 0) {

		// Close the input pipe end
		close(pipefd[0]);

		// Redirect stdout and stderr to the pipe
		dup2(pipefd[1],1);
		dup2(pipefd[1],2);

		// Call the process
		printf("GDB %s - %s\n",name_buf,pid_buf);
		execlp("gdb", "gdb", "--batch", "-n", "-ex", "thread", "-ex", "bt", name_buf, pid_buf, NULL);
		printf("Error calling gdb\n");
		abort();

    // 3.2. PARENT PROCESSOR
	} else {

		// Allow the child to see our trace
		// @arturo: Since linux 3.4 (TODO)
		//prctl(PR_SET_PTRACER, cpid, 0, 0, 0);

		// Close unused output pipe end
		close(pipefd[1]);

		// Get the trace from the child
		char buf;
		int i = 0;
		while (read(pipefd[0], &buf, 1) > 0){
			trace[i++] = buf;
		}
		trace[i] = '\0';

		// Wait for the child
		waitpid(cpid,NULL,0);
		close(pipefd[0]);
	}

	// 4. RETURN THE TRACE
	return trace;

#else /* Not Linux */

	// Not supported
	return NULL;

#endif

}



