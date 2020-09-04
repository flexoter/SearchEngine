#ifndef H_SEARCH_SERVER
#define H_SEARCH_SERVER

#include "synchronized.h"
#include "iterator_range.h"

#include <istream>
#include <ostream>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <mutex>
#include <future>

using namespace std;

const size_t WORDS_COUNT = 50;
const size_t DOCUMENT_COUNT = 11'000u;
const size_t CORES_COUNT = 16u;
const size_t DOCUMENT_PER_WORD = 2000u;
const size_t STREAMS_COUNT = 100u;
const size_t QUERIES_COUNT = 12'000u;

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
  SearchServer()
  : _index()
  , _streams_begin(0u)
  , _streams_end(STREAMS_COUNT)
  {
    _streams.reserve(QUERIES_COUNT);
  }
  explicit SearchServer(istream& document_input);
  void UpdateDocumentBaseSingleThread(istream& document_input);
  void UpdateDocumentBase(istream& document_input);
  // void AddQueriesStreamSingleThread(istream& query_input, ostream& search_results_output);
  void AddQueriesStream(istream& query_input, ostream& search_results_output);

private:
  Synchronized<InvertedIndex>_index;
  vector<future<void> > _futures;
  vector<pair<istream*, ostream*> > _streams;
  size_t _streams_count;
  size_t _streams_begin;
  size_t _streams_end;
};

void AddQueriesStreamSingleThread(
  istream& query_input,
  ostream& search_results_output,
  Synchronized<InvertedIndex>& index);

using StreamsIt = vector<pair<istream*, ostream*> >::iterator;
void AddQueriesStreamMultiThread(
  IteratorRange<StreamsIt>,
  Synchronized<InvertedIndex>&);

#endif /*H_SEARCH_SERVER*/