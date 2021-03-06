//
// Created by Dmitry Khrykin on 2019-07-06.
//

#ifndef STRATEGR_SESSIONSLIST_H
#define STRATEGR_SESSIONSLIST_H

#include <iostream>

#include "session.h"
#include "privatelist.h"
#include "notifiableonchange.h"
#include "timeslotsstate.h"
#include "streamablelist.h"
#include "sessionscalculator.h"

namespace stg {
    using activity_sessions_list_base = private_list<session>;
    class strategy;

    class sessions_list :
            public activity_sessions_list_base,
            public notifiable_on_change,
            public streamable_list<sessions_list> {
    public:
        struct overview_item {
            float duration_percentage = 0.0;
            float begin_percentage = 0.0;
            std::optional<activity::color_t> color = std::nullopt;
        };

        struct bounds {
            index_t start_index = 0;
            index_t end_index = 0;

            bool contain(index_t index) {
                return index >= start_index && index <= end_index;
            }
        };

        std::vector<session> get_non_empty() const;

        bounds get_bounds_for(index_t session_index) const {
            auto &session = _data[session_index];

            auto global_begin_time = _data.front().begin_time();
            auto total_duration = _data.back().end_time() - _data.front().begin_time();
            auto slot_duration = _data.front().duration();

            index_t start_index = static_cast<index_t>(session.begin_time() - global_begin_time) / slot_duration;
            index_t end_index = start_index + session.length();

            return {start_index, end_index};
        }

        stg::sessions_list::index_t session_index_for_time_slot_index(index_t time_slot_index) const;

        const session *session_after(const session &activity_session) const;

        std::optional<session>
        session_before(const session &activity_session) const;

        std::vector<overview_item> overview() const;

        time_slot::time_t relative_time(const session &session) const;

        std::string class_print_name() const override;
    private:
        using activity_sessions_list_base::activity_sessions_list_base;

        void recalculate(const time_slots_state &time_slots_state);

        friend strategy;
    };
}

#endif //STRATEGR_SESSIONSLIST_H
