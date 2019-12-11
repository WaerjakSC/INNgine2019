#ifndef SCRIPTSYSTEM_H
#define SCRIPTSYSTEM_H

#include "isystem.h"
#include <QJSEngine>
#include <QJSValueList>
/**
 * @brief The Script struct is a simple script prototype made for a compulsory
 */
struct Script {
    Script()
    {
        engine = new QJSEngine;
        engine->installExtensions(QJSEngine::ConsoleExtension);
    }

    QJSEngine *engine;
    QString filePath;
};
class ResourceManager;
/**
 * @brief The ScriptSystem class is a simple proof-of-concept script reader
 */
class ScriptSystem : public ISystem {
public:
    ScriptSystem();
    void update(DeltaTime = 0.016) override;
    void init();

    /**
     * @brief call a given script's function
     * @param script
     * @param func
     */
    void call(Script &script, const QString &func);

    /**
     * @brief readScript read script at fileName
     * @param fileName
     * @return did it find the file?
     */
    bool readScript(const QString &fileName);
    /**
     * @brief checkError check for errors in the script function
     * @param value
     * @return
     */
    QString checkError(QJSValue value);

private:
    ResourceManager *factory;
    Script script;
};

#endif // SCRIPTSYSTEM_H
