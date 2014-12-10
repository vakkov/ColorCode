# #####################################################################
# Automatically generated by qmake (2.01a) Thu Sep 24 22:30:55 2009
# #####################################################################
QMAKE_CFLAGS_DEBUG -= -O2
QMAKE_CFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_DEBUG -= -O2
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS += -O3
QMAKE_CFLAGS += -O3
QT += widgets
TARGET = colorcode
TEMPLATE = app

# Input
HEADERS += about.h \
    colorcode.h \
    colorpeg.h \
    msg.h \
    pegrow.h \
    rowhint.h \
    ccsolver.h \
    background.h \
    solutionrow.h \
    graphicsbtn.h \
    settings.h \
    prefdialog.h \
    pegfactory.h \
    gamenodisplay.h \
    timedisplay.h \
    timedisplaybg.h \
    highscore.h \
    ccgame.h \
    buttonscell.h \
    buttondelegate.h \
    cellbtn.h \
    gametimer.h \
    savedgamesmodel.h \
    prevgamesmodel.h \
    highscoresmodel.h \
    gametableview.h \
    gametablesdialog.h \
    gameslistmodel.h
FORMS += about.ui \
    prefdialog.ui \
    gametablesdialog.ui
SOURCES += about.cpp \
    colorcode.cpp \
    colorpeg.cpp \
    main.cpp \
    msg.cpp \
    pegrow.cpp \
    rowhint.cpp \
    ccsolver.cpp \
    background.cpp \
    solutionrow.cpp \
    graphicsbtn.cpp \
    settings.cpp \
    prefdialog.cpp \
    pegfactory.cpp \
    gamenodisplay.cpp \
    timedisplay.cpp \
    timedisplaybg.cpp \
    highscore.cpp \
    ccgame.cpp \
    buttonscell.cpp \
    buttondelegate.cpp \
    cellbtn.cpp \
    gametimer.cpp \
    savedgamesmodel.cpp \
    prevgamesmodel.cpp \
    highscoresmodel.cpp \
    gametableview.cpp \
    gametablesdialog.cpp \
    gameslistmodel.cpp
RESOURCES += resource.qrc
OTHER_FILES += docs/GPL.html
win32:RC_FILE = colorcode.rc
TRANSLATIONS += trans_de.ts \
    trans_en.ts \
    trans_cs.ts \
    trans_fr.ts \
    trans_hu.ts \
    trans_es.ts
CODECFORTR = UTF-8
CODECFORSRC = UTF-8
