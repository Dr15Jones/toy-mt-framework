//
//  Getter.cpp
//  SingleThreadedProcessingDemo
//
//  Created by Chris Jones on 8/5/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>

#include "Getter.h"

using namespace demo;

Getter::Getter(const std::string& iLabel,
               const std::string& iProduct):
m_label(iLabel),
m_product(iProduct),
m_value(0)
{}
