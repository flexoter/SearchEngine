#include "search_server.h"
#include "iterator_range.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>
#include <array>
#include <vector>
#include <map>

vector<string_view> SplitIntoWords(string_view line) {
  vector<string_view> r;
  r.reserve(WORDS_COUNT);
  size_t start = 0u, end = 0u;
  while(!line.empty()) {
    start = line.find_first_not_of(' ', 0u);
    end = line.find(' ', start);
    start = start == line.npos ? 0u : start;
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
  swap(_index, new_index);
}

void SearchServer::AddQueriesStream(
  istream& query_input, ostream& search_results_output
) {
  for (string current_query; getline(query_input, current_query); ) {
    const auto words = SplitIntoWords(current_query);
    vector<pair<size_t, size_t> > hit_counts;
    hit_counts.resize(_index.GetDocumentCount(), make_pair(0u, 0u));
    

    for (auto word : words) {
      const auto [succ, it] = _index.Lookup(word);
      if (!succ) {
        continue;
      }
      for (const auto [id, count] : it->second) {
        auto& [pair_id, hits] = hit_counts[id];
        pair_id = id;
        hits += count;
      }
    }
    vector<pair<size_t, size_t> > results(5u);
    partial_sort_copy(
      hit_counts.begin(),
      hit_counts.end(),
      results.begin(),
      results.end(),
      [](pair<size_t, size_t> lhs, pair<size_t, size_t> rhs) -> bool
      {
        if (lhs.second > rhs.second) {
          return true;
        }
        if (lhs.second == rhs.second) {
          return lhs.first < rhs.first;
        } 
        return false;
      }
    );

    search_results_output << current_query << ':';
    for (const auto [docid, hitcount] : results) {
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
  _docs.push_back(move(document));

  const size_t docid = _docs.size() - 1;
  for (auto word : SplitIntoWords(_docs.back())) {
    auto& id_list = _stats[word];
    if (id_list.empty()) {
      id_list.reserve(DOCUMENT_PER_WORD);
    }
    if (id_list.back().first != docid || docid == 0) {
      id_list.push_back(make_pair(docid, 1u));
      continue;
    }
    id_list.back().second++;
  }
}

using MapIt = map<string_view, vector<pair<size_t, size_t>> >::const_iterator;
pair<bool, MapIt>
InvertedIndex::Lookup(string_view word) {
  bool r_succ = false;
  auto r_it = _stats.end();
  if (const auto it = _stats.find(word); it != _stats.end()) {
    r_succ = true;
    r_it = it;
  }
  return make_pair(r_succ, r_it);
}
