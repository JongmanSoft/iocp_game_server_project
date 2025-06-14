#pragma once
#include "stdafx.h"
struct Tile
{
    int _id[3] = { -1,-1,-1 };
    unsigned short _collision = 0;
};

class Map_data
{
public:
    std::vector<std::vector<Tile>> _maps;
private:
    

    Map_data() {
        _maps.resize(2000, std::vector<Tile>(2000));
    }

    Map_data(const Map_data&) = delete;
    Map_data& operator=(const Map_data&) = delete;

public:
    static Map_data& get_inst() {
        static Map_data inst; 
        return inst;
    }
    
    void csv_import(std::ifstream& file, int layer);
    void csv_export();
    void init();

};