#pragma once
#include "stdafx.h"
class sprite_data
{
public:
    //sprite_resource 

    //orc sprite
    CImage orc_idle;
    CImage orc_walk;
    CImage orc_hurt;
    CImage orc_attack;
    CImage orc_death;

    //orc npc sprite
    CImage orc_npc_idle;
 
    //human monster sprite
    CImage human_idle;
    CImage human_walk;
    CImage human_hurt;
    CImage human_death;

    //human monster sprite
    CImage s_human_idle;
    CImage s_human_walk;
    CImage s_human_hurt;
    CImage s_human_attack;
    CImage s_human_death;


private:


    sprite_data() {
        
    }

   sprite_data(const sprite_data&) = delete;
   sprite_data& operator=(const sprite_data&) = delete;

public:
    static sprite_data& get_inst() {
        static sprite_data inst;
        return inst;
    }


    void init();

};