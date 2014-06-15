#ifndef TEST_SCRIPT_H
#define TEST_SCRIPT_H

#include <QObject>

class TestScript : public QObject
{
    Q_OBJECT
public:
    explicit TestScript(QObject *parent = 0);
private slots:
    /** Test that a simple script can execute. */
    void RunSimpleScript();

    /** Check that an invalid script is flagged as invalid. */
    void RunInvalidScript();

    /** Try a two-line simple script
     */
    void RunMultilineScript();
};

#endif // TEST_SCRIPT_H
