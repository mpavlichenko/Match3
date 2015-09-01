#include "engine.h"

int Engine::m_prevIndex (-1);

Engine::Engine(QObject *parent) : QAbstractListModel(parent) {

    delayIfAnyMatch = new QTimer(this);
    delayAddElement = new QTimer(this);

    m_scoreCount = 0;
    m_movesCount = 0;
    m_startCount = false;

    connect(delayIfAnyMatch, &QTimer::timeout, this, &Engine::ifAnyMatch);
    connect(delayAddElement, &QTimer::timeout, this, &Engine::addElement);

    loadSettings();

    resetBoard();

    boardInitialization();
}

void Engine::boardInitialization() {

    delayAddElement->start(50);
    delayIfAnyMatch->start(10);
}

void Engine::doSMTH() {

    QEventLoop loop;

    QTimer::singleShot(300, &loop, SLOT(quit()));
    loop.exec();
}

bool Engine::loadSettings() {

    QFile loadFile("settings.JSON");

    if(!loadFile.open(QIODevice::ReadOnly)) {

        QLoggingCategory category("loadFile");
        qCWarning(category) << "Couldn't open save file.";
        return false;
    }

    QByteArray saveData = loadFile.readAll();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(saveData));

    readSettings(jsonDoc.object());

    return true;
}

void Engine::readSettings(const QJsonObject &json) {

    m_columnsCount = json["width"].toInt();
    m_rowsCount = json["height"].toInt() + 1;
    m_elementScore = json["elementScore"].toInt();
    m_minScore = json["minScore"].toInt();
    m_maxMoves = json["maxMoves"].toInt();

    QJsonArray itemsArray = json["types"].toArray();
    for(const QJsonValueRef arr : itemsArray) {
        m_elements.append(arr.toInt());
    }
}

void Engine::moveRight(int index) {

    if((index % m_columnsCount != 0) && ((m_prevIndex+1) % m_columnsCount != 0)) {

        beginMoveRows(QModelIndex(), index, index, QModelIndex(), m_prevIndex);
        qSwap(m_model[index], m_model[m_prevIndex]);
        endMoveRows();

        m_movesCount += 1;

        if(anyMatch()) {

            doSMTH();

            beginMoveRows(QModelIndex(), index, index, QModelIndex(), m_prevIndex);
            qSwap(m_model[index], m_model[m_prevIndex]);
            endMoveRows();

            m_movesCount -= 1;
        }

        emit movesCountChanged();

        m_startCount = true;

        boardInitialization();
    }
}

void Engine::moveLeft(int index) {

    if(((index-1) % m_columnsCount != 0) && (m_prevIndex % m_columnsCount != 0)) {

        beginMoveRows(QModelIndex(), m_prevIndex, m_prevIndex, QModelIndex(), index);
        qSwap(m_model[index], m_model[m_prevIndex]);
        endMoveRows();

        m_movesCount += 1;

        if(anyMatch())  {

            doSMTH();

            beginMoveRows(QModelIndex(), m_prevIndex, m_prevIndex, QModelIndex(), index);
            qSwap(m_model[index], m_model[m_prevIndex]);
            endMoveRows();

            m_movesCount -= 1;
        }

        emit movesCountChanged();

        m_startCount = true;

        boardInitialization();
    }
}

void Engine::moveUp(int index) {

    beginMoveRows(QModelIndex(), m_prevIndex, m_prevIndex, QModelIndex(), index);
    qSwap(m_model[index], m_model[m_prevIndex]);
    endMoveRows();

    beginMoveRows(QModelIndex(), index, index, QModelIndex(), m_prevIndex);
    endMoveRows();

    m_movesCount += 1;

    if(anyMatch())  {

        doSMTH();

        beginMoveRows(QModelIndex(), m_prevIndex, m_prevIndex, QModelIndex(), index);
        qSwap(m_model[index], m_model[m_prevIndex]);
        endMoveRows();

        beginMoveRows(QModelIndex(), index, index, QModelIndex(), m_prevIndex);
        endMoveRows();

        m_movesCount -= 1;
    }

    emit movesCountChanged();

    m_startCount = true;

    boardInitialization();
}

