	EXENAME := terry
build:
	gcc -pedantic -s -O2 -Werror \
	-nostdlib \
	-fno-asynchronous-unwind-tables \
	-fdata-sections \
	-Wl,--gc-sections \
	-Wa,--noexecstack \
	-fno-builtin \
	-fno-stack-protector \
	-static \
	src/main.S src/main.c \
	-o $$EXENAME \
	\
	&& strip -R .comment $$EXENAME

build-run:
	make build
	./$$EXENAME