cmd_drivers/dahdi/xpp/print_fxo_modes.o := gcc -Wp,-MD,drivers/dahdi/xpp/.print_fxo_modes.o.d -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer    -include drivers/dahdi/xpp/../fxo_modes.h -c -o drivers/dahdi/xpp/print_fxo_modes.o drivers/dahdi/xpp/print_fxo_modes.c

deps_drivers/dahdi/xpp/print_fxo_modes.o := \
  drivers/dahdi/xpp/print_fxo_modes.c \
  drivers/dahdi/xpp/../fxo_modes.h \
  /usr/include/stdio.h \
  /usr/include/features.h \
  /usr/include/bits/predefs.h \
  /usr/include/sys/cdefs.h \
  /usr/include/bits/wordsize.h \
  /usr/include/gnu/stubs.h \
  /usr/include/gnu/stubs-32.h \
  /usr/lib/gcc/i486-linux-gnu/4.4.1/include/stddef.h \
  /usr/include/bits/types.h \
  /usr/include/bits/typesizes.h \
  /usr/include/libio.h \
  /usr/include/_G_config.h \
  /usr/include/wchar.h \
  /usr/lib/gcc/i486-linux-gnu/4.4.1/include/stdarg.h \
  /usr/include/bits/stdio_lim.h \
  /usr/include/bits/sys_errlist.h \
  /usr/include/bits/stdio.h \
  /usr/include/bits/stdio2.h \

drivers/dahdi/xpp/print_fxo_modes.o: $(deps_drivers/dahdi/xpp/print_fxo_modes.o)

$(deps_drivers/dahdi/xpp/print_fxo_modes.o):
