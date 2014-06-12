#ifndef FAB_H
#define FAB_H

namespace fab
{
    struct ParseError {};
    void onParseError(ParseError const& e);
    void loadModule();
}

#endif // FAB_H
