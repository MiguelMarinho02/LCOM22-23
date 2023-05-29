# name of the program (Minix service)
PROG=proj
.PATH: ${.CURDIR}/src ${.CURDIR}/lib
# source code files to be compiled
SRCS += proj.c gpu.c keyboard.c timer.c mouse.c utils.c game_loop.c controller.c draw_elements.c sprite.c rtc.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic

# list of library dependencies (for Lab 2, only LCF library)
DPADD += ${LIBLCF}
LDADD += -llcf

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>

CPPFLAGS += -D PROJ
