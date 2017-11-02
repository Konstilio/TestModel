#ifndef LOCALIDGENERATOR_H
#define LOCALIDGENERATOR_H

using CPMC_LocalID = int;

class LocalIDGenerator {
public:

    static CPMC_LocalID f_NextLaneID() {
        return f_Instance().fp_NextLaneID();
    }

    static CPMC_LocalID f_NextColumnID() {
        return f_Instance().fp_NextColumnID();
    }

    static CPMC_LocalID f_NextCardID() {
        return f_Instance().fp_NextCardID();
    }


private:
    LocalIDGenerator() = default;

    CPMC_LocalID fp_NextLaneID() {
        return LaneID++;
    }

    CPMC_LocalID fp_NextColumnID() {
        return ColumnID++;
    }

    CPMC_LocalID fp_NextCardID() {
        return CardID++;
    }

    static LocalIDGenerator& f_Instance()
    {
        static LocalIDGenerator Generator{};
        return Generator;
    }

private:
    int LaneID = 1;
    int ColumnID = 100;
    int CardID = 1000;
};

#endif // LOCALIDGENERATOR_H
