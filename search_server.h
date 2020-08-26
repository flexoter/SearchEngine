#ifndef H_SEARCH_SERVER
#define H_SEARCH_SERVER

#include <istream>
#include <ostream>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <string>

using namespace std;

const size_t WORDS_COUNT = 1000u;
const size_t DOCUMENT_COUNT = 50'000u;

class InvertedIndex {
public:
  InvertedIndex() {
    docs.reserve(DOCUMENT_COUNT);
  }
  void Add(string document);
  using MapIt = map<string_view, list<size_t> >::const_iterator;
  pair<bool, MapIt> Lookup(string_view word) const;

  const string& GetDocument(size_t id) const {
    return docs[id];
  }

private:
  map<string_view, list<size_t> > index;
  vector<string> docs;
};

class SearchServer {
public:
  SearchServer() = default;
  explicit SearchServer(istream& document_input);
  void UpdateDocumentBase(istream& document_input);
  void AddQueriesStream(istream& query_input, ostream& search_results_output);

private:
  InvertedIndex index;
};

#endif /*H_SEARCH_SERVER*/