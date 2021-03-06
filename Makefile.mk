.INCLUDE: Makefile.common

# /usr/lib/libtermcap.204 should be the ltermcap from GNO 2.0.4. The one 
# from 2.0.6 is broken (links to unimplemented functions), so don't use it.
# The 2.0.4 version is in the "lib.shk" file within  
# ftp://ftp.gno.org/pub/apple2/gs.specific/gno/base/v204/gnodisk1.sdk
LIBS = -l/usr/lib/libtermcap.204 -l/usr/lib/libnetdb -l/usr/lib/libutil

CFLAGS = -i -w -O14

TELNET_A_SPECIFIC = $(TELNET_SPECIFIC) -STELNET_A__
TELNET_B_SPECIFIC = $(TELNET_SPECIFIC) -STELNET_B__
TELNETD_SPECIFIC += -STELNETD___

$(OBJS): $(HEADERS)
%.o: %.c
	$(CC) $(INCLUDES) \
		$(!eq,$(TELNET_SRCS_A:s/$<//),$(TELNET_SRCS_A) $(TELNET_A_SPECIFIC)) \
		$(!eq,$(TELNET_SRCS_B:s/$<//),$(TELNET_SRCS_B) $(TELNET_B_SPECIFIC)) \
		$(!eq,$(TELNETD_SRCS:s/$<//),$(TELNETD_SRCS) $(TELNETD_SPECIFIC)) \
		$(CFLAGS) -c $< -o $@
