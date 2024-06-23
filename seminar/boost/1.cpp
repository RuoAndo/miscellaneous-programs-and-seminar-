#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
using namespace boost::spirit;
using namespace boost;

template<typename Iterator>
struct bool_grammar : qi::grammar<Iterator, bool(), ascii::space_type>{
  bool_grammar() : bool_grammar::base_type(formula){ // formulaから開始
    using qi::lit;

    constant = (lit('T')[_val = true] |
                lit('F')[_val = false]);
    formula  = (constant                                 [_val = _1]       |
                ('(' >> formula >> 'v' >> formula >> ')')[_val = _1 || _2] |
                ('(' >> formula >> '^' >> formula >> ')')[_val = _1 && _2]);
  }
  qi::rule<Iterator, bool(), ascii::space_type> constant, formula;
};

#include <iostream>
#include <string>
using namespace std;
int main(){
  string buf;
  bool result;
  bool_grammar<string::const_iterator> grammar;
  while(getline(cin, buf)){
    string::const_iterator iter = buf.begin(), end = buf.end();
    bool success = qi::phrase_parse(iter, end, grammar, ascii::space, result);
    if(success && iter == end){ // パース成功かつ残り物がない
      cout << " => " << (result ? 'T' : 'F') << endl;
    }else{
      cout << " => parse failed" << endl;
    }
  }
  return 0;
}
