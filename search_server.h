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
const size_t WORD_LENGTH = 100u;
const size_t CORES_COUNT = 4u;
const size_t DOCUMENT_PER_WORD = 4000u;

vector<string_view> SplitIntoWords(string_view line);

class InvertedIndex {
public:
  InvertedIndex()
  {
    _docs.reserve(DOCUMENT_COUNT);
  }
  void Add(string document);
  using MapIt = map<string_view, vector<pair<size_t, size_t>> >::const_iterator;
  pair<bool, MapIt>
  Lookup(string_view word);
  size_t GetDocumentCount() const {
    return _docs.size();
  }

  const string& GetDocument(size_t id) const {
    return _docs[id];
  }

private:
  map<string_view, vector<pair<size_t, size_t>> > _stats;
  vector<string> _docs;
};

class SearchServer {
public:
  SearchServer() = default;
  explicit SearchServer(istream& document_input);
  void UpdateDocumentBase(istream& document_input);
  void AddQueriesStream(istream& query_input, ostream& search_results_output);

private:
  InvertedIndex _index;
};

#endif /*H_SEARCH_SERVER*/