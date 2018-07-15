/*
 * master.h
 *
 *  Created on: 2018年7月15日
 *      Author: wangqiying
 */

#ifndef SRC_MASTER_MASTER_H_
#define SRC_MASTER_MASTER_H_

#include <braft/raft.h>                  // braft::Node braft::StateMachine
#include <braft/storage.h>               // braft::SnapshotWriter
#include <braft/util.h>                  // braft::AsyncClosureGuard
#include <braft/protobuf_file.h>         // braft::ProtoBufFile
#include "proto/master.pb.h"                 // CounterService
#include "work_node.h"

namespace elasticfaiss {
class MasterOpClosure;
class Master: public braft::StateMachine {
public:
	Master() :
			_node(NULL), _leader_term(-1) {
	}
	~Master() {
		delete _node;
	}

	// Starts this node
	int start(int32_t port);

	// Impelements Service methods
	void bootstrap(::google::protobuf::RpcController* controller,
			const ::elasticfaiss::BootstrapRequest* request,
			::elasticfaiss::BootstrapResponse* response,
			::google::protobuf::Closure* done);

	bool is_leader() const;

	// Shut this node down.
	void shutdown();

	// Blocking this thread until the node is eventually down.
	void join();

private:
	friend class MasterOpClosure;

	void redirect(::google::protobuf::Message* response);

	// @braft::StateMachine
	void on_apply(braft::Iterator& iter);

	void on_snapshot_save(braft::SnapshotWriter* writer, braft::Closure* done);

	int on_snapshot_load(braft::SnapshotReader* reader);

	void on_leader_start(int64_t term);
	void on_leader_stop(const butil::Status& status);

	void on_shutdown();
	void on_error(const ::braft::Error& e);
	void on_configuration_committed(const ::braft::Configuration& conf);
	void on_stop_following(const ::braft::LeaderChangeContext& ctx);
	void on_start_following(const ::braft::LeaderChangeContext& ctx);
	// end of @braft::StateMachine

private:
	braft::Node* volatile _node;
	butil::atomic<int64_t> _leader_term;
};

class MasterServiceImpl: public MasterService {
public:
	explicit MasterServiceImpl(Master* master) :
			_master(master) {
	}
	void bootstrap(::google::protobuf::RpcController* controller,
			const ::elasticfaiss::BootstrapRequest* request,
			::elasticfaiss::BootstrapResponse* response,
			::google::protobuf::Closure* done) {
		//return _counter->fetch_add(request, response, done);
	}
	void heartbeat(::google::protobuf::RpcController* controller,
			const ::elasticfaiss::HeartbeatRequest* request,
			::elasticfaiss::HeartbeatResponse* response,
			::google::protobuf::Closure* done) {
		//brpc::ClosureGuard done_guard(done);
		//return _counter->get(response);
	}
private:
	Master* _master;
};
}

#endif /* SRC_MASTER_MASTER_H_ */