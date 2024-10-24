#include <string>
#include <map>

namespace utils
{

    class Value  // 封装一个Value对象，支持string、int、double、bool类型，可以实现多个类型转换为Value对象，并且可以转换为多个类型
    {
    private:
        std::string m_value; 
    public:
        //支持各个类型的构造函数
        Value();
        Value(const std::string& value);
        Value(const int value);
        Value(const double value);
        Value(const bool value);
        Value(const char* value);
        ~Value();

        //赋值操作,支持任意类型
        Value& operator=(const std::string& value);
        Value& operator=(const int value);
        Value& operator=(const double value);
        Value& operator=(const bool value);
        Value& operator=(const char* value);

        //对于值的判断
        bool operator==(const Value& other);
        bool operator!=(const Value& other);

        //这里实现可以将Value对象转换为任意类型
        operator int();
        operator double();
        operator bool();
        operator std::string();
    };

    typedef std::map<std::string, Value> Section;

    class IniFile
    {
        private:
            std::string m_filename;
            std::map<std::string, Section> m_sections;
            std::string trim(std::string s);  //去除字符串两边的空格 
        public:
            IniFile();
            IniFile(const std::string& filename);
            ~IniFile();

            bool load(const std::string& filename);  //加载ini文件
            bool save(const std::string& filename);  //保存ini文件
            void show(); //显示ini文件内容
            void clear(); //清空ini文件内容
            
            Value& get(const std::string& section,const std::string& key); //获取指定分区指定键的值
            void set(const std::string& section,const std::string& key,const Value& value); //设置指定分区指定键的值
            bool remove(const std::string& section,const std::string& key); //删除指定分区指定键的值
            
            bool has(const std::string& section,const std::string& key); //判断键值对是否存在
            bool has(const std::string& section);// 判断分区是否存在
            
            Section& operator[](const std::string& section); // 重载[]操作符,用来访问分区中的键值
            std::string str(); //将字符串转换为ini文件格式
    };
};