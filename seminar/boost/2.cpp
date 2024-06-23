#include <iostream>
#include <string>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
using namespace std;
using namespace boost::spirit;
using namespace boost;

int main(){
    string buf;
    double result;
    while (getline(cin, buf)) {
        // 1行読み込んでパースする
        string::const_iterator iter = buf.begin(), end = buf.end();
        bool success = qi::phrase_parse(iter, end, qi::double_, ascii::space, result); // パース
        if (success && iter == end) {
            cout << "[success] " << result << endl;
        } else {
            cout << "[failed] invalid syntax" << endl;
        }
    }
    return 0;
}
