# Extract the git revision, tag, and branch, then populate them
# into various preprocessor macros to populate the About box.
GITREV = $$system(git log --pretty=format:'%h' -n 1)
GITDIFF = $$system(git diff --quiet --exit-code || echo "+")
GITTAG = $$system(git describe --exact-match --tags 2> /dev/null)
GITBRANCH = $$system(git rev-parse --abbrev-ref HEAD)

QMAKE_CXXFLAGS += "-D'GITREV=\"$${GITREV}$${GITDIFF}\"'"
QMAKE_CXXFLAGS += "-D'GITTAG=\"$${GITTAG}\"'"
QMAKE_CXXFLAGS += "-D'GITBRANCH=\"$${GITBRANCH}\"'"
