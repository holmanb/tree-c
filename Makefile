SRC := tests/main.c src/bst.c
BIN := tests/test
CFLAGS += -D__DEBUG_STACK

all:
	$(CC) -g $(CFLAGS) $(SRC) -o $(BIN)

test: all
	$(BIN)

gdb: all
	gdb $(BIN)

clean:
	-rm $(BIN)

check: all
	valgrind --leak-check=full  --track-origins=yes --exit-on-first-error=yes $(BIN)

asan-build:
	clang -g $(CFLAGS) -fsanitize=address -fno-omit-frame-pointer $(SRC) -o $(BIN)

asan: asan-build
	$(BIN)

coverage: test
	clang -fprofile-instr-generate -fcoverage-mapping -Wno-gnu-zero-variadic-macro-arguments $(CFLAGS) $(SRCS) -o $(BIN)
	LLVM_PROFILE_FILE="$(PROFRAW)" $(BIN)
	llvm-profdata merge -sparse $(PROFRAW) -o $(PROFDATA)
	llvm-cov show $(BIN) -instr-profile=$(PROFDATA)
	llvm-cov report $(TESTS) -instr-profile=$(PROFDATA)
