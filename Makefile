include Makefile.common

LIBS = -ltermcap

# CFLAGS suitable for clang or GCC
CFLAGS = -std=c89 -funsigned-char -Wall -Wno-pointer-sign -Wno-unused-variable -Wno-string-plus-int

$(OBJS): $(HEADERS)
%.o: %.c
	$(CC) $(INCLUDES) \
		$(if $(findstring $<,$(TELNET_SRCS)),$(TELNET_SPECIFIC)) \
		$(if $(findstring $<,$(TELNETD_SRCS)),$(TELNETD_SPECIFIC)) \
		$(CFLAGS) -c $< -o $@

