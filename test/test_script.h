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

    /** Try to import fab.shapes to make sure it's in the namespace.
    */
    void ImportFabShapes();

    /** Make a script node (rather than a bare script datum).
     */
    void MakeScriptNode();

    /** Make a script node with an input
     */
    void MakeScriptInput();

    /** Confirm that script input creates a new datum
     */
    void CheckScriptInput();
};

#endif // TEST_SCRIPT_H
