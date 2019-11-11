#ifndef SCRIPTSYSTEM_H
#define SCRIPTSYSTEM_H

#include "components.h"
#include "isystem.h"
#include <QJSValueList>
struct Script {
    Script() {
        engine = new QJSEngine;
        engine->installExtensions(QJSEngine::ConsoleExtension);
    }

    QJSEngine *engine;
    QString filePath;
};
class ResourceManager;
class ScriptSystem : public ISystem {
public:
    ScriptSystem();

    void call(Script &script, const QString &func);

    bool readScript(Script &comp, const QString &fileName);
    QString checkError(QJSValue value);

private:
    ResourceManager *factory;
};

#endif // SCRIPTSYSTEM_H
