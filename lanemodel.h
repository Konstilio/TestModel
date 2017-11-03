#ifndef BOARDMODEL_H
#define BOARDMODEL_H

#include <memory>
#include <QAbstractItemModel>
#include <QString>
#include "boardmodelitem.h"
#include "boardroles.h"

using namespace std;

class ColumnsModel;
class BoardProxyModel;
class LaneModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(int visualWidth READ visualWidth NOTIFY visualWidthChanged)

public:

    LaneModel(QObject* _pParent = Q_NULLPTR);
    virtual ~LaneModel();

    static QString fs_Format();

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &_Index) const override;
    int rowCount(QModelIndex const& _Parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &_Parent = QModelIndex()) const override;
    QVariant data(QModelIndex const& _Index, int _Role = Qt::DisplayRole) const override;

    QModelIndex fr_GetIndexForItem(BoardModelItem *_pItem) const;

    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE void append();
    Q_INVOKABLE void appendColumn(int _iParentLaneRow);

    int visualWidth() const;
    BoardModelItem *f_GetModelItemForID(CPMC_LocalID const &_LocalID) const;
    QModelIndex f_GetModelIndexForID(CPMC_LocalID const &_LocalID) const;

    //#TODO_BOARDS: Refactor it to take a range of cards
    void f_CardAdded(CPMC_LocalID const &_LocalLaneID, CPMC_LocalID const &_LocalColumnID, CPMC_LocalID const &_LocalCardID, CPMC_LocalID const &_LocalPreviousCardID);
    void f_CardDeleted(CPMC_LocalID const &_LocalTaskID);
    void f_CardChanged(CPMC_LocalID const &_LocalTaskID);
    //#TODO_BOARDS: Refactor it to take a range of cards
    void f_CardMoved(CPMC_LocalID const &_LocalTaskID, CPMC_LocalID const &_LocalLaneID, CPMC_LocalID const &_LocalColumnID, CPMC_LocalID const &_LocalPreviousTaskID);
    void f_LaneAdded(CPMC_LocalID const &_LocalLaneID, CPMC_LocalID const &_LocalPreviousLaneID);
    void f_LaneMoved(CPMC_LocalID const &_LocalLaneID, CPMC_LocalID const &_LocalPreviousLaneID);
    void f_LaneDeleted(CPMC_LocalID const &_LocalLaneID);
    void f_ColumnAdded(CPMC_LocalID const &_ParentLaneID, CPMC_LocalID const &_LocalColumnID, CPMC_LocalID const &_LocalPreviousColumnID);
    void f_ColumnMoved(CPMC_LocalID const &_ParentLaneID, CPMC_LocalID const &_LocalColumnID, CPMC_LocalID const &_LocalPreviousColumnID);
    void f_ColumnDeleted(CPMC_LocalID const &_ParentLaneID, CPMC_LocalID const &_LocalColumnID);

signals:
    void visualWidthChanged();

protected:

    BoardModelItem* fp_CreateCard(CPMC_LocalID const &_CardID, BoardModelItem* _pParent);
    BoardModelItem* fp_CreateColumn(CPMC_LocalID const &_ColumnID, BoardModelItem* _pParent);
    BoardModelItem* fp_CreateLane(CPMC_LocalID const &_LaneID);

    BoardModelItem *fp_GetColumnInLane(CPMC_LocalID const &_LocalLaneID, CPMC_LocalID const &_LocalColumnID) const;

    void fp_AddCard(int _Lane, int _Column, int _iIndex, unique_ptr<BoardModelItem>&& _pCardItem);
    void fp_AddColumn(int _Lane, int _iIndex, unique_ptr<BoardModelItem>&& _pColumnItem);
    void fp_AddLane(int _iIndex, unique_ptr<BoardModelItem>&& _pLaneItem);

    void fp_RemoveCard(BoardModelItem* _pCardItem);
    void fp_RegisterProxyModel(BoardModelItem *_pItem);

    void fp_ResetCache();

    unique_ptr<BoardModelItem> mp_pRoot;
    QHash<CPMC_LocalID, BoardModelItem*> mp_ItemLookupCache;
    QHash<CPMC_LocalID, BoardProxyModel*> mp_ProxyModelsCache;
    bool mp_bInReset;
};

#endif // BOARDMODEL_H
