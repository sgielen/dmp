
#include "library-entry.hpp"

#include "ostream"

using namespace std;

namespace dmp_library {

uint32_t library_entry::next_id = 0;

library_entry::library_entry(string artist, string title, string album)
: artist(artist)
, ascii_artist(transliterate_to_ascii(artist))
, title(title)
, ascii_title(transliterate_to_ascii(title))
, album(album)
, ascii_album(transliterate_to_ascii(album))
, id(next_id++)
{}

ostream& operator<<(ostream& os, library_entry const& le)
{
	return os 	<< "{" << std::endl
				<< "\t" << "UID: " << le.id << std::endl
				<< "\t" << "artist: " << le.artist << std::endl
				<< "\t" << "a_artist: " << le.ascii_artist << std::endl
				<< "\t" << "title: " << le.title << std::endl
				<< "\t" << "a_title: " << le.ascii_title << std::endl
				<< "\t" << "album: " << le.album << std::endl
				<< "\t" << "a_album: " << le.ascii_album << std::endl
				<< "}" << std::endl;
}

}