#include "boardmodelitem.h"

BoardModelItem::BoardModelItem(TypeClass::EType _Type, CPMC_LocalID _Id, QString _Title)
    : mp_pParent(nullptr)
    , mp_Type(_Type)
    , mp_ID(_Id)
    , mp_Title(_Title)
{

}

TypeClass::EType BoardModelItem::type() const
{
    return mp_Type;
}

BoardModelItem* BoardModelItem::parent() const
{
    return mp_pParent;
}

void BoardModelItem::setParent(BoardModelItem *_pParent)
{
    mp_pParent = _pParent;
}

int BoardModelItem::childCount() const
{
    return static_cast<int>(mp_Children.size());
}

int BoardModelItem::rowForChild(BoardModelItem const* _pChild) const
{
    for (int i = 0; i < mp_Children.size(); ++i)
    {
        if (mp_Children[i].get() == _pChild)
            return i;
    }
    return -1;
}

int BoardModelItem::rowInParent() const
{
    if (!mp_pParent)
        return 0;

    int Row = mp_pParent->rowForChild(this);
    return Row;
}

BoardModelItem* BoardModelItem::child(int _iIndex) const
{
    return mp_Children[_iIndex].get();
}

void BoardModelItem::addChild(unique_ptr<BoardModelItem>&& _pChild)
{
    _pChild->setParent(this);
    mp_Children.push_back(std::move(_pChild));
}

void BoardModelItem::insertChild(int _iIndex, unique_ptr<BoardModelItem>&& _pChild)
{
    _pChild->setParent(this);

    if (_iIndex >= mp_Children.size())
    {
        mp_Children.push_back(std::move(_pChild));
    }
    else
    {
        auto pos = mp_Children.begin() + _iIndex;
        mp_Children.insert(pos, std::move(_pChild));
    }
}

void BoardModelItem::removeChild(int _iIndex)
{
    auto pos = mp_Children.begin() + _iIndex;
    mp_Children.erase(pos);
}

BoardModelItem* BoardModelItem::detachChild(int _iIndex)
{
    BoardModelItem *pItem = mp_Children[_iIndex].release();
    removeChild(_iIndex);
    return pItem;
}

const unique_ptr<BoardModelItem> &BoardModelItem::f_LastChild() const
{
    return mp_Children.back();
}

QString BoardModelItem::title() const
{
    return mp_Title;
}


QColor BoardModelItem::color() const
{
    return QColor(0, 0, 0);
}

CPMC_LocalID BoardModelItem::identifier() const
{
    return mp_ID;
}

vector<unique_ptr<BoardModelItem>> const& BoardModelItem::f_GetChildren() const
{
    return mp_Children;
}


