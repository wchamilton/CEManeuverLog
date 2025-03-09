#include "CEManeuvers.h"

extern const QMap<QString, Maneuver> master_maneuver_map = {
    // Spin maneuver
    {"0S1", Maneuver("0S1", "-/-/-", false, Maneuver::Rot_North, { Maneuver::Right }, {388, 529})},

    // Speed 0 maneuvers
    {"1L0", Maneuver("1L0", "-/X/-", true, Maneuver::Rot_North_West, { Maneuver::Left  }, {175, 35})},
    {"1S0", Maneuver("1S0", "-/X/-", true, Maneuver::Rot_North,      { Maneuver::Right }, {275, 35})},
    {"1R0", Maneuver("1R0", "-/X/-", true, Maneuver::Rot_North_East, { Maneuver::Right }, {395, 35})},

    // Speed 1 maneuvers
    {"10L1", Maneuver("10L1", Maneuver::Rot_North,      { Maneuver::North_West }, {130, 148})},
    {"8L1",  Maneuver("8L1",  Maneuver::Rot_North_West, { Maneuver::North_West }, {180, 135})},
    {"6S1",  Maneuver("6S1",  Maneuver::Rot_North_West, { Maneuver::North      }, {247, 130})},
    {"2S1",  Maneuver("2S1",  Maneuver::Rot_North,      { Maneuver::North      }, {292, 130})},
    {"7S1",  Maneuver("7S1",  Maneuver::Rot_North_East, { Maneuver::North      }, {338, 130})},
    {"8R1",  Maneuver("8R1",  Maneuver::Rot_North_East, { Maneuver::North_East }, {385, 133})},
    {"10R1", Maneuver("10R1", Maneuver::Rot_North,      { Maneuver::North_East }, {435, 148})},

    // Speed 2 maneuvers
    {"11L2", Maneuver("11L2", Maneuver::Rot_South,      { Maneuver::North_West }, {35, 220})},
    {"9L2",  Maneuver("9L2",  Maneuver::Rot_South_West, { Maneuver::North_West }, {30, 370})},
    {"17L2", Maneuver("17L2", Maneuver::Rot_North,      { Maneuver::North_West, Maneuver::North_West }, {65,  320})},
    {"16L2", Maneuver("16L2", Maneuver::Rot_North_West, { Maneuver::North_West, Maneuver::North_West }, {100, 285})},
    {"15L2", Maneuver("15L2", Maneuver::Rot_South_West, { Maneuver::North,      Maneuver::North_West }, {135, 270})},
    {"14L2", Maneuver("14L2", Maneuver::Rot_North_West, { Maneuver::North,      Maneuver::North_West }, {180, 250})},
    {"12S2", Maneuver("12S2", Maneuver::Rot_North_West, { Maneuver::North,      Maneuver::North      }, {247, 245})},
    {"3S2",  Maneuver("3S2",  Maneuver::Rot_North,      { Maneuver::North,      Maneuver::North      }, {292, 245})},
    {"13S2", Maneuver("13S2", Maneuver::Rot_North_East, { Maneuver::North,      Maneuver::North      }, {338, 245})},
    {"14R2", Maneuver("14R2", Maneuver::Rot_North_East, { Maneuver::North,      Maneuver::North_East }, {390, 250})},
    {"15R2", Maneuver("15R2", Maneuver::Rot_South_East, { Maneuver::North,      Maneuver::North_East }, {435, 270})},
    {"16R2", Maneuver("16R2", Maneuver::Rot_North_East, { Maneuver::North_East, Maneuver::North_East }, {475, 285})},
    {"17R2", Maneuver("17R2", Maneuver::Rot_North,      { Maneuver::North_East, Maneuver::North_East }, {510, 320})},
    {"9R2",  Maneuver("9R2",  Maneuver::Rot_South_East, { Maneuver::North_East }, {540, 370})},
    {"11R2", Maneuver("11R2", Maneuver::Rot_South,      { Maneuver::North_East }, {540, 220})},

    // Speed 3 maneuvers
    {"24L3", Maneuver("24L3", Maneuver::Rot_South,      { Maneuver::North, Maneuver::North_West, Maneuver::South_West }, {55,  530})},
    {"23L3", Maneuver("23L3", Maneuver::Rot_South_West, { Maneuver::North, Maneuver::North_West, Maneuver::South_West }, {85,  475})},
    {"22L3", Maneuver("22L3", Maneuver::Rot_South_West, { Maneuver::North, Maneuver::North_West, Maneuver::North_West }, {115, 440})},
    {"21L3", Maneuver("21L3", Maneuver::Rot_North_West, { Maneuver::North, Maneuver::North_West, Maneuver::North_West }, {150, 410})},
    {"20L3", Maneuver("20L3", Maneuver::Rot_North_West, { Maneuver::North, Maneuver::North,      Maneuver::North_West }, {185, 385})},
    {"18S3", Maneuver("18S3", Maneuver::Rot_North_West, { Maneuver::North, Maneuver::North,      Maneuver::North      }, {247, 377})},
    {"4S3",  Maneuver("4S3",  Maneuver::Rot_North,      { Maneuver::North, Maneuver::North,      Maneuver::North      }, {292, 377})},
    {"19S3", Maneuver("19S3", Maneuver::Rot_North_East, { Maneuver::North, Maneuver::North,      Maneuver::North      }, {338, 377})},
    {"20R3", Maneuver("20R3", Maneuver::Rot_North_East, { Maneuver::North, Maneuver::North,      Maneuver::North_East }, {387, 385})},
    {"21R3", Maneuver("21R3", Maneuver::Rot_North_East, { Maneuver::North, Maneuver::North_East, Maneuver::North_East }, {422, 410})},
    {"22R3", Maneuver("22R3", Maneuver::Rot_South_East, { Maneuver::North, Maneuver::North_East, Maneuver::North_East }, {452, 440})},
    {"23R3", Maneuver("23R3", Maneuver::Rot_South_East, { Maneuver::North, Maneuver::North_East, Maneuver::South_East }, {482, 475})},
    {"24R3", Maneuver("24R3", Maneuver::Rot_South,      { Maneuver::North, Maneuver::North_East, Maneuver::South_East }, {515, 530})},

    // Speed 4 maneuvers
    {"25S4", Maneuver("25S4", Maneuver::Rot_North_West, { Maneuver::North, Maneuver::North, Maneuver::North, Maneuver::North }, {247, 531})},
    {"5S4",  Maneuver("5S4",  Maneuver::Rot_North,      { Maneuver::North, Maneuver::North, Maneuver::North, Maneuver::North }, {292, 531})},
    {"26S4", Maneuver("26S4", Maneuver::Rot_North_East, { Maneuver::North, Maneuver::North, Maneuver::North, Maneuver::North }, {338, 531})},

    // Restricted speed 1 maneuvers
    {"28L1", Maneuver("28L1", Maneuver::Rot_South_West, { Maneuver::Right }, {115, 610}, true)},
    {"28R1", Maneuver("28R1", Maneuver::Rot_South_East, { Maneuver::Left },  {455, 610}, true)},

    // Restricted speed 2 maneuvers
    {"30L2", Maneuver("30L2", Maneuver::Rot_South_West, { Maneuver::North      }, {125, 650}, true)},
    {"31L2", Maneuver("31L2", Maneuver::Rot_North_East, { Maneuver::North_West }, {182, 670}, true)},
    {"27S2", Maneuver("27S2", Maneuver::Rot_South,      { Maneuver::Right      }, {291, 650}, true, true)},
    {"29S2", Maneuver("29S2", Maneuver::Rot_South,      { Maneuver::North      }, {291, 650}, true, true)},
    {"31R2", Maneuver("31R2", Maneuver::Rot_North_West, { Maneuver::North_East }, {387, 670}, true)},
    {"30R2", Maneuver("30R2", Maneuver::Rot_South_East, { Maneuver::North      }, {445, 650}, true)},

    // Restricted speed 3 maneuvers
    {"36L3", Maneuver("36L3", Maneuver::Rot_North_East, { Maneuver::North_West, Maneuver::North      }, {140, 770}, true)},
    {"34S3", Maneuver("34S3", Maneuver::Rot_North,      { Maneuver::North,      Maneuver::North_West }, {230, 780}, true)},
    {"32S3", Maneuver("32S3", Maneuver::Rot_South_West, { Maneuver::North,      Maneuver::North      }, {270, 780}, true)},
    {"33S3", Maneuver("33S3", Maneuver::Rot_South_East, { Maneuver::North,      Maneuver::North      }, {310, 780}, true)},
    {"35S3", Maneuver("35S3", Maneuver::Rot_North,      { Maneuver::North,      Maneuver::North_East }, {350, 780}, true)},
    {"36R3", Maneuver("36R3", Maneuver::Rot_North_West, { Maneuver::North_East, Maneuver::North      }, {430, 770}, true)},
};

BaseItem::~BaseItem()
{
    qDeleteAll(children);
}

QVariant BaseItem::data(int column) const
{
    return column_data.value(column);
}

void BaseItem::setData(int column, const QVariant &data)
{
    column_data[column] = data;
}
