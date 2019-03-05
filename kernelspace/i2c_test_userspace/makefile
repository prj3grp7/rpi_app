SOURCES = test_file.cpp
CXXFLAGS = -ggdb -I.

#Making for host
#>make ARCH = host
ifeq (${ARCH},host)
CXX = g++
BUILD_DIR = build/host
endif

#Making for target
#>make ARCH = target
ifeq (${ARCH},target)
CXX = arm-rpizw-g++
BUILD_DIR = build/target
endif

OBJECTS = ${addprefix ${BUILD_DIR}/,${SOURCES:.cpp=.o}}
DEPS = ${addprefix ${BUILD_DIR}/,${SOURCES:.cpp=.d}}
EXECUTABLE = ${BUILD_DIR}/output

.PHONY all:
all: ${EXECUTABLE} ${MKDIR_P}

${EXECUTABLE}: ${DEPS} ${OBJECTS}
	${CXX} -o $@ ${OBJECTS} ${CXXFLAGS}

.PHONY clean:
clean:
	rm -f ${DEPS} ${OBJECTS} ${EXECUTABLE}

${BUILD_DIR}/%.d: %.cpp
	@echo "Generating "$@
	${CXX} -MT ${@:.d=.o} -MM ${CXXFLAGS} $^ > $@

${BUILD_DIR}/%.o: %.cpp
	@echo "Compiling "$@
	${CXX} -c $< -o $@ ${CXXFLAGS} 

ifneq (${MAKECMDGOALS},clean)
-include ${DEPS}
${shell mkdir -p ${BUILD_DIR}}
endif
