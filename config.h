#ifndef CONFIG_H
#define CONFIG_H

#include <sstream>
#include <fstream>
#include <json/json.h>

struct Config
{
  int delay, color, dilution;

  Config(int delay= 100, int color= 100, int dilution= 5)
  : delay(delay) // 100ms
  , color(color) // 20-255
  , dilution(dilution) // 1-100
  {
    std::ifstream config("config.json");
    if(!config.is_open())
      return;
    std::ostringstream strm_config;
    strm_config<< config.rdbuf();
    Json::Value root;
    Json::Reader reader;
    
    if(reader.parse(strm_config.str(), root))
    {
      delay= root["delay"].asInt();
      color= root["color"].asInt();
      dilution= root["dilution"].asInt();
    }
  }

};

#endif

