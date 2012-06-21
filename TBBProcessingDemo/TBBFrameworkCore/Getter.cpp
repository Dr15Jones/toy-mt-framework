/*
 *  Getter.cpp
 *  DispatchProcessingDemo
 *
 *  Created by Chris Jones on 9/17/09.
 *  Copyright 2009 FNAL. All rights reserved.
 *
 */

#include "Getter.h"

using namespace demo;

Getter::Getter(const std::string& iLabel,
               const std::string& iProduct):
m_label(iLabel),
m_product(iProduct)
{}
