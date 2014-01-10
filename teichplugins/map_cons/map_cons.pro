include (../../shared.pri)

TARGET = map_cons

HEADERS += ./map_cons.h
SOURCES += ./map_cons.cpp

RESOURCES += \
    map_cons.qrc

	
	

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/teichmuller/ -lteichmuller
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/teichmuller/ -lteichmuller
#else:unix: LIBS += -L$$PWD/teichmuller/ -lteichmuller

INCLUDEPATH += D:\math\teichmuller\src
DEPENDPATH += D:\math\teichmuller\lib

INCLUDEPATH += D:\math\github\vcglib\eigenlib
DEPENDPATH += D:\math\github\vcglib\eigenlib

INCLUDEPATH += D:/math/Library/SuperLU_4.3/SRC
DEPENDPATH += D:/math/Library/SuperLU_4.3/SRC

##cholmod.h
#INCLUDEPATH += D:/math/Library/SuiteSparseCMake/SuiteSparse/include
#DEPENDPATH += D:/math/Library/SuiteSparseCMake/SuiteSparse/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../teichmuller/release/ -lteichmuller
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../teichmuller/debug/ -lteichmuller
else:unix: LIBS += -L$$PWD/../../../../teichmuller/ -lteichmuller

LIBS += -LD:\math\teichmuller\lib -lsuitesparse
LIBS += -LD:\math\teichmuller\lib -lblas
LIBS += -LD:\math\teichmuller\lib -llibblas
LIBS += -LD:\math\teichmuller\lib -lliblapack
LIBS += -LD:\math\teichmuller\lib -lsuperlu
LIBS += -LD:\math\teichmuller\lib -lmetis

INCLUDEPATH += $$PWD/../../../../teichmuller/Debug
DEPENDPATH += $$PWD/../../../../teichmuller/Debug

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../teichmuller/release/teichmuller.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../teichmuller/debug/teichmuller.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../../../teichmuller/libteichmuller.a