#ifndef VLAN_H
#define VLAN_H

struct vlanhdr
{
    unsigned short v_priority:3,
                   v_cfi:1,
                   v_id:12;
    unsigned short v_type;
};

#endif // VLAN_H
