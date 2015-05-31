/* We use this as a hack to define stuff in every file, since they all
 * #include <sys/cdefs.h> first.  When compiling with occ, this file will
 * be included in preference to the system header of the same name. */

#ifdef __GNO__
# define __unused
#endif

#include "/usr/include/sys/cdefs.h"
