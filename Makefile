LIB_SRCS = $(shell find src -type f -name '*.cpp')
LIB_OBJS_PREFIX = build/cxxspec
LIB_OBJS = $(patsubst %.cpp, $(LIB_OBJS_PREFIX)/%.o, $(LIB_SRCS))

SPEC_SRCS = $(shell find spec -type f -name '*.cpp')
SPEC_OBJS_PREFIX = build/specs
SPEC_OBJS = $(patsubst %.cpp, $(SPEC_OBJS_PREFIX)/%.o, $(SPEC_SRCS))

HEADERS_RAW = $(shell find src -type f -name '*.hpp')
HEADERS = $(patsubst src/%.hpp, %.hpp, $(HEADERS_RAW))

BUILD_PREFIX = build

ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif

CFLAGS = -O3 -Isrc

all: libcxxspec specs

libcxxspec: $(BUILD_PREFIX)/libcxxspec.so
specs: libcxxspec $(BUILD_PREFIX)/specs.run

$(BUILD_PREFIX)/libcxxspec.so: $(LIB_OBJS)
	@mkdir -p "$(@D)"
	$(CXX) -o $@ $^ -shared -fPIC -m64 -s

$(BUILD_PREFIX)/specs.run: $(SPEC_OBJS)
	@mkdir -p "$(@D)"
	$(CXX) -o $@ $^ -m64 -L$(BUILD_PREFIX) -Wl,-rpath=\$$ORIGIN -s -lcxxspec

$(LIB_OBJS_PREFIX)/%.o: %.cpp
	@mkdir -p "$(@D)"
	$(CC) -c -m64 -fPIC $(CFLAGS) -DNDEBUG -o $@ $^

$(SPEC_OBJS_PREFIX)/%.o: %.cpp
	@mkdir -p "$(@D)"
	$(CC) -c -m64 -fvisibility=hidden -fvisibility-inlines-hidden $(CFLAGS) -DNDEBUG -o $@ $^

install: libcxxspec
	install -d $(PREFIX)/lib/
	install -m 644 $(BUILD_PREFIX)/libcxxspec.so $(PREFIX)/lib/
	install -d $(PREFIX)/include/cxxspec/
	for headerfile in $(HEADERS) ; do \
		install -d $(PREFIX)/include/cxxspec/$$(dirname $$headerfile)/ ; \
		install -m 644 src/$$headerfile $(PREFIX)/include/cxxspec/$$headerfile ; \
	done

uninstall:
	echo "This will execute destructive actions; continue? [y/N]" ; \
		read answer ; \
		if [[ "$$answer" = "y" ]]; then \
			rm -v $(PREFIX)/lib/libcxxspec.so ; \
			rm -rv $(PREFIX)/include/cxxspec ; \
		else \
			echo "uninstalling aborted." ; \
		fi

clean:
	rm -rf ./build

.PHONY: clean install