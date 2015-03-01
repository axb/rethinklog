#include "storage.h"
#include "log_entry.h"

#include <chrono>
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

Storage::Storage(boost::asio::io_service & io, Config & cfg)
	: _cfg(cfg), _io(io) {
	/// <todo> init stripes & SLAs
	for (auto sc : _cfg.masterStripes()) {
		stripe(sc.id);
	}
	/// every cluster interface client handler knows which stripes it will write replicas to
	/// thus, we may need no synchronization on replicated stripes
	for (auto rsc : _cfg.replicaStripes()) {
		stripe(rsc.id);
	}
}

Config & Storage::config() {
	return _cfg;
}

void Storage::publish(std::string stripe_, std::string key, std::string data, std::string localtime,
	std::function< void(uint64_t offset) > whendone_) {
	uint64_t offset = 0;
	auto st = stripe(stripe_);
	if (!st) {
		offset = 0;
	}
	else {
		offset = (*st)( UINT64_MAX, key, data, localtime);

		/// <todo> replication SLA
		// find connection(s) for stripe
		// send out the data
		// on ack check SLA
	}
	whendone_(offset);
}

Stripe::pointer_type Storage::stripe(std::string stripe) {
	auto it = _stripes.find(stripe);
	if (it != _stripes.end())
		return it->second;
	/// <todo> check config for auto creation
	auto res = Stripe::create(stripe, _cfg);
	_stripes.insert({ stripe, res });
	return res;
}

void Storage::housekeeping() {}

void Storage::abort() {
	// close all open files
}


void Stripe::buildFrame() {
	//
	// build filename
	//
	std::stringstream dir;
	dir << _cfg.dataDir() << "/" << _id << "/";
	fs::create_directories(dir.str());

	int ix = 0;
	std::time_t tm = std::time(0);
	std::string sfn;
	do {
		std::stringstream fn;
		fn << dir.str() << tm << "_" << ix << ".log";
		++ix;
		sfn = fn.str();
	} while (fs::exists(sfn));

	//
	// open mapped file
	//
	if (_frame.is_open())
		_frame.close();

	bs::mapped_file_params args;
	args.path = sfn;
	args.new_file_size = FRAME_FILESIZE;
	_frame.open(bs::mapped_file_sink(args));
}

Stripe::Stripe(std::string id_, Config & cfg_) : _cfg(cfg_), _id(id_), _lastOffset(1) {
	/// <todo> read last offset from storage
}

uint64_t Stripe::operator() (uint64_t offset, std::string key, std::string data, std::string localtime) {
	if ( offset == UINT64_MAX )
		offset = ++_lastOffset;

	LogEntry le{ offset, key, data, localtime };
	if (le.size() >= FRAME_FILESIZE)
		return false; // no way to write that big thing - "нельзя впихнуть невпихуемое"

	 // let it be 2 lines - we can not OR'em
	if (!_frame.is_open())                                      buildFrame();
	if ((size_t)_frame.tellp() + le.size() >= FRAME_FILESIZE)   buildFrame();

	// finally write
	_frame << le;

	return offset;
}

Stripe::~Stripe() {
	if (_frame.is_open()) {
		std::flush(_frame);
		_frame.close();
	}
}
