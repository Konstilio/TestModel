#include <QApplication>
#include <QDialog>
#include <QQuickView>
#include <QQmlContext>
#include "lanemodel.h"

int c_Lane = 1;
int c_Col = 100;
int c_Card = 1000;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LaneModel Model;
    int LaneID1 = LocalIDGenerator::f_NextLaneID();
    int LaneID2 = LocalIDGenerator::f_NextLaneID();
    int LaneID3 = LocalIDGenerator::f_NextLaneID();
    Model.f_LaneAdded(LaneID1, -1);
    {
        int ColID1 = LocalIDGenerator::f_NextColumnID();
        int ColID2 = LocalIDGenerator::f_NextColumnID();


        Model.f_ColumnAdded(LaneID1, ColID1, -1);
        {
            int CardID1 = LocalIDGenerator::f_NextCardID();
            int CardID2 = LocalIDGenerator::f_NextCardID();

            Model.f_CardAdded(LaneID1, ColID1, CardID1, -1);
            Model.f_CardAdded(LaneID1, ColID1, CardID2, CardID1);
        }

        Model.f_ColumnAdded(LaneID1, ColID2, ColID1);
        {
            int CardID1 = LocalIDGenerator::f_NextCardID();
            int CardID2 = LocalIDGenerator::f_NextCardID();
            int CardID3 = LocalIDGenerator::f_NextCardID();

            Model.f_CardAdded(LaneID1, ColID2, CardID1, -1);
            Model.f_CardAdded(LaneID1, ColID2, CardID2, CardID1);
            Model.f_CardAdded(LaneID1, ColID2, CardID3, CardID2);
        }
    }

    Model.f_LaneAdded(LaneID2, LaneID1);
    Model.f_LaneAdded(LaneID3, LaneID2);

    QQuickView View;
    View.setResizeMode(QQuickView::SizeRootObjectToView);
    QQmlContext *pContext = View.rootContext();
    pContext->setContextProperty("BoardModel", &Model);
    View.setSource(QUrl("qrc:/Quick/main.qml"));
    View.show();

    return a.exec();
}
