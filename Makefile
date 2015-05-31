LIBTELNET_SRCS = \
  libtelnet/auth.c \
  libtelnet/enc_des.c \
  libtelnet/encrypt.c \
  libtelnet/genget.c \
  libtelnet/getent.c \
  libtelnet/kerberos.c \
  libtelnet/kerberos5.c \
  libtelnet/krb4encpwd.c \
  libtelnet/misc.c \
  libtelnet/read_password.c \
  libtelnet/rsaencpwd.c \
  libtelnet/sra.c
# libtelnet/pk.c 

TELNET_SRCS = \
  telnet/authenc.c \
  telnet/commands.c \
  telnet/main.c \
  telnet/network.c \
  telnet/ring.c \
  telnet/sys_bsd.c \
  telnet/telnet.c \
  telnet/terminal.c \
  telnet/utilities.c

TELNETD_SRCS = \
  telnetd/authenc.c \
  telnetd/global.c \
  telnetd/slc.c \
  telnetd/state.c \
  telnetd/sys_term.c \
  telnetd/telnetd.c \
  telnetd/termstat.c \
  telnetd/utility.c

HEADERS = \
  arpa/telnet.h \
  libtelnet/auth-proto.h \
  libtelnet/auth.h \
  libtelnet/enc-proto.h \
  libtelnet/encrypt.h \
  libtelnet/key-proto.h \
  libtelnet/misc-proto.h \
  libtelnet/misc.h \
  libtelnet/pk.h \
  telnet/baud.h \
  telnet/defines.h \
  telnet/externs.h \
  telnet/fdset.h \
  telnet/general.h \
  telnet/ring.h \
  telnet/types.h \
  telnetd/defs.h \
  telnetd/ext.h \
  telnetd/pathnames.h \
  telnetd/telnetd.h

LIBTELNET_OBJS = $(LIBTELNET_SRCS:.c=.o)
TELNET_OBJS = $(TELNET_SRCS:.c=.o)
TELNETD_OBJS = $(TELNETD_SRCS:.c=.o)
OBJS = $(LIBTELNET_OBJS) $(TELNET_OBJS) $(TELNETD_OBJS)

INCLUDES = -I libtelnet -I .
TELNET_DEFINES = -DTERMCAP -DKLUDGELINEMODE -DENV_HACK -DOPIE
TELNETD_DEFINES = -DDIAGNOSTICS -DOLD_ENVIRON -DENV_HACK -DSTREAMSPTY
DEFINES = $(TELNET_DEFINES) $(TELNETD_DEFINES)
LIBS = -ltermcap
# CFLAGS suitable for clang or GCC
CFLAGS = -std=c89 -funsigned-char -Wall -Wno-pointer-sign -Wno-unused-variable -Wno-string-plus-int

.PHONY: all
all: telnet/telnet telnetd/telnetd

telnet/telnet: $(TELNET_OBJS) $(LIBTELNET_OBJS)
	$(CC) $(LIBS) $(TELNET_OBJS) $(LIBTELNET_OBJS) -o $@

telnetd/telnetd: $(TELNETD_OBJS) $(LIBTELNET_OBJS)
	$(CC) $(LIBS) $(TELNETD_OBJS) $(LIBTELNET_OBJS) -o $@

$(OBJS): $(HEADERS)
%.o: %.c
	$(CC) $(INCLUDES) \
		$(if $(findstring $<,$(TELNET_SRCS)),-I telnet $(TELNET_DEFINES)) \
		$(if $(findstring $<,$(TELNETD_SRCS)),-I telnetd $(TELNETD_DEFINES)) \
		$(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	$(RM) $(OBJS) telnet/telnet telnetd/telnetd