void Engine::moveDown(int index) {

    beginMoveRows(QModelIndex(), index, index, QModelIndex(), m_prevIndex);
    qSwap(m_model[index], m_model[m_prevIndex]);
    endMoveRows();

    beginMoveRows(QModelIndex(), m_prevIndex, m_prevIndex, QModelIndex(), index);
    endMoveRows();

    m_movesCount += 1;
    if(anyMatch())  {

        doSMTH();

        beginMoveRows(QModelIndex(), index, index, QModelIndex(), m_prevIndex);
        qSwap(m_model[index], m_model[m_prevIndex]);
        endMoveRows();

        beginMoveRows(QModelIndex(), m_prevIndex, m_prevIndex, QModelIndex(), index);
        endMoveRows();

        m_movesCount -= 1;
    }

    emit movesCountChanged();

    m_startCount = true;

    boardInitialization();
}

void Engine::ifAnyMatch() {

    if(!anyMatch()) {

        for(const int i : m_indexes) {
            beginRemoveRows(QModelIndex(), i, i);
            endRemoveRows();

            m_model[i] = "";

            beginInsertRows(QModelIndex(), i, i);
            endInsertRows();

            if(m_startCount)
                m_scoreCount += m_elementScore;
        }
        emit scoreCountChanged();
        boardInitialization();
    }
}

void Engine::fallDown() {

    for(int j = 0; j < m_model.length()-m_columnsCount; j+=m_columnsCount) {
        bool check = false;

        for(int i = j; i < m_columnsCount+j; i++) {
            if(m_model[i] == "") {
                checkUpperIndex(i);
                check = true;
            }
        }
        if(check)
            return;
    }

//    if([this]()-> bool {
//            for(int i = this->model.length()-this->columnsCount; i < this->model.length(); i++)
//                return this->model[i] == "" ? true : false;
//        })

    delayAddElement->start(50);

}

void Engine::checkUpperIndex(int index) {

    for(int i = index+m_columnsCount; i < m_model.length(); i+= m_columnsCount) {

        if(m_model[i] != "") {

            int j = i;
            m_myIndex = j;

//            while(j != index) {
                beginMoveRows(QModelIndex(), j-m_columnsCount, j-m_columnsCount, QModelIndex(), j);
                qSwap(m_model[j], m_model[j-m_columnsCount]);
                endMoveRows();

                beginMoveRows(QModelIndex(), j, j, QModelIndex(), j-m_columnsCount);
                endMoveRows();

//                j-=columnsCount;
//            }
            return;
        }
    }
    if(m_model[m_myIndex - m_columnsCount] == "")
        checkUpperIndex(m_myIndex-m_columnsCount);
}

void Engine::addElement() {

    for(int i = m_model.length()-m_columnsCount; i < m_model.length(); i++) {

        if(m_model[i] == "") {
            int temp;
            temp = rand() % (m_elements.length());

            beginRemoveRows(QModelIndex(), i, i);
            endRemoveRows();

            QString elementName = "%1.png";
            m_model[i] = elementName.arg(temp);

            beginInsertRows(QModelIndex(), i, i);
            endInsertRows();
        }
    }

    for(const QString list : m_model)
        if(list == "") {
            fallDown();
            return;
        }
}

void Engine::swap(int index) {

    if(m_prevIndex == -1) {
        m_prevIndex = index;
    }
    else {
        if(m_prevIndex + 1 == index)
            moveRight(index);

        if(m_prevIndex - 1 == index)
            moveLeft(index);

        if(m_prevIndex + m_columnsCount == index)
            moveUp(index);

        if(m_prevIndex - m_columnsCount == index)
            moveDown(index);

        m_prevIndex = -1;
        m_currentIndex = -1;
        emit currentIndexChanged();
    }
}

