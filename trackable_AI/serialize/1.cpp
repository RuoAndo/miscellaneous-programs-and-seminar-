#include <iostream>
#include <fstream>
#include <string>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>

class Data {
    int value;
    std::string str;
public:
    Data() {}
    Data(int value, const std::string& str)
        : value(value), str(str) {}

    void print()
    {
        std::cout << value << std::endl;
        std::cout << str << std::endl;
    }

private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, unsigned int /*version*/)
    {
        ar & boost::serialization::make_nvp("value", value);
        ar & boost::serialization::make_nvp("str", str);
    }
};

int main()
{
    {
        std::ofstream file("a.txt");
        boost::archive::text_oarchive ar(file);

        Data data(3, "abc");
        ar << boost::serialization::make_nvp("Data", data);
    }
    {
        std::ifstream file("a.txt");

        boost::archive::text_iarchive ar(file);

        Data data;
        ar >> boost::serialization::make_nvp("Data", data);

        data.print();
    }
}
