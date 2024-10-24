#include "ini.h"

#include <iostream>
#include <fstream>
#include <sstream>


namespace utils{
        //支持各个类型的构造函数
        Value::Value(){}
        Value::Value(const std::string& value): m_value(value){}
        Value::Value(const int value)
        {
            *this=value;   
        }
        Value::Value(const double value)
        {
            *this=value;
        }
        Value::Value(const bool value)
        {
            *this=value;
        }

        Value::Value(const char* value)
        {
            m_value=value;
        }
        Value::~Value(){}

        //赋值操作,支持任意类型
        Value& Value::operator=(const std::string& value)
        {
            m_value = value;
            return *this;
        }

        Value& Value::operator=(const char* value)
        {
            m_value=value;
            return *this;
        }
        Value& Value::operator=(const int value)
        {
            std::stringstream ss;
            ss<<value;
            m_value=ss.str();
            return *this;
        }
        Value& Value::operator=(const double value)
        {
            std::stringstream ss;
            ss<<value;
            m_value=ss.str();
            return *this;   
        }
        Value& Value:: operator=(const bool value)
        {
            std::stringstream ss;
            ss<<value;
            m_value=ss.str();
            return *this;
        }

        //对于值的判断
        bool Value:: operator==(const Value& other)
        {
            return m_value==other.m_value;
        }
        bool Value:: operator!=(const Value& other)
        {
            return m_value!=other.m_value;
        }

        //这里实现可以将Value对象转换为任意类型
        Value::operator int()
        {
            return atoi(m_value.c_str());
        }
        Value::operator double()
        {
            return atof(m_value.c_str());
        }
        Value::operator bool()
        {
            return m_value=="true"?true:false;
        }
        Value::operator std::string()
        {
            return m_value;
        }

        IniFile::IniFile()
        {

        }

        IniFile::IniFile(const std::string& filename)
        {
            load(filename);
        }

        IniFile::~IniFile()
        {

        }

        bool IniFile:: load(const std::string& filename)
        {
            m_sections.clear();
            std::ifstream fin(filename);
            if(!fin)
            {
                std::cout<<"can not open file:"<<filename<<std::endl;
                return false;
            }
            std::string strline;
            std::string section;
            while(getline(fin,strline))
            {
                if(strline.empty()) continue;
                if(strline[0]=='#') continue; //跳过注释
                if(strline[0]=='[') //做分区处理
                {
                    int pos=strline.find_first_of(']');
                    if(pos==-1)
                    {
                        return false;
                    }
                    section=trim(strline.substr(1,pos-1));
                    section=trim(section);
                    m_sections[section]=Section();
                }
                else
                {
                    int pos=strline.find('=');
                    std::string key=trim(strline.substr(0,pos));
                    trim(key);
                    std::string value=trim(strline.substr(pos+1));
                    trim(value);
                    m_sections[section][key]=value;
                }   
            }
            fin.close();
            return true;
        }

        bool IniFile::save(const std::string& filename)
        {
            std::ofstream fout(filename);
            if(!fout)
            {
                return false;
            }
            fout<<str();
            fout.close();
            return true;
        }

        std::string IniFile::trim(std::string s)
        {
            if(s.empty())
            {
                return "";
            }
            s.erase(0,s.find_first_not_of(" \r\n"));
            s.erase(s.find_last_not_of(" \r\n")+1);
            return s;
        }

        void IniFile::show()
        {
            for(auto it=m_sections.begin();it!=m_sections.end();++it)
            {
                std::cout<<"["<<it->first<<"]"<<std::endl;
                for(auto it2=it->second.begin();it2!=it->second.end();++it2)
                {
                    std::cout<<it2->first<<"="<<(std::string)it2->second<<std::endl;
                }
            }
        }

        void IniFile::clear()
        {
            m_sections.clear();
        }

        Value& IniFile::get(const std::string& section,const std::string&key)
        {
            return m_sections[section][key];
        }

        void IniFile::set(const std::string& section,const std::string& key,const Value& value)
        {
            m_sections[section][key]=value;
        }

        bool IniFile::remove(const std::string& section,const std::string& key)
        {
            if(m_sections.empty())  return false;
            if(!has(section)) return false;
            if(!has(section,key)) return false;
            m_sections[section].erase(key);
            return true;
        }

        bool IniFile::has(const std::string& section)
        {
            return m_sections.find(section)!=m_sections.end();
        }

        bool IniFile::has(const std::string& section,const std::string& key)
        {
            return m_sections[section].find(key)!=m_sections[section].end();
        }

        Section& IniFile::operator[](const std::string& section)
        {
            return m_sections[section];
        }

        std::string IniFile::str()
        {
            std::stringstream ss;
            for(auto it=m_sections.begin();it!=m_sections.end();++it)
            {
                ss<<"["<<it->first<<"]"<<std::endl;
                for(auto it2=it->second.begin();it2!=it->second.end();++it2)
                {
                    ss<<it2->first<<"="<<(std::string)it2->second<<std::endl;
                }
            }
            ss<<std::endl;
            return ss.str();
        }
};



