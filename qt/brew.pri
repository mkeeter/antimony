macx {
    BREW_HOME = $$(BREW_HOME)
    isEmpty(BREW_HOME){
        BREW_HOME = /usr/local
    }
}
