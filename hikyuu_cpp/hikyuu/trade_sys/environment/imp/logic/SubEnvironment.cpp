/*
 *  Copyright (c) 2025 hikyuu.org
 *
 *  Created on: 2025-03-06
 *      Author: fasiondog
 */

#include "SubEnvironment.h"

#if HKU_SUPPORT_SERIALIZATION
BOOST_CLASS_EXPORT(hku::SubEnvironment)
#endif

namespace hku {

SubEnvironment::SubEnvironment() : EnvironmentBase("EV_Sub") {}

SubEnvironment::SubEnvironment(const EnvironmentPtr& ev1, const EnvironmentPtr& ev2)
: EnvironmentBase("EV_Sub") {
    if (ev1) {
        m_ev1 = ev1->clone();
    }
    if (ev2) {
        m_ev2 = ev2->clone();
    }
}

SubEnvironment::~SubEnvironment() {}

void SubEnvironment::_calculate() {
    HKU_IF_RETURN(!m_ev1 && !m_ev2, void());

    if (m_ev1) {
        m_ev1->setQuery(m_query);
    }

    if (m_ev2) {
        m_ev2->setQuery(m_query);
    }

    if (m_ev1 && !m_ev2) {
        auto values = m_ev1->getValues();
        auto dates = values.getDatetimeList();
        size_t total = dates.size();
        m_values.resize(total);
        for (size_t i = 0; i < total; i++) {
            m_date_index[dates[i]] = i;
            m_values[i] = values[i];
        }
        return;
    }

    if (!m_ev1 && m_ev2) {
        auto values = m_ev2->getValues();
        auto dates = values.getDatetimeList();
        size_t total = dates.size();
        m_values.resize(total);
        for (size_t i = 0; i < total; i++) {
            m_date_index[dates[i]] = i;
            m_values[i] = -values[i];
        }
        return;
    }

    auto values = m_ev1->getValues();
    auto dates = values.getDatetimeList();
    size_t total = dates.size();
    m_values.resize(total);
    for (size_t i = 0; i < total; i++) {
        m_date_index[dates[i]] = i;
        m_values[i] = values[i];
    }

    values = m_ev2->getValues();
    dates = values.getDatetimeList();
    total = dates.size();
    const auto* src = values.data();
    for (size_t i = 0; i < total; i++) {
        _addValid(dates[i], -src[i]);
    }
}

void SubEnvironment::_reset() {
    if (m_ev1) {
        m_ev1->reset();
    }
    if (m_ev2) {
        m_ev2->reset();
    }
}

EnvironmentPtr SubEnvironment::_clone() {
    auto p = make_shared<SubEnvironment>();
    if (m_ev1) {
        p->m_ev1 = m_ev1->clone();
    }
    if (m_ev2) {
        p->m_ev2 = m_ev2->clone();
    }
    return p;
}

HKU_API EnvironmentPtr operator-(const EnvironmentPtr& ev1, const EnvironmentPtr& ev2) {
    return make_shared<SubEnvironment>(ev1, ev2);
}

}  // namespace hku