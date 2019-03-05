SOURCES = GameController.cpp Main.cpp PlayersideRead.cpp PlayersideWrite.cpp Thread.cpp ThreadFunctor.cpp BallDispenserRead.cpp I2C.cpp BallDispenserWrite.cpp#WebPage.cpp
OBJECTS = ${SOURCES:.cpp=.o}
EXECUTABLE = edit
INSTALL_DIR = /home/stud
CXX = arm-rpizw-g++
CXXFLAGS = -I.

%.o: %.cpp
	${CXX} -c -o $@ $^ ${CXXFLAGS}

${EXECUTABLE}: ${OBJECTS}
	${CXX} $(CXXFLAGS) -o $@ $(OBJECTS) -lpthread

install:
	cp ${EXECUTABLE} ${INSTALL_DIR}

run:
	${INSTALL_DIR}/${EXECUTABLE}
clean:
	rm ${EXECUTABLE} ${OBJECTS}
help:
	echo ${SOURCES}
