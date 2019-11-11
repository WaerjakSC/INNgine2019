#include "scriptsystem.h"
#include "resourcemanager.h"
#include <QFileInfo>
ScriptSystem::ScriptSystem() : factory(ResourceManager::instance()) {
}

void ScriptSystem::call(Script &script, const QString &func) {
    QJSValue value = script.engine->evaluate(func, script.filePath);
    if (value.isError()) {
        checkError(value);
    }
}
bool ScriptSystem::readScript(Script &comp, const QString &fileName) {
    //Make the Script engine itself
    QJSEngine engine;

    if (!fileName.size())
        return false;

    //Make a QFile for it
    QFile scriptFile(fileName);
    if (scriptFile.exists()) {
        qDebug() << "Script file (" + fileName + ") does not exist!";
        return false;
    }
    if (!scriptFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open script: " << fileName;
    }
    //Try to open file and give error if something is wrong
    if (!scriptFile.open(QIODevice::ReadOnly))
        qDebug() << "Error - NO FILE HERE: " << fileName;

    //    //reads the file
    QTextStream stream(&scriptFile);
    QString contents = stream.readAll();
    //close the file, because we don't need it anymore
    scriptFile.close();

    auto value = engine.evaluate(contents, fileName);
    if (value.isError()) {
        checkError(value);
        return false;
    }
    QFileInfo info(fileName);
    comp.filePath = info.filePath();
    return true;
}
QString ScriptSystem::checkError(QJSValue value) {
    QString lineNumber = QString::number(value.property("lineNumber").toInt());
    QString valueString = value.toString();
    QString error("Uncaught exception at line" + lineNumber + ":" + valueString);
    qDebug() << error;
    return error;
}
