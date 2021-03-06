//
// Created by Dmitry Khrykin on 2019-11-23.
//

#include <algorithm>

#include "json.h"
#include "strategy.h"

std::string stg::json::serialize(const strategy &strategy) {
    auto json = nlohmann::json();

    json[keys::slot_duration] = strategy.time_slot_duration();
    json[keys::start_time] = strategy.begin_time();

    std::transform(strategy.activities().begin(),
                   strategy.activities().end(),
                   std::back_inserter(json[keys::activities]),
                   [](auto activity) {
                       return activity->to_json();
                   });

    std::transform(strategy.time_slots().begin(),
                   strategy.time_slots().end(),
                   std::back_inserter(json[keys::slots]),
                   [&strategy](auto &time_slot) {
                       auto activity_index = strategy
                               .activities()
                               .index_of(time_slot.activity);

                       if (activity_index) {
                           return nlohmann::json(*activity_index);
                       }

                       return nlohmann::json();
                   });

    return json.dump();
}

std::unique_ptr<stg::strategy> stg::json::parse(const std::string &json_string) {
    try {
        auto json = nlohmann::json::parse(json_string);

        auto activities = parse_activities(json);
        auto time_slots = parse_time_slots(json, activities);

        return std::make_unique<strategy>(time_slots, activities);
    } catch (...) {
        return nullptr;
    }
}

stg::time_slots_state::data_t
stg::json::parse_time_slots(const nlohmann::json &json,
                            const activity_list::data_t &activities) {
    time_slots_state::data_t time_slots_vector;

    auto time_slot_duration = strategy::defaults::time_slot_duration;
    if (!json[keys::slot_duration].is_null())
        time_slot_duration = json[keys::slot_duration];

    auto begin_time = strategy::defaults::begin_time;
    if (!json[keys::start_time].is_null())
        begin_time = json[keys::start_time];

    if (!json[keys::slots].is_null()) {
        for (auto it = json[keys::slots].begin();
             it != json[keys::slots].end();
             ++it) {

            auto slot_index = it - json[keys::slots].begin();
            auto time_slot_begin_time = begin_time + slot_index * time_slot_duration;

            auto time_slot = stg::time_slot(time_slot_begin_time,
                                            time_slot_duration);

            if (!it->is_null()) {
                auto activity_index = static_cast<activity_list::index_t>(*it);

                try {
                    auto *activity = activities.at(activity_index).get();
                    time_slot.activity = activity;
                } catch (const std::out_of_range &) {
                    // activity is present in time slots, but not present in
                    // strategy.activities(), so we won't preserve it.
                }
            }

            time_slots_vector.push_back(time_slot);
        }
    }

    return time_slots_vector;
}

stg::activity_list::data_t stg::json::parse_activities(const nlohmann::json &json) {
    activity_list::data_t activities_vector;

    if (!json[keys::activities].is_null()) {
        for (const auto &activity_json : json[keys::activities]) {
            auto activity = std::make_shared<stg::activity>(activity::from_json(activity_json));
            activities_vector.emplace_back(activity);
        }
    }

    return activities_vector;
}