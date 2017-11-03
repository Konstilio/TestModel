#ifndef TESTITEM_H
#define TESTITEM_H

#include <vector>
#include <memory>
#include <QString>
#include <QColor>
#include <QObject>
#include <QtQml>
#include "localidgenerator.h"
using namespace std;


class TypeClass : public QObject
{
    Q_OBJECT

public:

    TypeClass() : QObject() {}

    enum EType
    {
        EType_Board				// Board (root)
        , EType_Lane			// Lane
        , EType_Column			// Column
        , EType_Card			// Card
    };
    Q_ENUMS(EType);

    static void registerQML() {
        qmlRegisterType<TypeClass>("Boards", 1, 0, "Type");
    }


};

class BoardModelItem
{
public:

    BoardModelItem(TypeClass::EType _Type, CPMC_LocalID _Id, QString _Title);
    ~BoardModelItem() = default;

    TypeClass::EType type() const;

    void setParent(BoardModelItem *_pParent);
    BoardModelItem* parent() const;
    int childCount() const;
    int rowInParent() const;
    int rowForChild(BoardModelItem const *_pChild) const;
    BoardModelItem *child(int _iIndex) const;
    void addChild(unique_ptr<BoardModelItem>&& _pChild);
    void insertChild(int _iIndex, unique_ptr<BoardModelItem> &&_pChild);
    void removeChild(int _Index);
    BoardModelItem* detachChild(int _Index);
    unique_ptr<BoardModelItem> const &f_LastChild() const;

    vector<unique_ptr<BoardModelItem>> const &f_GetChildren() const;

    QString title() const;
    CPMC_LocalID identifier() const;
    QColor color() const;

protected:

        BoardModelItem *mp_pParent;
    TypeClass::EType mp_Type;
    CPMC_LocalID mp_ID;
    QString mp_Title;
    vector<unique_ptr<BoardModelItem>> mp_Children;
};

#endif // TESTITEM_H
