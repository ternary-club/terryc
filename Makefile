build:
	@echo ==== building =========================
	gcc -pedantic -s -O2 -Werror \
	-nostdlib \
	-fno-asynchronous-unwind-tables \
	-fdata-sections \
	-Wl,--gc-sections \
	-Wa,--noexecstack \
	-fno-builtin \
	-fno-stack-protector \
	-static \
	./src/main.s ./src/main.c \
	-o terry \
	\
	&& strip -R .comment terry
	exit

build-run:
	make build
	@echo ==== running ==========================
	@./terry