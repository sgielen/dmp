#include "dmp_radio.hpp"

#include <string>
#include <tuple>

DmpRadio::DmpRadio(std::string name, std::weak_ptr<DmpServerInterface> server, std::shared_ptr<NumberPool> port_pool)
: name(name)
, server(server)
, port_pool(port_pool)
, radio_mutex(new std::mutex)
, loop(g_main_loop_new(nullptr, false))
, pipeline(gst_pipeline_new("tcp_bridge"))
, source(gst_element_factory_make("tcpserversrc", "recv"))
, sink(gst_element_factory_make("tcpserversink", "send"))
, recv_port(port_pool->AllocateNumber())
, send_port(port_pool->AllocateNumber())
{
	bus.reset(gst_pipeline_get_bus (GST_PIPELINE (pipeline.get())));
	add_bus_callbacks();
	
	if (!pipeline || !source || !sink)
	{
		CHECK_GSTREAMER_COMPONENT(pipeline);
		CHECK_GSTREAMER_COMPONENT(source);
		CHECK_GSTREAMER_COMPONENT(sink);
		throw std::runtime_error("Could not create the pipeline components for this radio.");
	}

	g_object_set(G_OBJECT(source.get()), "port", gint(recv_port), nullptr);
	g_object_set(G_OBJECT(sink.get()), "port", gint(send_port), nullptr);
	
	gst_bin_add_many (GST_BIN(pipeline.get()), source.get(), sink.get(), nullptr);
	gst_element_link_many(source.get(), sink.get(), nullptr);

	//Nifty GStreamer debug graph.
	std::string prefix("dmp_radio_");
	std::string graph_file_name = prefix + name;
	GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline.get()), GST_DEBUG_GRAPH_SHOW_ALL, graph_file_name.c_str());
}

DmpRadio::~DmpRadio()
{
	gst_element_set_state(pipeline.get(), GST_STATE_NULL);
	g_main_loop_quit(loop.get());
}

void DmpRadio::run()
{
	gst_element_set_state(pipeline.get(), GST_STATE_PLAYING);
	g_main_loop_run(loop.get());
}

//Reversing the logic here. The receiving end of the client is on the sender end of the radio and vice versa.
uint16_t DmpRadio::get_receiver_port()
{
	return send_port;
}

uint16_t DmpRadio::get_sender_port()
{
	return recv_port;
}

Playlist DmpRadio::get_playlist()
{
	return playlist;
}

void DmpRadio::stop()
{
	std::unique_lock<std::mutex> l(*radio_mutex);
	
	PlaylistEntry entry = playlist.front();
	auto sp = server.lock();
	sp->order_stop(entry.owner, name);
}

void DmpRadio::play()
{
	std::unique_lock<std::mutex> l(*radio_mutex);
	
	PlaylistEntry entry = playlist.front();
	auto sp = server.lock();
	sp->order_play(entry.owner, name);
}

void DmpRadio::pause()
{
	std::unique_lock<std::mutex> l(*radio_mutex);
	
	PlaylistEntry entry = playlist.front();
	auto sp = server.lock();
	sp->order_pause(entry.owner, name);
}

void DmpRadio::next()
{
	std::unique_lock<std::mutex> l(*radio_mutex);
	
	PlaylistEntry entry = playlist.front();
	auto sp = server.lock();
	sp->order_stop(entry.owner, name);
	
	playlist.erase(playlist.begin());
	entry = playlist.front();
	sp->order_stream(entry.owner, name, entry.entry, get_sender_port());
	
	sp->order_play(entry.owner, name);
}

void DmpRadio::queue(std::string queuer, std::string owner, dmp_library::LibraryEntry entry)
{
	std::unique_lock<std::mutex> l(*radio_mutex);
	
	bool queue_was_empty = playlist.empty();
	playlist.push_back({queuer, owner, entry});
	
	if (queue_was_empty) {
		PlaylistEntry entry = playlist.front();
		auto sp = server.lock();
		sp->order_stream(entry.owner, name, entry.entry, get_sender_port());
	}
}