void Engine::resetBoard()
{
    m_scoreCount = 0;
    m_movesCount = 0;
    emit movesCountChanged();
    emit scoreCountChanged();

    if(m_model.length() == 0) {
        for(int i = 0; i < m_columnsCount*m_rowsCount; i++) {
            beginRemoveRows(QModelIndex(), i, i);
            endRemoveRows();

            m_model.append("");

            beginInsertRows(QModelIndex(), i, i);
            endInsertRows();
        }
    }
    else {
        for(int i = 0; i < m_columnsCount*m_rowsCount; i++) {
            beginRemoveRows(QModelIndex(), i, i);
            endRemoveRows();

            m_model[i] = "";

            beginInsertRows(QModelIndex(), i, i);
            endInsertRows();
        }
    }
}

QVariant Engine::data(const QModelIndex &index, int role) const {

    if(!index.isValid())
        return QVariant();
    if(role == Path)
        return QVariant(m_model[index.row()]);
    if(role == ModelSize)
        return QVariant(m_model.length());
    return QVariant();
}

QHash<int, QByteArray> Engine::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[Path] = "path";
    roles[ModelSize] = "modelSize";
    return roles;
}
int Engine::currentIndex() const {
    return m_currentIndex;
}
void Engine::setCurrentIndex(int currentIndex) {
    m_currentIndex = currentIndex;
}

int Engine::columnsCount() const {
    return m_columnsCount;
}
void Engine::setColumnsCount(int columnsCount) {
    m_columnsCount = columnsCount;
}

int Engine::rowsCount() const {
    return m_rowsCount;
}
void Engine::setRowsCount(int rowsCount) {
    m_rowsCount = rowsCount;
}

int Engine::scoreCount() const {
    return m_scoreCount;
}
void Engine::setScoreCount(int scoreCount) {
    m_scoreCount = scoreCount;
}

int Engine::movesCount() const {
    return m_movesCount;
}
void Engine::setMovesCount(int movesCount) {
    m_movesCount = movesCount;
}

int Engine::maxMoves() const
{
    return m_maxMoves;
}

int Engine::minScore() const
{
    return m_minScore;
}

bool Engine::startCount() const
{
    return m_startCount;
}

void Engine::setStartCount(bool startCount)
{
    m_startCount = startCount;
}

int Engine::rowCount(const QModelIndex &) const {
    return m_model.size();
}

void Engine::rowMatch() {

    for(int j = 0; j < m_model.length()-m_columnsCount; j+=m_columnsCount) {

        QList<int> temp;
        temp.append(j);

        for(int i = j; i < m_columnsCount+j-1; i++) {

            if((m_model[i] != "") && (m_model[i] == m_model[i+1])) {
                temp.append(i+1);
            }
            else {
                if(temp.length() >= 3) {
                    for(const int tmp : temp)
                        m_indexes.append(tmp);
                }
                temp.clear();
                temp.append(i+1);
            }
        }
        if(temp.length() >= 3) {
            for(const int tmp : temp)
                m_indexes.append(tmp);
            temp.clear();
        }
    }
}

void Engine::columnMatch() {

    for(int j = 0; j < m_columnsCount; j++) {

        QList<int> temp;
        temp.append(j);

        for(int i = j; i < m_model.length() - m_columnsCount*2; i+=m_columnsCount) {

            if((m_model[i] != "") && (m_model[i] == m_model.at(i+m_columnsCount))) {
                temp.append(i+m_columnsCount);
            }
            else {
                if(temp.length() >= 3) {
                    for(const int tmp : temp)
                        m_indexes.append(tmp);
                }
                temp.clear();
                temp.append(i+m_columnsCount);
            }
        }

        if(temp.length() >= 3) {
            for(const int tmp : temp)
                m_indexes.append(tmp);
            temp.clear();
        }
    }
}

bool Engine::anyMatch() {

    m_indexes.clear();

    rowMatch();
    columnMatch();

    return (m_indexes.length() == 0
            ? true
            : false);
}
