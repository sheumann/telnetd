telnetd for GNO
===============

This is a port of the telnet server (telnetd) for GNO, based on FreeBSD 
sources.  It allows you to log in to a GNO system remotely using telnet.
To use it, you need GNO 2.0.6 and MariGNOtti 0.4 or later.  It may also
work with other TCP/IP stacks for GNO, but that is untested.  Note that
it will not work with Sweet16, because Sweet16's TCP/IP configuration 
does not support servers running on the emulated IIgs.

The binary download also includes inetd (compiled from GNO 2.0.6 
sources), since telnetd is designed to be run from inetd.

The source repository also includes code for the telnet client,
but it does not currently work and is not included in the download.

Installation
------------

1. Copy the `telnetd` and `inetd` binaries to your `/usr/local/bin` 
   directory.  (If you use a different location, you will have to 
   adjust the configuration files appropriately.)

2. Copy the `etc/inetd.conf` file to your `/etc` directory.  (If you
   have an existing `/etc/inetd.conf` file, you should add the new
   entry to the end of it instead.)

Running telnetd
---------------

Once you have completed the above steps, you can enable the telnet
server (in addition to any other servers managed by inetd) by starting 
MariGNOtti and then simply running `inetd` as root.  It will accept any 
number of telnet connections (up to the limit of system resources) and 
continue running until you stop inetd or shut down GNO.

It is also possible to run `telnetd -debug` (as root) to accept a telnet 
connection without using inetd.  In this mode, telnetd will accept a 
single telnet connection and quit once that connection finishes.
For more information on running telnetd, see the included `telnetd.8` 
man page (which does not currently display correctly on GNO).
Be aware that some features, including authentication, encryption,
line mode, and IPv6 support, are not available on GNO.

Starting the server automatically
---------------------------------

If you want your GNO system to accept telnet connections as soon as it
starts up (without logging in on the local console and running `inetd`), 
you can make MariGNOtti and inetd start up automatically.  To do this, 
you can add lines like the following to the `/etc/inittab` file
(these are similar to commented-out lines for GS/TSP in the stock 
GNO 2.0.6 version of the file, which you can modify):

    ip:2348:once::/usr/local/bin/marignotti
    id:234:once::/usr/local/bin/inetd

If you add these lines, you will also need to set the default run level 
to one that will start up inetd, e.g. run level 3.  If you are using the 
stock GNO 2.0.6 configuration, you can just change the `8` to a `3` in 
the following line :

    db:b:runl::8

Note the the meaning of the run levels is just a convention and you can
use different ones if you want; see the `init(8)` man page for details.

Connecting to GNO over telnet
-----------------------------

Once you have your system set up as described above, you should be able
to telnet to its IP address and connect to it.  In some cases, I have
seen the first attempted connection be quite slow and occasionally fail;
if that happens, try again.

If you are connecting to GNO from a modern terminal emulator, you will
also want to set it up appropriately to use that terminal type.
The telnet client will normally indicate the terminal type when 
connecting, and telnetd will set the `TERM` environment variable 
accordingly.  Make sure your shell login/startup scripts are not 
resetting it to something inappropriate. (In particular, the default
`gshrc` script sets `TERM` to `gnocon`; you should remove that line.)

You will also want to add a termcap entry for your terminal in
`/etc/termcap`.  The included `etc/xterm.termcap` file provides an 
entry that should be suitable for several modern terminal emulators,
including xterm and OS X's Terminal.app.  On many modern *nix systems,
you can run `infocmp -C` to get an entry for your terminal.  Note that
termcap entries on GNO are strictly limited to 1023 characters.
