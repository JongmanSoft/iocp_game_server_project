#include "ai_manager.h"

std::vector<std::vector<bool>> collision_data;

void ai_init()
{
    collision_data.resize(100, std::vector<bool>(100));
    std::ifstream file("collision_data_small.csv");
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

    for (size_t i = 0; i < 100; ++i) {
        for (size_t j = 0; j < 100; ++j) {
            if (i < data.size() && j < data[i].size()) {
                try {
                    int value = std::stoi(data[i][j]);
                    collision_data[j][i] = value;
             
                }
                catch (const std::exception& e) {
                    std::cerr << "데이터 변환 오류: (" << i << ", " << j << ")" << std::endl;
                }
            }
        }
    }
    
}
