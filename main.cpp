#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <regex>
#include <algorithm>

std::vector<std::string> names;

std::unordered_map<std::string, std::string> new_key_value;
std::unordered_map<std::string, std::string> old_key_value;

std::unordered_map<std::string, std::string> old_new_key;
std::unordered_map<std::string, std::string> old_new_value;

std::unordered_set<std::string> unknown_keys;

char replace_tab(char in)
{
    if (in == '\t')
        return ' ';
    return in;
}

void clean(std::string& in)
{
    int pos = in.find('\r');
    if (pos >= 0 && pos < in.size())
    {
        in.erase(pos, 1);
    }
    std::transform(in.begin(), in.end(), in.begin(), replace_tab);
    
}

char to_lower(char in)
{
    if (in == ' ')
        return '_';
    if (in <= 'Z' && in >='A')
        return in-('Z'-'z');
    return in;
}

std::string underscored(std::string in)
{
    std::transform(in.begin(), in.end(), in.begin(), to_lower);
    return in;
}


void extractProfiles()
{

    for (std::string name : names)
    {
        std::stringstream file_str;
        file_str << "/home/tim/Development/LegacyCura/resources/machine_profiles/" << name << ".ini";
        std::ifstream file(file_str.str().c_str());
        std::cerr << " reading " << file_str.str() << "\n";
    
        std::ofstream ini;
        std::stringstream ini_str;
        ini_str << "/home/tim/Documents/Cura meta docs/legacy machine profiles/" << name << ".ini";
        ini.open(ini_str.str().c_str());
        std::string line;
        if (file.is_open())
        {
            ini << "[info]\nname = " << name << "\n\n";
            bool output_now = false;
            while (getline(file, line))
            {
                clean(line);
                if (line == std::string("[profile]\r") || line == std::string("[profile]"))
                {
                    output_now = true;
                }
                if (line == std::string("[alterations]\r") || line == std::string("[alterations]"))
                {
                    output_now = false;
                }
                if (output_now)
                {
                    ini << line << '\n';
                }
            }
        }
        else
        {
            std::cerr << ">>>>>>  Couldn't open " << file_str.str() << "\n\n\n";
        }
        file.close();
        ini.close();
    }
}

void readIni(std::string name)
{
    std::stringstream file_str;
    file_str << "/home/tim/Development/LegacyCura/resources/machine_profiles/" << name << ".ini";
    std::ifstream file(file_str.str().c_str());
    std::cerr << " reading " << file_str.str() << "\n";
    
    
    
    std::string line;
    if (file.is_open())
    {
        std::regex key_value_regex("([^ ]*) = ([^\\n]*)");
        
        bool output_now = false;
        while (getline(file, line))
        {
            clean(line);
            if (line == std::string("[profile]\r") || line == std::string("[profile]"))
            {
                break;
            }

            std::cmatch sub_matches;    // same as std::match_results<const char*> cm;
            std::regex_match(line.c_str(), sub_matches, key_value_regex);

            if (sub_matches.size() == 3) // one match for the whole string
            {
                std::string key = sub_matches[1];
                std::string value = sub_matches[2];
                old_key_value.emplace(key, value);
                if (old_new_key.find(key) == old_new_key.end())
                {
                    unknown_keys.emplace(key);
                }
                else
                {
                    new_key_value.emplace(old_new_key[key], value);
                }
            }
        }
        while (getline(file, line))
        {
            if (line == std::string("[alterations]"))
            {
                break;
            }
        }
        std::regex key_regex("([^ ]*) =.*");
        std::string key;
        std::stringstream value_ss;
        while (getline(file, line))
        {
            clean(line);
            std::cmatch sub_matches;    // same as std::match_results<const char*> cm;
            std::regex_match(line.c_str(), sub_matches, key_regex);

            if (sub_matches.size() == 2) // one match for the whole string
            {
                if (key != std::string(""))
                {
                    old_key_value.emplace(key, value_ss.str());
                    value_ss = std::stringstream();
                    key = "";
                }
                key = sub_matches[1];
            }
            else
            {
                value_ss << line << "\\n";
            }
        }
        if (key != std::string(""))
        {
            old_key_value.emplace(key, value_ss.str());
            value_ss = std::stringstream();
            key = "";
        }
    }
    else
    {
        std::cerr << ">>>>>  Couldn't open " << file_str.str() << "\n\n\n";
    }
    file.close();
}

