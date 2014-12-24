#ifndef TEST_SCRIPT_H
#define TEST_SCRIPT_H

#include <QObject>

class NodeRoot;

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

    /** Confirm that script input creates a new float datum
     */
    void CheckFloatInput();

    /** Confirm that when an input changes, the script emits changed()
     */
    void ChangeFloatInput();

    /** Confirm that script input creates a new Shape datum
     */
    void CheckShapeInput();

    /** Try to make an input with an invalid name.
     */
    void InvalidInputNames();

    /** Make sure that datums are deleted when script changes.
     */
    void AddThenRemoveDatum();

    /** Use a datum from another node in a script.
     */
    void UseOtherDatum();

    /** Create a shape output
     */
    void MakeShapeOutput();

    /** Changes the order of input calls to reorder datums.
     */
    void ChangeInputOrder();

    /** Make sure that an output shape datum is updated.
     */
    void ShapeUpdate();

protected:
    NodeRoot* r;
};

#endif // TEST_SCRIPT_H
