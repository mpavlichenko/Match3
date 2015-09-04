#ifndef ENGINE_H
#define ENGINE_H
#include <QObject>
#include <QAbstractListModel>
#include <QList>
#include <QTimer>

class Engine : public QAbstractListModel {
    Q_OBJECT

    Q_PROPERTY(int thisIndex READ currentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(int columnsCount READ columnsCount NOTIFY columnsCountChanged)
    Q_PROPERTY(int rowsCount READ rowsCount NOTIFY rowsCountChanged)
    Q_PROPERTY(int movesCount READ movesCount WRITE setMovesCount NOTIFY movesCountChanged)
    Q_PROPERTY(int scoreCount READ scoreCount WRITE setScoreCount NOTIFY scoreCountChanged)
    Q_PROPERTY(int minScore READ minScore NOTIFY minScoreChanged)
    Q_PROPERTY(int maxMoves READ maxMoves NOTIFY maxMovesChanged)
    Q_PROPERTY(int startCount READ startCount WRITE setStartCount NOTIFY startCountChanged)
    Q_PROPERTY(int isVictory READ isVictory WRITE setIsVictory NOTIFY isVictoryChanged)

public:
    explicit Engine(QObject *parent = nullptr);
    ~Engine();

    Q_INVOKABLE void swap(int index);
    Q_INVOKABLE void resetBoard();

    enum Roles {
        Path = Qt::UserRole + 1,
        ModelSize = Qt::UserRole + 2,
        };

    virtual int rowCount(const QModelIndex&) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    QHash<int, QByteArray> roleNames() const;

    int currentIndex() const;
    void setCurrentIndex(int currentIndex);

    int columnsCount() const;
    void setColumnsCount(int columnsCount);

    int rowsCount() const;
    void setRowsCount(int rowsCount);

    int scoreCount() const;
    void setScoreCount(int scoreCount);

    int movesCount() const;
    void setMovesCount(int movesCount);

    int maxMoves() const;

    int minScore() const;

    bool startCount() const;
    void setStartCount(bool startCount);

    bool isVictory() const;
    void setIsVictory(bool isVictory);

private:
    int m_columnsCount;
    int m_rowsCount;
    int m_elementScore;
    int m_minScore;
    int m_maxMoves;
    int m_currentIndex;
    int m_myIndex;
    int m_scoreCount;
    int m_movesCount;
    bool m_startCount;
    bool m_isVictory;
    int m_prevIndex;

    QTimer *delayIfAnyMatch;
    QTimer *delayAddElement;

    QList<int> m_elements;
    QList<int> m_boxes;
    QList<QString> m_model;
    QList<int> m_indexes;

    bool loadSettings();
    void readSettings(const QJsonObject &json);
    void boardInitialization();
    void moveRight(int index);
    void moveLeft(int index);
    void moveDown(int index);
    void moveUp(int index);
    void checkUpperIndex(int index);
    void rowMatch();
    void columnMatch();
    bool anyMatch();
    void fallDown();
    void victory();

signals:
    void currentIndexChanged();
    void columnsCountChanged();
    void rowsCountChanged();
    void movesCountChanged();
    void scoreCountChanged();
    void minScoreChanged();
    void maxMovesChanged();
    void startCountChanged();
    void isVictoryChanged();

public slots:
    void delay();
    void ifAnyMatch();
    void addElement();
};

#endif // ENGINE_H
