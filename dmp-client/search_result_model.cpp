#include "search_result_model.hpp"
#include "fusion_static_dispatch.hpp"
#include "message_outputter.hpp"

#include <string>
#include <iostream>

SearchResultModel::SearchResultModel()
{
	typedef QVector<int> QIntVector;
	qRegisterMetaType<QIntVector>("QIntVector");
}

void SearchResultModel::add_search_response(message::SearchResponse response)
{
	search_results.push_back(std::make_pair(Client(response.origin), response.results));
}

int SearchResultModel::row_count() const
{
	size_t rows = 0;
	return std::accumulate(search_results.cbegin(), search_results.cend(), 0, [](int acc, SearchResultsElement rh){return acc + rh.second.size();});
}

int SearchResultModel::column_count() const
{
	return boost::fusion::result_of::size<dmp_library::LibraryEntry>::type::value + 1;
}

std::string SearchResultModel::get_cell(int row, int column) const
{
	if(column < 0 || size_t(column) >= number_of_library_entry_members + 1) {
		throw std::out_of_range("Column index was out of range.");
	}

	for(SearchResultsElement p : search_results)
	{
		if(row > 0 && size_t(row) >= p.second.size()) {
			row -= p.second.size();
			continue;
		}
		
		return get_nth(boost::fusion::joint_view<dmp_library::Library::tracklist_t::value_type, Client>(p.second.at(row), p.first), column);
	}

	throw std::out_of_range("Row index was out of range.");
}

std::string SearchResultModel::header_data(int section) const
{
	if(section < 0 || size_t(section) >= number_of_library_entry_members + 1) {
		throw std::out_of_range("Column index was out of range.");
	}
	
	return get_nth_name<boost::fusion::joint_view<dmp_library::Library::tracklist_t::value_type, Client>>(section);
}

void SearchResultModel::clear()
{
	search_results.clear();
}

std::pair<std::string, dmp_library::LibraryEntry> SearchResultModel::get_row_info(int row)
{
	for(auto&p : search_results)
	{
		if(row > 0 && size_t(row) >= p.second.size()) {
			row -= p.second.size();
			continue;
		}
		return std::make_pair(p.first.client, p.second.at(row));
	}

	throw std::runtime_error("Row index is out of range");
}
