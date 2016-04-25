//
//  GeantVProducer.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 2016/04/25.
//

#include <vector>
#include <memory>
#include "ConfiguredProducer.h"
#include "Event.h"
#include "Waiter.h"
#include "tbb/task.h"

namespace {

  /* Stand in for work done by GeantV via TBB based tasks*/

  class SimulateTask : public tbb::task {
    tbb::task* m_waitingTask;

  public:
    SimulateTask(tbb::task* iWaiting) :
      m_waitingTask(iWaiting) {}

    tbb::task* execute() override {
      usleep(.1*1000000);
      //IMPORTANT: decremeting the reference count is what
      // tells the framework the simulation is done with
      // this event.
      m_waitingTask->decrement_ref_count();
      return nullptr;
    }

  };
}

namespace demo {

  class GeantVProducer : public ConfiguredProducer {
  public:
    GeantVProducer(const boost::property_tree::ptree& iConfig);
  private:
    virtual void produce(edm::Event&) override;

    void runSimulation(tbb::task*);
    std::vector<const Getter*> m_getters;
  };

  GeantVProducer::GeantVProducer(const boost::property_tree::ptree& iConfig):
    ConfiguredProducer(iConfig,kThreadSafeBetweenInstances)
  {
    registerProduct(demo::DataKey());
    
    for(const boost::property_tree::ptree::value_type& v: iConfig.get_child("toGet")) {
      m_getters.push_back(registerGet(v.second.get<std::string>("label"), 
                                      ""));
    }
  }
  
  void 
  GeantVProducer::produce(edm::Event& iEvent) {
    int sum=0;
    for(std::vector<const Getter*>::iterator it = m_getters.begin(), itEnd=m_getters.end();
        it != itEnd;
        ++it) {
      sum +=iEvent.get(*it);
    }

    std::shared_ptr<tbb::task> waitTask{new (tbb::task::allocate_root()) tbb::empty_task{},
        [](tbb::task* iTask){tbb::task::destroy(*iTask);} };
    waitTask->set_ref_count(1+1);
    tbb::task* pWaitTask = waitTask.get();
   
    runSimulation(pWaitTask);

    waitTask->wait_for_all();

    iEvent.put(this,"",static_cast<int>(sum));
  }

  /* GeantV interface code goes there */
  void
  GeantVProducer::runSimulation(tbb::task* iWaitingTask) {
    //Need to decrement reference count on iWaitingTask when Event has been fully simulated
    auto simTask = new (tbb::task::allocate_root()) SimulateTask{iWaitingTask};
    tbb::task::spawn(*simTask);
  }
  
}
REGISTER_PRODUCER(demo::GeantVProducer);
