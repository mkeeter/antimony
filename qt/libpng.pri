# Link against libpng for image export

cygwin {
    LIBS += $$system(libpng-config --libs)
}

macx {
    include(brew.pri)
    LIBS += -L$${BREW_HOME}/lib -lpng
    INCLUDEPATH += $${BREW_HOME}/include/libpng15/
    INCLUDEPATH += $${BREW_HOME}/include/libpng16/
}

linux {
    LIBS += -lpng
}
