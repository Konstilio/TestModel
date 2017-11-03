#ifndef BOARDPROXYMODEL_H
#define BOARDPROXYMODEL_H
#include <QAbstractProxyModel>

class BoardModelItem;
class BoardProxyModel : public QAbstractProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int visualHeight READ visualHeight NOTIFY visualHeightChanged)
public:
    BoardProxyModel(BoardModelItem *_pRoot, QObject *_pParent = Q_NULLPTR);
    
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &_Index) const override;
    int rowCount(QModelIndex const& _Parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &_Parent = QModelIndex()) const override;
    QModelIndex	mapFromSource(const QModelIndex &_SourceIndex) const override;
    QModelIndex	mapToSource(const QModelIndex &_ProxyIndex) const override;
    
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void append();

    int visualHeight() const;

public slots:
    void onSourceRowsInserted(QModelIndex const &_ParentSourceIndex, int _First, int _Last);

signals:
    void visualHeightChanged();

private:
    BoardModelItem *mp_pRoot;
};

#endif // BOARDPROXYMODEL_H
