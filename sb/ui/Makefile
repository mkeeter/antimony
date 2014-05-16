UI_FILES := $(shell find . -name "*.ui")
PY_FILES := $(UI_FILES:.ui=.py)

all: $(PY_FILES)

%.py: %.ui
	pyside-uic -o ui_$@ $<
