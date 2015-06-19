cygwin {
    LIBS += $$system(libpng-config --libs)
}

macx {
    # Link against libpng for image export
    LIBS += -L/usr/local/lib -lpng
    INCLUDEPATH += /usr/local/include/libpng15/
    INCLUDEPATH += /usr/local/include/libpng16/
}

linux {
    # Link against libpng for image export
    LIBS += -lpng
}
