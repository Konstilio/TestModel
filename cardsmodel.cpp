#include "cardsmodel.h"
#include "boardmodelitem.h"
#include "boardroles.h"

CardsModel::CardsModel(BoardModelItem *_pRoot, QObject *_pParent)
    : QAbstractListModel(_pParent)
    , mp_pRoot(_pRoot)
{
    Q_ASSERT(mp_pRoot->type() == TypeClass::EType_Column);
}

int CardsModel::rowCount(const QModelIndex &_Parent) const
{
    if (_Parent.isValid())
       return 0;

    return mp_pRoot->childCount();
}

QVariant CardsModel::data(const QModelIndex &_iIndex, int _Role) const
{
    if (!_iIndex.isValid())
        return QVariant();

    BoardModelItem *pItem = mp_pRoot->f_GetChildren()[_iIndex.row()].get();
    Q_ASSERT(pItem != Q_NULLPTR);
    Q_ASSERT(pItem->type() == TypeClass::EType_Card);

    switch (_Role)
    {
        case ERole_Type:
            return pItem->type();
        case ERole_Title:
            return pItem->title();
        case ERole_Color:
            return pItem->color();
        case ERole_Identifier:
            return pItem->identifier();
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> CardsModel::roleNames() const
{
    QHash<int, QByteArray> Result;
    Result[ERole_Type] = "type";
    Result[ERole_Title] = "title";
    Result[ERole_Color] = "color";
    Result[ERole_Identifier] = "identifier";
    return Result;
}
