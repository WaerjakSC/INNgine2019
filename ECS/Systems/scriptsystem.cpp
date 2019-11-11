#include "scriptsystem.h"
#include "resourcemanager.h"
ScriptSystem::ScriptSystem() : factory(ResourceManager::instance()) {
}

void ScriptSystem::call(Script &script, const QString &func) {
    QJSValue value = script.engine->evaluate(func, script.filePath);
    if (value.isError()) {
        factory->checkError(value);
    }
}
