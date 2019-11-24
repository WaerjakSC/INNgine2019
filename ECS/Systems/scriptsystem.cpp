#include "scriptsystem.h"
#include "constants.h"
#include "resourcemanager.h"
#include <QFileInfo>
ScriptSystem::ScriptSystem() : factory{ResourceManager::instance()} {
}

void ScriptSystem::update(DeltaTime deltaTime) {
    Q_UNUSED(deltaTime);
}

void ScriptSystem::init() {
    readScript(QString::fromStdString(gsl::assetFilePath) + "Scripts/testscript.js");
}

bool ScriptSystem::readScript(const QString &fileName) {
    //Make the Script engine itself
    QJSEngine engine;

    if (!fileName.size())
        return false;

    //Make a QFile for it
    QFile scriptFile{fileName};
    if (!scriptFile.exists()) {
        qDebug() << "Script file (" + fileName + ") does not exist!";
        return false;
    }
    if (!scriptFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open script: " << fileName;
    }
    //    //reads the file
    QTextStream stream{&scriptFile};
    QString contents{stream.readAll()};
    //close the file, because we don't need it anymore
    scriptFile.close();

    auto value{engine.evaluate(contents, fileName)};
    if (value.isError()) {
        checkError(value);
        return false;
    }
    QFileInfo info{fileName};
    QJSValue func{engine.evaluate("beginPlay").call()};
    qDebug() << func.toString();
    return true;
}
QString ScriptSystem::checkError(QJSValue value) {
    QString lineNumber{QString::number(value.property("lineNumber").toInt())};
    QString valueString{value.toString()};
    QString error{"Uncaught exception at line" + lineNumber + ":" + valueString};
    qDebug() << error;
    return error;
}
