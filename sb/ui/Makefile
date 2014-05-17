UI_FILES := $(shell find . -name "*.ui")
PY_FILES := $(subst ./,./ui_,$(UI_FILES:.ui=.py))

all: $(PY_FILES)

ui_%.py: %.ui
	pyside-uic -o $@ $<
