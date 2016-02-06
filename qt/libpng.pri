cygwin {
    LIBS += $$system(libpng-config --libs)
}

macx {
    # Link against libpng for image export
    !contains(DEFINES, BREW_HOME){
        BREW_HOME=/usr/local
    }
    LIBS += -L$$(BREW_HOME)/lib -lpng
    INCLUDEPATH += $$(BREW_HOME)/include/libpng15/
    INCLUDEPATH += $$(BREW_HOME)/include/libpng16/
}

linux {
    # Link against libpng for image export
    LIBS += -lpng
}
