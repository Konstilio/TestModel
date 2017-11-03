#include "columnsmodel.h"
#include "cardsmodel.h"
#include "boardmodelitem.h"

ColumnsModel::ColumnsModel(BoardModelItem *_pRoot, QObject *_pParent)
    : QAbstractListModel(_pParent)
    , mp_pRoot(_pRoot)
{
    Q_ASSERT(mp_pRoot->type() == TypeClass::EType_Lane);
}

int ColumnsModel::rowCount(const QModelIndex &_Parent) const
{
    if (_Parent.isValid())
       return 0;

    return mp_pRoot->childCount();
}

QVariant ColumnsModel::data(const QModelIndex &_iIndex, int _Role) const
{
    if (!_iIndex.isValid())
        return QVariant();

    BoardModelItem *pItem = mp_pRoot->f_GetChildren()[_iIndex.row()].get();
    Q_ASSERT(pItem != Q_NULLPTR);
    Q_ASSERT(pItem->type() == TypeClass::EType_Column);

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
//        case ERole_Card:
//            {
//                auto *pCardsModel = mp_CardsModelsCache[pItem->identifier()];
//                return QVariant::fromValue(pCardsModel);
//            }
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> ColumnsModel::roleNames() const
{
    QHash<int, QByteArray> Result;
    Result[ERole_Type] = "type";
    Result[ERole_Title] = "title";
    Result[ERole_Color] = "color";
    Result[ERole_Identifier] = "identifier";
    return Result;
}

int ColumnsModel::visualHeight() const
{
    auto const &Children = mp_pRoot->f_GetChildren();
    int Height = 0;
    for (auto const &Child : Children)
        Height = std::max(Height, Child->childCount());
    return Height;
}

void ColumnsModel::f_OnColumnAdded(BoardModelItem *_pItem)
{
    Q_ASSERT(_pItem->type() == TypeClass::EType_Column);

    CardsModel *pModel = new CardsModel(_pItem, this);
    mp_CardsModelsCache[_pItem->identifier()] = pModel;

    emit visualHeightChanged();
}

void ColumnsModel::f_OnColumnRemoved(BoardModelItem *_pItem)
{
    Q_ASSERT(_pItem->type() == TypeClass::EType_Column);
    auto *pModel = mp_CardsModelsCache[_pItem->identifier()];
    pModel->deleteLater();
    mp_CardsModelsCache.remove(_pItem->identifier());
}
