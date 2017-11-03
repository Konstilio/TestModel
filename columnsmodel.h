#ifndef COLUMNSMODEL_H
#define COLUMNSMODEL_H

#include <QAbstractListModel>
#include "boardmodelitem.h"
#include "boardroles.h"

class CardsModel;

class ColumnsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int visualHeight READ visualHeight NOTIFY visualHeightChanged)

public:
    explicit ColumnsModel(BoardModelItem *_pRoot, QObject *_pParent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &_Parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &_iIndex, int _Role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    int visualHeight() const;

    void f_OnColumnAdded(BoardModelItem* _pItem);
    void f_OnColumnRemoved(BoardModelItem *_pItem);

signals:
    void visualHeightChanged();

private:
    BoardModelItem *mp_pRoot;
    QHash<CPMC_LocalID, CardsModel*> mp_CardsModelsCache;
};

#endif // COLUMNSMODEL_H
