#include "Map_data.h"

void Map_data::csv_import(std::ifstream& file, int layer)
{
    std::vector<std::vector<std::string>> data;
    std::string line;

    while (std::getline(file, line)) {
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string cell;
        while (std::getline(ss, cell, ',')) {
            row.push_back(cell);
        }
        data.push_back(row);
    }
    file.close();

    for (size_t i = 0; i < 2000; ++i) {
        for (size_t j = 0; j < 2000; ++j) {
            size_t data_row = i % 100; 
            size_t data_col = j % 100; 
            if (data_row < data.size() && data_col < data[data_row].size()) {
                try {
                    int value = std::stoi(data[data_row][data_col]);
                    _maps[j][i]._id[layer] = value;
                    if(layer ==2 )_maps[j][i]._collision = (value != -1) ? 1 : 0;
                    else if(layer==1&& _maps[j][i]._collision==0)_maps[j][i]._collision = (value != -1) ? 1 : 0;
                }
                catch (const std::exception& e) {
                    std::cerr << "데이터 변환 오류: (" << data_row << ", " << data_col << ")" << std::endl;
                }
            }
        }
    }
}

void Map_data::init()
{
    std::cout << "map import begin";
    //object.acc layer import
    {
        std::ifstream layer2("csv/100map_object_acc.csv");
        if (!layer2.is_open()) {
            std::cerr << "파일을 열 수 없습니다!" << std::endl;
        }
        csv_import(layer2, 2);
    }
    //object layer import
    {
        std::ifstream layer1("csv/100map_object.csv");
        if (!layer1.is_open()) {
            std::cerr << "파일을 열 수 없습니다!" << std::endl;
        }
        csv_import(layer1, 1);
    }
    //ground layer import
    {
        std::ifstream layer0("csv/100map_ground.csv");
        if (!layer0.is_open()) {
            std::cerr << "파일을 열 수 없습니다!" << std::endl;
        }
        csv_import(layer0, 0);
    }
    std::cout << "map import end";
}
