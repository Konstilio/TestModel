#include "boardproxymodel.h"
#include "boardmodelitem.h"
#include "lanemodel.h"
#include "localidgenerator.h"
#include <QDebug>

BoardProxyModel::BoardProxyModel(BoardModelItem *_pRoot, QObject *_pParent)
    : QAbstractProxyModel(_pParent)
    , mp_pRoot(_pRoot)
{
    Q_ASSERT(mp_pRoot->type() == TypeClass::EType_Lane || mp_pRoot->type() == TypeClass::EType_Column);
}

QModelIndex BoardProxyModel::index(int _Row, int _Column, const QModelIndex &_Parent) const
{
    Q_ASSERT(!_Parent.isValid());
    //if (!hasIndex(_Row, _Column, _Parent))
        //return QModelIndex();

    Q_ASSERT(_Column == 0);
    return createIndex(_Row, _Column, mp_pRoot->f_GetChildren()[_Row].get());
}

QModelIndex BoardProxyModel::parent(const QModelIndex &_Index) const
{
    Q_UNUSED(_Index);
    return QModelIndex();
}

int BoardProxyModel::rowCount(const QModelIndex &_Parent) const
{
    if (_Parent.isValid())
       return 0;

    return mp_pRoot->childCount();
}

int BoardProxyModel::columnCount(const QModelIndex &_Parent) const
{
    Q_UNUSED(_Parent);
    return 1;
}

QModelIndex BoardProxyModel::mapFromSource(const QModelIndex &_SourceIndex) const
{
    if (!_SourceIndex.isValid())
        return QModelIndex();

    BoardModelItem *pItem = static_cast<BoardModelItem *>(_SourceIndex.internalPointer());
    Q_ASSERT(pItem);
    if (pItem->parent() != mp_pRoot)
        return QModelIndex();

    return createIndex(pItem->rowInParent(), 0, pItem);
}

QModelIndex BoardProxyModel::mapToSource(const QModelIndex &_ProxyIndex) const
{
    if (!_ProxyIndex.isValid())
        return QModelIndex();

    BoardModelItem *pItem = static_cast<BoardModelItem *>(_ProxyIndex.internalPointer());
    Q_ASSERT(pItem);
    Q_ASSERT(pItem->parent() == mp_pRoot);

    if (mp_pRoot->type() == TypeClass::EType_Lane)
    {
        Q_ASSERT(pItem->type() == TypeClass::EType_Column);
    }
    else if (mp_pRoot->type() == TypeClass::EType_Column)
    {
        Q_ASSERT(pItem->type() == TypeClass::EType_Card);
    }

    LaneModel *pSourceModel = static_cast<LaneModel *>(sourceModel());
    Q_ASSERT(pSourceModel);
    QModelIndex ParentSourceIndex = pSourceModel->fr_GetIndexForItem(mp_pRoot);
    Q_ASSERT(ParentSourceIndex.isValid() || mp_pRoot->type() == TypeClass::EType_Lane);
    auto Index = pSourceModel->index(_ProxyIndex.row(), 0, ParentSourceIndex);
    auto *pParentItem = static_cast<BoardModelItem*>(ParentSourceIndex.internalPointer());
    Q_ASSERT(pParentItem == mp_pRoot);

    qDebug() << pItem->title()
             << " Source Index row: " << Index.row()
             << " Parent Source Index row: " << ParentSourceIndex.row()
             << " Proxy Index row: " << _ProxyIndex.row();
    ;
    return Index;
}

QHash<int, QByteArray> BoardProxyModel::roleNames() const
{
    LaneModel *pSourceModel = static_cast<LaneModel *>(sourceModel());
    Q_ASSERT(pSourceModel);
    return pSourceModel->roleNames();
}

void BoardProxyModel::append()
{
    LaneModel *pSourceModel = static_cast<LaneModel *>(sourceModel());
    Q_ASSERT(pSourceModel);
    int insertAfter = mp_pRoot->childCount() ? mp_pRoot->f_LastChild()->identifier() : -1;
    if (mp_pRoot->type() == TypeClass::EType_Lane)
    {
        pSourceModel->f_ColumnAdded
            (
                mp_pRoot->identifier()
                , LocalIDGenerator::f_NextColumnID()
                , insertAfter
            )
        ;
    }
    else
    {
        pSourceModel->f_CardAdded
           (
               mp_pRoot->parent()->identifier()
               , mp_pRoot->identifier()
               , LocalIDGenerator::f_NextCardID()
               , insertAfter
            )
        ;
    }
}

int BoardProxyModel::visualHeight() const
{
    Q_ASSERT(mp_pRoot->type() == TypeClass::EType_Lane);

    auto const &Children = mp_pRoot->f_GetChildren();
    int Height = 0;
    for (auto const &Child : Children)
        Height = std::max(Height, Child->childCount());
    return Height;
}

void BoardProxyModel::onSourceRowsInserted(const QModelIndex &_ParentSourceIndex, int _First, int _Last)
{
    auto *pItem = static_cast<BoardModelItem *>(_ParentSourceIndex.internalPointer());
    if (pItem != mp_pRoot)
        return;

    Q_ASSERT(_First == _Last);

    emit layoutChanged({ { index(_First, 0, QModelIndex()) } } );
}

