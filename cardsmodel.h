#ifndef CARDSMODEL_H
#define CARDSMODEL_H

#include <QAbstractListModel>
class BoardModelItem;
class CardsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit CardsModel(BoardModelItem *_pRoot, QObject *_pParent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    BoardModelItem *mp_pRoot;
};

#endif // CARDSMODEL_H
