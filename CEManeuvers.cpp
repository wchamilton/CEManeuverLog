#include "CEManeuvers.h"

extern const QList<Maneuver> master_maneuver_list = {
    // Spin maneuver
    Maneuver("0S1", "-", "-", "D1", false, Maneuver::Rot_North, { Maneuver::Right }),

    // Speed 0 maneuvers
    Maneuver("1L0", "-", "X", "-", true, Maneuver::Rot_North_West, { Maneuver::Left  }),
    Maneuver("1S0", "-", "X", "-", true, Maneuver::Rot_North,      { Maneuver::Right }),
    Maneuver("1R0", "-", "X", "-", true, Maneuver::Rot_North_East, { Maneuver::Right }),

    // Speed 1 maneuvers
    Maneuver("10L1", Maneuver::Rot_North,      { Maneuver::North_West }),
    Maneuver("8L1",  Maneuver::Rot_North_West, { Maneuver::North_West }),
    Maneuver("6S1",  Maneuver::Rot_North_West, { Maneuver::North      }),
    Maneuver("2S1",  Maneuver::Rot_North,      { Maneuver::North      }),
    Maneuver("7S1",  Maneuver::Rot_North_East, { Maneuver::North      }),
    Maneuver("8R1",  Maneuver::Rot_North_East, { Maneuver::North_East }),
    Maneuver("10R1", Maneuver::Rot_North,      { Maneuver::North_East }),

    // Speed 2 maneuvers
    Maneuver("11L2", Maneuver::Rot_South,      { Maneuver::North_West }),
    Maneuver("9L2",  Maneuver::Rot_South_West, { Maneuver::North_West }),
    Maneuver("17L2", Maneuver::Rot_North,      { Maneuver::North_West, Maneuver::North_West }),
    Maneuver("16L2", Maneuver::Rot_North_West, { Maneuver::North_West, Maneuver::North_West }),
    Maneuver("15L2", Maneuver::Rot_South_West, { Maneuver::North,      Maneuver::North_West }),
    Maneuver("14L2", Maneuver::Rot_North_West, { Maneuver::North,      Maneuver::North_West }),
    Maneuver("12S2", Maneuver::Rot_North_West, { Maneuver::North,      Maneuver::North      }),
    Maneuver("3S2",  Maneuver::Rot_North,      { Maneuver::North,      Maneuver::North      }),
    Maneuver("13S2", Maneuver::Rot_North_East, { Maneuver::North,      Maneuver::North      }),
    Maneuver("14R2", Maneuver::Rot_North_East, { Maneuver::North,      Maneuver::North_East }),
    Maneuver("15R2", Maneuver::Rot_South_East, { Maneuver::North,      Maneuver::North_East }),
    Maneuver("16R2", Maneuver::Rot_North_East, { Maneuver::North_East, Maneuver::North_East }),
    Maneuver("17R2", Maneuver::Rot_North,      { Maneuver::North_East, Maneuver::North_East }),
    Maneuver("9R2",  Maneuver::Rot_South_East, { Maneuver::North_East }),
    Maneuver("11R2", Maneuver::Rot_South,      { Maneuver::North_East }),

    // Speed 3 maneuvers
    Maneuver("24L3", Maneuver::Rot_South,      { Maneuver::North, Maneuver::North_West, Maneuver::South_West }),
    Maneuver("23L3", Maneuver::Rot_South_West, { Maneuver::North, Maneuver::North_West, Maneuver::South_West }),
    Maneuver("22L3", Maneuver::Rot_South_West, { Maneuver::North, Maneuver::North_West, Maneuver::North_West }),
    Maneuver("21L3", Maneuver::Rot_North_West, { Maneuver::North, Maneuver::North_West, Maneuver::North_West }),
    Maneuver("20L3", Maneuver::Rot_North_West, { Maneuver::North, Maneuver::North,      Maneuver::North_West }),
    Maneuver("18S3", Maneuver::Rot_North_West, { Maneuver::North, Maneuver::North,      Maneuver::North      }),
    Maneuver("4S3",  Maneuver::Rot_North,      { Maneuver::North, Maneuver::North,      Maneuver::North      }),
    Maneuver("19S3", Maneuver::Rot_North_East, { Maneuver::North, Maneuver::North,      Maneuver::North      }),
    Maneuver("20R3", Maneuver::Rot_North_East, { Maneuver::North, Maneuver::North,      Maneuver::North_East }),
    Maneuver("21R3", Maneuver::Rot_North_East, { Maneuver::North, Maneuver::North_East, Maneuver::North_East }),
    Maneuver("22R3", Maneuver::Rot_South_East, { Maneuver::North, Maneuver::North_East, Maneuver::North_East }),
    Maneuver("23R3", Maneuver::Rot_South_East, { Maneuver::North, Maneuver::North_East, Maneuver::South_East }),
    Maneuver("24R3", Maneuver::Rot_South,      { Maneuver::North, Maneuver::North_East, Maneuver::South_East }),

    // Speed 4 maneuvers
    Maneuver("25S4", Maneuver::Rot_North_West, { Maneuver::North, Maneuver::North, Maneuver::North, Maneuver::North }),
    Maneuver("5S4",  Maneuver::Rot_North,      { Maneuver::North, Maneuver::North, Maneuver::North, Maneuver::North }),
    Maneuver("26S4", Maneuver::Rot_North_East, { Maneuver::North, Maneuver::North, Maneuver::North, Maneuver::North }),

    // Restricted speed 1 maneuvers
    Maneuver("28L1", Maneuver::Rot_South_West, { Maneuver::Right }, true),
    Maneuver("28R1", Maneuver::Rot_South_East, { Maneuver::Left }, true),

    // Restricted speed 2 maneuvers
    Maneuver("30L2", Maneuver::Rot_South_West, { Maneuver::North      }, true),
    Maneuver("31L2", Maneuver::Rot_North_East, { Maneuver::North_West }, true),
    Maneuver("29S2", Maneuver::Rot_South,      { Maneuver::North      }, true, true),
    Maneuver("31R2", Maneuver::Rot_North_West, { Maneuver::North_East }, true),
    Maneuver("30R2", Maneuver::Rot_South_East, { Maneuver::North      }, true),

    // Restricted speed 3 maneuvers
    Maneuver("36L3", Maneuver::Rot_North_East, { Maneuver::North_West, Maneuver::North      }, true),
    Maneuver("34S3", Maneuver::Rot_North,      { Maneuver::North,      Maneuver::North_West }, true),
    Maneuver("32S3", Maneuver::Rot_South_West, { Maneuver::North,      Maneuver::North      }, true),
    Maneuver("33S3", Maneuver::Rot_South_East, { Maneuver::North,      Maneuver::North      }, true),
    Maneuver("35S3", Maneuver::Rot_North,      { Maneuver::North,      Maneuver::North_East }, true),
    Maneuver("36R3", Maneuver::Rot_North_West, { Maneuver::North_East, Maneuver::North      }, true),
};

BaseItem::~BaseItem()
{
    qDeleteAll(children);
}

QVariant BaseItem::data(int column) const
{
    return column_data.value(column);
}

void BaseItem::setData(int column, QVariant data)
{
    column_data[column] = data;
}