char cura_name_convert(char in)
{
    if (in == ' ')
        return '+';
    return in;
}

void readCfg(std::string name)
{
    std::stringstream file_str;
    
    std::string cura_file = name;
    std::transform(cura_file.begin(), cura_file.end(), cura_file.begin(), cura_name_convert);
    file_str << "/home/tim/Documents/Cura meta docs/legacy machine profiles/Cura2converted/" << cura_file << ".inst.cfg";
    std::ifstream file(file_str.str().c_str());
    std::cerr << " reading " << file_str.str() << "\n";
    
    
    
    std::string line;
    if (file.is_open())
    {
        std::regex regex("([^ ]*) = ([^\\n]*)");
        
        bool read_here = false;
        bool output_now = false;
        while (getline(file, line))
        {
            clean(line);
            if (line == std::string("[values]\r") || line == std::string("[values]"))
            {
                read_here = true;
                continue; // skip this line
            }
            if (!read_here) continue;
            
            std::cmatch sub_matches;    // same as std::match_results<const char*> cm;
            std::regex_match(line.c_str(), sub_matches, regex);
            
            if (sub_matches.size() == 3) // one match for the whole string
            {
                std::string key = sub_matches[1];
                std::string value = sub_matches[2];
                old_key_value.emplace(key, value);
//                 std::cerr << key << " : " << value << '\n';
                new_key_value.emplace(key, value);
            }
        }
    }
    else
    {
        std::cerr << ">>>>>>>  Couldn't open " << file_str.str() << "\n\n\n";
    }
    file.close();
    
}
void generateJson(std::string name)
{

        std::ofstream json;
        std::stringstream json_str;
        json_str << "/home/tim/Documents/Cura meta docs/legacy machine profiles/JSON/" << underscored(name) << ".def.json";
        json.open(json_str.str().c_str());

        std::string show_name = name;
        for (auto pair : new_key_value)
            std::cerr << pair.first << " : " << pair.second << '\n';
        if (new_key_value.find(std::string("machine_name")) != new_key_value.end())
        {
            show_name = new_key_value[std::string("machine_name")];
        }
        json << "{\n";
        json << "    \"id\": \"" << underscored(name) << "\",\n";
        json << "    \"name\": \"" << show_name << "\",\n";
        json << "    \"version\": 2,\n";
        json << "    \"inherits\": \"fdmprinter\",\n";
        json << "    \"metadata\": {\n";
        json << "        \"visible\": true,\n";
        json << "        \"author\": \"Ultimaker\",\n";
        json << "        \"manufacturer\": \"Unknown\",\n";
        json << "        \"category\": \"Other\",\n";
        json << "        \"file_formats\": \"text/x-gcode\",\n";
        json << "        \"platform_offset\": [ 0, 0, 0]\n";
        json << "    },\n";
        json << "\n";
        json << "    \"overrides\": {\n";
        
        
        readIni(name);
        readCfg(name);
        
        bool first = true;
        if (new_key_value.find(std::string("machine_name")) == old_key_value.end())
        {
            if (!first) json << ",\n";
            first=false;
            json << "        \"machine_name\": { \"default_value\": \"" << name << "\" }";
        }
        if (old_key_value.find(std::string("start.gcode")) != old_key_value.end())
        {
            if (!first) json << ",\n";
            first=false;
            json << "        \"machine_start_gcode\": {\n";
            json << "            \"default_value\": \"" << old_key_value["start.gcode"] << "\"\n";
            json << "        }";
        }
        if (old_key_value.find(std::string("end.gcode")) != old_key_value.end())
        {
            if (!first) json << ",\n";
            first=false;
            json << "        \"machine_end_gcode\": {\n";
            json << "            \"default_value\": \"" << old_key_value["end.gcode"] << "\"\n";
            json << "        }";
        }
        bool has_minx = old_key_value.find(std::string("extruder_head_size_min_x")) != old_key_value.end();
        bool has_maxx = old_key_value.find(std::string("extruder_head_size_max_x")) != old_key_value.end();
        bool has_miny = old_key_value.find(std::string("extruder_head_size_min_y")) != old_key_value.end();
        bool has_maxy = old_key_value.find(std::string("extruder_head_size_max_y")) != old_key_value.end();
        if (has_minx && has_maxx && has_miny && has_maxy) // most machine profiles have 0,0,0,0,0... anyway!
        {
            if (!first) json << ",\n";
            first=false;
            json << "        \"machine_head_polygon\": { \"default_value\": [";
            json << "[ " << old_key_value[std::string("extruder_head_size_min_x")] << ", " << old_key_value[std::string("extruder_head_size_min_y")] << "], ";
            json << "[ " << old_key_value[std::string("extruder_head_size_min_x")] << ", " << old_key_value[std::string("extruder_head_size_max_y")] << "], ";
            json << "[ " << old_key_value[std::string("extruder_head_size_max_x")] << ", " << old_key_value[std::string("extruder_head_size_max_y")] << "], ";
            json << "[ " << old_key_value[std::string("extruder_head_size_max_x")] << ", " << old_key_value[std::string("extruder_head_size_min_y")] << "]";
            json << "] }";
        }
        for (auto pair : new_key_value)
        {
            std::string key = pair.first;
            std::string value = pair.second;
            if (old_new_value.find(value) != old_new_key.end())
            {
                value = old_new_value[value];
            }
            if (!first)
            {
                json << ",\n";
            }
            first = false;
            if (key == std::string("machine_gcode_flavor")
                || key == std::string("retraction_combing")
                || key == std::string("support_pattern")
                || key == std::string("adhesion_type")
                || key == std::string("machine_name"))
            {
                json << "        \"" << key << "\": { \"default_value\": \""<< value <<"\" }";
            }
            else
            {
                json << "        \"" << key << "\": { \"default_value\": "<< value <<" }";
            }
        }
        json << "\n";
        
        
        json << "    }\n";
        json << "}\n";
        
        json.close();
}

