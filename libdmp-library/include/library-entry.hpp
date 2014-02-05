#pragma once

#include "icu-ascii-transliterator.hpp"

#include <iostream>
#include <string>
#include <cstdint>

#include <boost/fusion/adapted.hpp>

namespace dmp_library
{

struct LibraryEntry
{
	LibraryEntry() = default;

	LibraryEntry(std::string artist, std::string title, std::string album, std::uint32_t track);

	LibraryEntry(LibraryEntry const&) = default;
	LibraryEntry& operator=(LibraryEntry const&) = default;
	LibraryEntry& operator=(LibraryEntry&&) = default;
	bool operator==(LibraryEntry const& that) const;

	std::string artist;
	std::string ascii_artist;
	std::string title;
	std::string ascii_title;
	std::string album;
	std::string ascii_album;
	std::uint32_t track;
	std::uint32_t id;

	friend std::ostream& operator<<(std::ostream& os, LibraryEntry const& le);

	template<typename Archive>
	void serialize(Archive& ar, const unsigned int)
	{
		ar & artist & ascii_artist & title & ascii_title & album & ascii_album & track & id;
	}

private:
	static uint32_t next_id;
};

}

BOOST_FUSION_ADAPT_STRUCT
(
	dmp_library::LibraryEntry,
	(std::string, artist)
	(std::string, ascii_artist)
	(std::string, title)
	(std::string, ascii_title)
	(std::string, album)
	(std::string, ascii_album)
	(uint32_t, track)
	(uint32_t, id)
)
