/*
 *  Producer.cpp
 *  DispatchProcessingDemo
 *
 *  Created by Chris Jones on 9/17/09.
 *  Copyright 2009 FNAL. All rights reserved.
 *
 */

#include "Producer.h"
#include "Event.h"
#include "Queues.h"


using namespace demo;

Producer::Producer(const std::string& iLabel,
                   ThreadType iThreadType):
Module(iLabel,iThreadType) {
}

Producer::Producer(const Producer& iOther):
Module(iOther.label(),iOther.threadType()),
m_products(iOther.m_products)
{
}
Producer::~Producer()
{
}

void
Producer::doProduce(Event& iEvent)
{
   edm::Event event(&iEvent,threadType()!=kThreadUnsafe);
   produce(event);
}