void generateJsons()
{

    for (std::string name : names)
    {
        generateJson(name);
        new_key_value.clear();
        old_key_value.clear();
    }
    
    std::cerr << "Following keys were not processed:\n\t";
    for (std::string key : unknown_keys)
        std::cerr << key << ", \n\t";
    std::cerr << "\n";
}

void preSetup()
{
//     names.emplace_back("Hephestos");
//     names.emplace_back("Hephestos_XL");
//     names.emplace_back("Mendel");
//     names.emplace_back("RigidBot");
//     names.emplace_back("RigidBotBig");
//     names.emplace_back("Witbox");
    // unused /\ .
    
    names.emplace_back("BFB");
    names.emplace_back("DeltaBot");
    names.emplace_back("julia");
    names.emplace_back("Kupido");
    names.emplace_back("MakerBotReplicator");
    names.emplace_back("Ord");
    names.emplace_back("Prusa Mendel i3");
    names.emplace_back("punchtec Connect XL");
    names.emplace_back("rigid3d_3rdGen");
    names.emplace_back("Rigid3D");
    names.emplace_back("Rigid3d_Zero");
    names.emplace_back("RIGID3D HOBBY");
    names.emplace_back("ROBO 3D R1");
    names.emplace_back("Zone3d Printer");

    old_new_key.emplace(std::string("machine_name"), std::string("machine_name"));
//     old_new_key.emplace(std::string("machine_type"), std::string(""));
    old_new_key.emplace(std::string("machine_width"), std::string("machine_width"));
    old_new_key.emplace(std::string("machine_height"), std::string("machine_height"));
    old_new_key.emplace(std::string("machine_depth"), std::string("machine_depth"));
    old_new_key.emplace(std::string("machine_center_is_zero"), std::string("machine_center_is_zero"));
    old_new_key.emplace(std::string("has_heated_bed"), std::string("machine_heated_bed"));
    old_new_key.emplace(std::string("gcode_flavor"), std::string("machine_gcode_flavor"));
    old_new_key.emplace(std::string("extruder_amount"), std::string("machine_extruder_count"));
    
    
    
    old_new_value.emplace(std::string("True"), std::string("true"));
    old_new_value.emplace(std::string("False"), std::string("false"));
    old_new_value.emplace(std::string(""), std::string(""));
    old_new_value.emplace(std::string(""), std::string(""));
    old_new_value.emplace(std::string(""), std::string(""));
    old_new_value.emplace(std::string(""), std::string(""));
    
}


int main(int argc, char **argv) {
    preSetup();
    
//     extractProfiles();
    generateJsons();
    
    return 0;
}
