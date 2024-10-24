#include "ini.h"
#include <iostream>

using namespace utils;
using namespace std;

int main()
{
    IniFile ini;
    if(ini.load("../config/config.ini")==false)
    {
        perror("load failed");
    }
    ini.show();

    cout<<"------------------"<<'\n'<<endl;
    std::string s1= ini.get("server","port");
    cout<<s1<<endl;
    cout<<"------------------"<<'\n'<<endl;
    
    ini.set("server","port",1234);

    string s2= ini.get("server","port");
    cout<<s2<<endl;
    cout<<"------------------"<<'\n'<<endl;

    ini.remove("sections","port");
    
    if(ini.has("server"))  cout<<"ok"<<endl;
    cout<<"------------------"<<'\n'<<endl;
    
    if(ini.has("log","file"))  cout<<"ok"<<endl;
    cout<<"------------------"<<'\n'<<endl;
    
    if(ini.save("../config/config1.ini"))  cout<<"save success"<<endl;
    cout<<"------------------"<<'\n'<<endl;
    
    ini.show();
    cout<<"------------------"<<'\n'<<endl;
    
    ini.clear();
    
    ini.show();

}
