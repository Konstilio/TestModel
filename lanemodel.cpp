#include "lanemodel.h"
#include "columnsmodel.h"

LaneModel::LaneModel(QObject* _pParent)
    : QAbstractListModel(_pParent)
    , mp_bInReset(false)
{
    mp_pRoot = make_unique<BoardModelItem>(TypeClass::EType_Board, 0, "Board");
}

LaneModel::~LaneModel()
{

}

int LaneModel::rowCount(QModelIndex const& _Parent) const
{
    if (_Parent.isValid())
       return 0;

    return mp_pRoot->childCount();
}

QVariant LaneModel::data(QModelIndex const& _Index, int _Role) const
{
    if (!_Index.isValid())
        return QVariant();

    BoardModelItem* pItem = mp_pRoot->f_GetChildren()[_Index.row()].get();
    Q_ASSERT(pItem != Q_NULLPTR);
    Q_ASSERT(pItem->type() == TypeClass::EType_Lane);

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
        case ERole_Column:
            {
                auto *pColumnsModel = mp_ColumnModelsCache[pItem->identifier()];
                return QVariant::fromValue(pColumnsModel);
            }
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> LaneModel::roleNames() const
{
    QHash<int, QByteArray> Result;
    Result[ERole_Type] = "type";
    Result[ERole_Title] = "title";
    Result[ERole_Color] = "color";
    Result[ERole_Identifier] = "identifier";
    Result[ERole_Column] = "columnsModel";
    return Result;
}

int LaneModel::visualWidth() const
{
    auto const &Children = mp_pRoot->f_GetChildren();
    int Width = 0;
    for (auto const &Child : Children)
        Width = std::max(Width, Child->childCount());
    return Width;
}

void LaneModel::appendLane()
{
    if (mp_pRoot->childCount() > 0)
    {
        auto const &LastLane = mp_pRoot->f_LastChild();
        Q_ASSERT(LastLane->type() == TypeClass::EType_Lane);
        f_LaneAdded(LocalIDGenerator::f_NextLaneID(), LastLane->identifier());
    }
    else
        f_LaneAdded(LocalIDGenerator::f_NextLaneID(), -1);
}

void LaneModel::appendColumn(int _iParentLaneRow)
{
    BoardModelItem* pLaneItem = mp_pRoot->f_GetChildren()[_iParentLaneRow].get();
    Q_ASSERT(pLaneItem != nullptr);
    Q_ASSERT(pLaneItem->type() == TypeClass::EType_Lane);

    if (pLaneItem->childCount() > 0)
    {
        auto const &LastColumn = pLaneItem->f_LastChild();
        Q_ASSERT(LastColumn->type() == TypeClass::EType_Column);
        f_ColumnAdded(pLaneItem->identifier(), LocalIDGenerator::f_NextColumnID(), LastColumn->identifier());
    }
    else
        f_ColumnAdded(pLaneItem->identifier(), LocalIDGenerator::f_NextColumnID(), -1);
}

void LaneModel::f_CardChanged(CPMC_LocalID const &_LocalTaskID)
{
    BoardModelItem* pCardItem = mp_ItemLookupCache[_LocalTaskID];
    Q_ASSERT(pCardItem != nullptr);
    Q_ASSERT(pCardItem->type() == TypeClass::EType_Card);

    BoardModelItem* pColumn = pCardItem->parent();
    BoardModelItem* pLane = pColumn->parent();

    QModelIndex LaneIndex = index(pLane->rowInParent(), 0, QModelIndex());
    QModelIndex ColumnIndex = index(pColumn->rowInParent(), 0, LaneIndex);
    QModelIndex CardIndex = index(pCardItem->rowInParent(), 0, ColumnIndex);

    dataChanged(CardIndex, CardIndex);
}

void LaneModel::f_CardMoved(CPMC_LocalID const &_LocalTaskID, CPMC_LocalID const &_LocalLaneID, CPMC_LocalID const &_LocalColumnID, CPMC_LocalID const &_LocalPreviousTaskID)
{
    BoardModelItem* pCardItem = f_GetModelItemForID(_LocalTaskID);
    Q_ASSERT(pCardItem != nullptr);
    Q_ASSERT(pCardItem->type() == TypeClass::EType_Card);

    BoardModelItem* pSourceColumn = pCardItem->parent();
    Q_ASSERT(pSourceColumn != nullptr);
    Q_ASSERT(pSourceColumn->type() == TypeClass::EType_Column);

    BoardModelItem* pSourceLane = pSourceColumn->parent();
    Q_ASSERT(pSourceLane != nullptr);
    Q_ASSERT(pSourceLane->type() == TypeClass::EType_Lane);

    BoardModelItem *pNewColumn = fp_GetColumnInLane(_LocalLaneID, _LocalColumnID);
    Q_ASSERT(pNewColumn && pNewColumn->type() == TypeClass::EType_Column);

    BoardModelItem* pNewLane = f_GetModelItemForID(_LocalLaneID);
    Q_ASSERT(pNewLane && pNewLane->type() == TypeClass::EType_Lane);

    QModelIndex SourceLaneIndex = index(pSourceLane->rowInParent(), 0, QModelIndex());
    QModelIndex SourceColumnIndex = index(pSourceColumn->rowInParent(), 0, SourceLaneIndex);
    QModelIndex SourceIndex = index(pCardItem->rowInParent(), 0, SourceColumnIndex);

    QModelIndex TargetLaneIndex = index(pNewLane->rowInParent(), 0, QModelIndex());
    QModelIndex TargetIndex = index(pNewColumn->rowInParent(), 0, TargetLaneIndex);

    int InsertAtRow = 0;
    BoardModelItem* pPrevCardItem = nullptr;

    if (_LocalPreviousTaskID != -1)
    {
        pPrevCardItem = f_GetModelItemForID(_LocalPreviousTaskID);
        Q_ASSERT(pPrevCardItem && pPrevCardItem->type() == TypeClass::EType_Card);

        InsertAtRow = pPrevCardItem->rowInParent() + 1;
    }

    Q_ASSERT(pPrevCardItem != pCardItem);

    if (!beginMoveRows(SourceColumnIndex, SourceIndex.row(), SourceIndex.row(), TargetIndex, InsertAtRow))
        return;

    BoardModelItem* pColumn = pCardItem->parent();
    pCardItem = pColumn->detachChild(pCardItem->rowInParent());

    if (pPrevCardItem)
        pNewColumn->insertChild(pPrevCardItem->rowInParent() + 1, (unique_ptr<BoardModelItem>(pCardItem)));
    else
    {
        if (pNewColumn->f_GetChildren().empty())
            pNewColumn->addChild(unique_ptr<BoardModelItem>(pCardItem));
        else
            pNewColumn->insertChild(0, unique_ptr<BoardModelItem>(pCardItem));
    }

    endMoveRows();
}

void LaneModel::f_CardAdded(CPMC_LocalID const &_LocalLaneID, CPMC_LocalID const &_LocalColumnID, CPMC_LocalID const &_LocalCardID, CPMC_LocalID const &_LocalPreviousCardID)
{
    BoardModelItem* pLaneItem = f_GetModelItemForID(_LocalLaneID);
    Q_ASSERT(pLaneItem && pLaneItem->type() == TypeClass::EType_Lane);

    BoardModelItem* pColumn = fp_GetColumnInLane(_LocalLaneID, _LocalColumnID);
    Q_ASSERT(pColumn && pColumn->type() == TypeClass::EType_Column);

    BoardModelItem* pPrevItem = nullptr;
    if (_LocalPreviousCardID != -1)
    {
        pPrevItem = f_GetModelItemForID(_LocalPreviousCardID);
        Q_ASSERT(pPrevItem && pPrevItem->type() == TypeClass::EType_Card);
    }

    unique_ptr<BoardModelItem> pNewCard = unique_ptr<BoardModelItem>(fp_CreateCard(_LocalCardID, pColumn));
    fp_AddCard(pColumn->parent()->rowInParent(), pColumn->rowInParent(), pPrevItem ? pPrevItem->rowInParent() + 1 : static_cast<int>(pColumn->f_GetChildren().size()), std::move(pNewCard));
}

void LaneModel::f_CardDeleted(CPMC_LocalID const &_LocalTaskID)
{
    BoardModelItem* pCardItem = mp_ItemLookupCache[_LocalTaskID];
    Q_ASSERT(pCardItem && pCardItem->type() == TypeClass::EType_Card);

    fp_RemoveCard(pCardItem);
}

void LaneModel::f_LaneAdded(CPMC_LocalID const &_LocalLaneID, CPMC_LocalID const &_LocalPreviousLaneID)
{
    Q_ASSERT(_LocalLaneID != -1);

    BoardModelItem *pPrevLaneItem = nullptr;
    if (_LocalPreviousLaneID != -1)
    {
        pPrevLaneItem = f_GetModelItemForID(_LocalPreviousLaneID);
        Q_ASSERT(pPrevLaneItem && pPrevLaneItem->type() == TypeClass::EType_Lane);
    }

    // Create empty lane
    unique_ptr<BoardModelItem> pNewLane = unique_ptr<BoardModelItem>(fp_CreateLane(_LocalLaneID));
    fp_AddLane(pPrevLaneItem ? pPrevLaneItem->rowInParent() + 1 : 0, std::move(pNewLane));
}

void LaneModel::f_LaneMoved(CPMC_LocalID const &_LocalLaneID, CPMC_LocalID const &_LocalPreviousLaneID)
{
    Q_ASSERT(_LocalLaneID != -1);
    Q_ASSERT(_LocalLaneID != _LocalPreviousLaneID);

    BoardModelItem *pSourceLane = f_GetModelItemForID(_LocalLaneID);
    Q_ASSERT(pSourceLane && pSourceLane->type() == TypeClass::EType_Lane);

    QModelIndex SourceLaneIndex = index(pSourceLane->rowInParent(), 0, QModelIndex());

    int InsertAtRow = 0;
    BoardModelItem *pPrevLaneItem = nullptr;
    if (_LocalPreviousLaneID != -1)
    {
        pPrevLaneItem = f_GetModelItemForID(_LocalPreviousLaneID);
        Q_ASSERT(pPrevLaneItem && pPrevLaneItem->type() == TypeClass::EType_Lane);
        InsertAtRow = pPrevLaneItem->rowInParent() + 1;
    }

    if (!beginMoveRows(QModelIndex(), SourceLaneIndex.row(), SourceLaneIndex.row(), QModelIndex(), InsertAtRow))
        return;

    BoardModelItem *pBoard = pSourceLane->parent();
    Q_ASSERT(pBoard == mp_pRoot.get());
    auto *pLaneItem = pBoard->detachChild(pSourceLane->rowInParent());
    if (pPrevLaneItem)
        pBoard->insertChild(pPrevLaneItem->rowInParent() + 1, unique_ptr<BoardModelItem>(pLaneItem));
    else
    {
        if (pBoard->f_GetChildren().empty())
            pBoard->addChild(unique_ptr<BoardModelItem>(pLaneItem));
        else
            pBoard->insertChild(0, unique_ptr<BoardModelItem>(pLaneItem));
    }

    endMoveRows();
}

void LaneModel::f_LaneDeleted(CPMC_LocalID const &_LocalLaneID)
{
    Q_ASSERT(_LocalLaneID != -1);

    BoardModelItem *pSourceLane = f_GetModelItemForID(_LocalLaneID);
    Q_ASSERT(pSourceLane && pSourceLane->type() == TypeClass::EType_Lane);
    QModelIndex SourceLaneIndex = index(pSourceLane->rowInParent(), 0, QModelIndex());

    BoardModelItem *pBoard = pSourceLane->parent();
    Q_ASSERT(pBoard == mp_pRoot.get());

    beginRemoveRows(QModelIndex(), SourceLaneIndex.row(), SourceLaneIndex.row());
    auto *pColumnsModel = mp_ColumnModelsCache[pSourceLane->identifier()];
    pColumnsModel->deleteLater();
    mp_ColumnModelsCache.remove(pSourceLane->identifier());

    mp_ItemLookupCache.remove(pSourceLane->identifier());
    pBoard->removeChild(pSourceLane->rowInParent());
    endRemoveRows();
}

void LaneModel::f_ColumnAdded(CPMC_LocalID const &_ParentLaneID, CPMC_LocalID const &_LocalColumnID, CPMC_LocalID const &_LocalPreviousColumnID)
{
    Q_ASSERT(_ParentLaneID != -1);
    Q_ASSERT(_LocalColumnID != -1);
    Q_ASSERT(_LocalColumnID != _LocalPreviousColumnID);

    BoardModelItem *pPrevItem = nullptr;

    if (_LocalPreviousColumnID != -1)
    {
        pPrevItem = f_GetModelItemForID(_LocalPreviousColumnID);
        Q_ASSERT(pPrevItem && pPrevItem->type() == TypeClass::EType_Column);
    }

    BoardModelItem *pLaneItem = f_GetModelItemForID(_ParentLaneID);
    Q_ASSERT(pLaneItem && pLaneItem->type() == TypeClass::EType_Lane);
    unique_ptr<BoardModelItem> pNewColumn = unique_ptr<BoardModelItem>(fp_CreateColumn(_LocalColumnID, pLaneItem));
    fp_AddColumn(pLaneItem->rowInParent(), pPrevItem ? pPrevItem->rowInParent() + 1 : 0, std::move(pNewColumn));

    emit visualWidthChanged();
}

void LaneModel::f_ColumnMoved(CPMC_LocalID const &_ParentLaneID, CPMC_LocalID const &_LocalColumnID, CPMC_LocalID const &_LocalPreviousColumnID)
{
    BoardModelItem *pSourceColumn = f_GetModelItemForID(_LocalColumnID);
    Q_ASSERT(pSourceColumn && pSourceColumn->type() == TypeClass::EType_Column);

    BoardModelItem *pParentLane = pSourceColumn->parent();
    Q_ASSERT(pParentLane && pParentLane->type() == TypeClass::EType_Lane && pParentLane->identifier() == _ParentLaneID);

    QModelIndex ParentLaneIndex = index(pParentLane->rowInParent(), 0, QModelIndex());
    QModelIndex SourceColumnIndex = index(pSourceColumn->rowInParent(), 0, ParentLaneIndex);

    int InsertAtRow = 0;
    BoardModelItem *pPrevColumnItem = nullptr;
    if (_LocalPreviousColumnID != -1)
    {
        pPrevColumnItem = f_GetModelItemForID(_LocalPreviousColumnID);
        Q_ASSERT(pPrevColumnItem && pPrevColumnItem->type() == TypeClass::EType_Column && pPrevColumnItem->parent() == pParentLane);
        InsertAtRow = pPrevColumnItem->rowInParent() + 1;
    }

    if (!beginMoveRows(ParentLaneIndex, SourceColumnIndex.row(), SourceColumnIndex.row(), ParentLaneIndex, InsertAtRow))
        return;

    auto *pColumnItem = pParentLane->detachChild(pSourceColumn->rowInParent());
    if (pPrevColumnItem)
        pParentLane->insertChild(pPrevColumnItem->rowInParent() + 1, unique_ptr<BoardModelItem>(pColumnItem));
    else
    {
        if (pParentLane->f_GetChildren().empty())
            pParentLane->addChild(unique_ptr<BoardModelItem>(pColumnItem));
        else
            pParentLane->insertChild(0, unique_ptr<BoardModelItem>(pColumnItem));
    }

    endMoveRows();
}

void LaneModel::f_ColumnDeleted(CPMC_LocalID const &_ParentLaneID, CPMC_LocalID const &_LocalColumnID)
{
    Q_ASSERT(_ParentLaneID != -1 && _LocalColumnID != -1);

    BoardModelItem *pSourceColumn = f_GetModelItemForID(_LocalColumnID);
    Q_ASSERT(pSourceColumn && pSourceColumn->type() == TypeClass::EType_Column);

    BoardModelItem *pParentLane = pSourceColumn->parent();
    Q_ASSERT(pParentLane && pParentLane->type() == TypeClass::EType_Lane && pParentLane->identifier() == _ParentLaneID);

    QModelIndex ParentLaneIndex = index(pParentLane->rowInParent(), 0, QModelIndex());
    QModelIndex SourceColumnIndex = index(pSourceColumn->rowInParent(), 0, ParentLaneIndex);

    beginRemoveRows(ParentLaneIndex, SourceColumnIndex.row(), SourceColumnIndex.row());
    mp_ColumnModelsCache[pParentLane->identifier()]->f_OnColumnRemoved(pSourceColumn);
    mp_ItemLookupCache.remove(pSourceColumn->identifier());
    pParentLane->removeChild(pSourceColumn->rowInParent());
    endRemoveRows();
}

BoardModelItem *LaneModel::fp_CreateCard(const CPMC_LocalID &_CardID, BoardModelItem *_pParent)
{
    BoardModelItem *pCard = new BoardModelItem(TypeClass::EType_Card, _CardID, "Card" + QString::number(_CardID));
    pCard->setParent(_pParent);
    return pCard;
}

BoardModelItem *LaneModel::fp_CreateColumn(const CPMC_LocalID &_ColumnID, BoardModelItem *_pParent)
{
    BoardModelItem *pColumn = new BoardModelItem(TypeClass::EType_Column, _ColumnID, "Column" + QString::number(_ColumnID));
    pColumn->setParent(_pParent);
    return pColumn;
}

BoardModelItem *LaneModel::fp_CreateLane(const CPMC_LocalID &_LaneID)
{
    BoardModelItem *pLane = new BoardModelItem(TypeClass::EType_Lane, _LaneID, "Lane" + QString::number(_LaneID));
    pLane->setParent(mp_pRoot.get());
    return pLane;
}

BoardModelItem *LaneModel::f_GetModelItemForID(CPMC_LocalID const &_LocalID) const
{
    if (!mp_ItemLookupCache.contains(_LocalID))
        return nullptr;

    return mp_ItemLookupCache[_LocalID];
}

QModelIndex LaneModel::f_GetModelIndexForID(CPMC_LocalID const &_LocalID) const
{
    BoardModelItem* pItem = f_GetModelItemForID(_LocalID);
    if (!pItem)
        return QModelIndex();

    QModelIndex ParentIndex;
    BoardModelItem* pParentItem = pItem->parent();
    if (pParentItem)
        ParentIndex = f_GetModelIndexForID(pParentItem->identifier());

    return index(pItem->rowInParent(), 0, ParentIndex);
}

QString LaneModel::fs_Format()
{
    return "application/boardmodeldata";
}

//
// BoardModel protected methods
//

void LaneModel::fp_AddCard(int _Lane, int _Column, int _iIndex, unique_ptr<BoardModelItem>&& _pCardItem)
{
    BoardModelItem* pLaneItem = mp_pRoot->child(_Lane);
    BoardModelItem* pColumnItem = pLaneItem->child(_Column);

    if (!mp_bInReset)
    {
        QModelIndex LaneIndex = index(_Lane, 0, QModelIndex());
        QModelIndex ColumnIndex = index(_Column, 0, LaneIndex);

        beginInsertRows(ColumnIndex, _iIndex, _iIndex);
    }

    mp_ItemLookupCache[_pCardItem->identifier()] = _pCardItem.get();
    pColumnItem->insertChild(_iIndex, std::move(_pCardItem));

    if (!mp_bInReset)
        endInsertRows();
}

void LaneModel::fp_AddColumn(int _Lane, int _iIndex, unique_ptr<BoardModelItem>&& _pColumnItem)
{
    BoardModelItem* pLaneItem = mp_pRoot->child(_Lane);

    mp_ItemLookupCache[_pColumnItem->identifier()] = _pColumnItem.get();
    mp_ColumnModelsCache[pLaneItem->identifier()]->f_OnColumnAdded(std::move(_pColumnItem), _iIndex);

}

void LaneModel::fp_AddLane(int _iIndex, unique_ptr<BoardModelItem>&& _pLaneItem)
{
    if (!mp_bInReset)
        beginInsertRows(QModelIndex(), _iIndex, _iIndex);

    mp_ItemLookupCache[_pLaneItem->identifier()] = _pLaneItem.get();
    ColumnsModel *pModel = new ColumnsModel(_pLaneItem.get(), this);
    mp_ColumnModelsCache[_pLaneItem->identifier()] = pModel;
    mp_pRoot->insertChild(_iIndex, std::move(_pLaneItem));

    if (!mp_bInReset)
        endInsertRows();
}

void LaneModel::fp_RemoveCard(BoardModelItem *_pCardItem)
{
    Q_ASSERT(_pCardItem && _pCardItem->type() == TypeClass::EType_Card);

    BoardModelItem* pParent = _pCardItem->parent();

    QModelIndex LaneIndex = index(pParent->parent()->rowInParent(), 0, QModelIndex());
    QModelIndex ColIndex = index(pParent->rowInParent(), 0, LaneIndex);
    QModelIndex CardIndex = index(_pCardItem->rowInParent(), 0, ColIndex);

    beginRemoveRows(ColIndex, CardIndex.row(), CardIndex.row());

    mp_ItemLookupCache.remove(_pCardItem->identifier());
    pParent->removeChild(_pCardItem->rowInParent());

    endRemoveRows();
}

BoardModelItem *LaneModel::fp_GetColumnInLane(CPMC_LocalID const &_LocalLaneID, CPMC_LocalID const &_LocalColumnID) const
{
    BoardModelItem* pLaneItem = f_GetModelItemForID(_LocalLaneID);
    Q_ASSERT(pLaneItem && pLaneItem->type() == TypeClass::EType_Lane);

    int ChildCount = static_cast<int>(pLaneItem->f_GetChildren().size());
    for (int iChild = 0; iChild < ChildCount; iChild++)
    {
        BoardModelItem* pColumnItem = pLaneItem->child(iChild);

        Q_ASSERT(pColumnItem && pColumnItem->type() == TypeClass::EType_Column);

        if (pColumnItem->identifier() == _LocalColumnID)
            return pColumnItem;
    }

    return nullptr;
}

void LaneModel::fp_ResetCache()
{
    mp_pRoot = make_unique<BoardModelItem>(TypeClass::EType_Board, 0, "Board");
    mp_ItemLookupCache.clear();
}

