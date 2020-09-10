#include "helper_gameplay.hpp"
#include <sstream>
#include <fstream>
#include <iostream>

namespace Gameplay
{
    std::shared_ptr<Level>
    level_loader::load(std::string &filePath) const
    {
        std::ifstream file(filePath);
        std::string line;
        std::vector<int> bricks;
        while (std::getline(file, line))
        {
            std::stringstream lineStream(line);
            int value;
            while (lineStream >> value)
            {
                bricks.push_back(value);
            }
        }
        return std::shared_ptr<Level>(new Level());
    }
} // namespace Gameplay