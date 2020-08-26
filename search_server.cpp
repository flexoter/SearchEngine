#include "search_server.h"
#include "iterator_range.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>
#include <array>

vector<string_view> SplitIntoWords(string_view line) {
  vector<string_view> r;
  r.reserve(WORDS_COUNT);
  size_t start = 0u, end = 0u;
  while(!line.empty()) {
    start = line.find_first_not_of(' ', 0u);
    end = line.find(' ', start); 
    r.push_back(line.substr(start, end - start));
    line.remove_prefix(end == line.npos ? line.size() : end + 1);
  }
  return r;
}

SearchServer::SearchServer(istream& document_input) {
  UpdateDocumentBase(document_input);
}

void SearchServer::UpdateDocumentBase(istream& document_input) {
  InvertedIndex new_index;

  for (string current_document; getline(document_input, current_document); ) {
    new_index.Add(move(current_document));
  }

  index = move(new_index);
}

int FindInsertionIndex(
  const array<pair<size_t, size_t>, 5>& t_arr, size_t count, size_t id) {
    int pos = -1;
    bool is_found = false;
    for (size_t index = 0u; index != t_arr.size(); index++) {
      if (!is_found && count > t_arr[index].first) {
        pos = index;
        is_found = true;
      }
      if (t_arr[index].second == id) {
        pos = index;
        break;
      }
    }
    return pos;
  }

void SearchServer::AddQueriesStream(
  istream& query_input, ostream& search_results_output
) {
  for (string current_query; getline(query_input, current_query); ) {
    const auto words = SplitIntoWords(current_query);
    array<pair<size_t, size_t>, 5> search_results;
    search_results.fill(make_pair(0u, DOCUMENT_COUNT + 1));

    map<size_t, size_t> hit_counts;
    for (auto word : words) {
      auto [succ, it] = index.Lookup(word);
      if (!succ) {
        continue;
      }
      for (const size_t id : it->second) {
        auto& count = ++hit_counts[id];
        int pos = FindInsertionIndex(search_results, count, id);
        if (pos != -1) {
          search_results[pos].first = count;
          search_results[pos].second = id;
        }
      }
    }
    sort(
      search_results.begin(),
      search_results.end(),
      [](pair<size_t, size_t> lhs, pair<size_t, size_t> rhs) -> bool
      {return lhs.first > rhs.first;}
    );

    search_results_output << current_query << ':';
    for (auto [hitcount, docid] : search_results) {
      if (hitcount == 0) {
        break;
      }
      search_results_output << " {"
        << "docid: " << docid << ", "
        << "hitcount: " << hitcount << '}';
    }
    search_results_output << endl;
  }
}

void InvertedIndex::Add(string document) {
  docs.push_back(move(document));

  const size_t docid = docs.size() - 1;
  for (auto word : SplitIntoWords(docs.back())) {
    index[word].push_back(docid);
  }
}

using MapIt = map<string_view, list<size_t> >::const_iterator;
pair<bool, MapIt> InvertedIndex::Lookup(string_view word) const {
  auto it = index.find(word);
  bool is_found = it == index.end() ? false : true;
  return make_pair(is_found, it);
}
