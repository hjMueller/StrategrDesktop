//
// Created by Dmitry Khrykin on 2019-07-07.
//

#include "strategyhistory.h"

void stg::strategy_history::commit(const entry &new_state) {
    if (new_state != current_state) {
        undo_stack.push_back(current_state);
        redo_stack.clear();

        current_state = new_state;
    }
}

std::optional<stg::strategy_history::entry> stg::strategy_history::undo() {
    if (has_prevoius_state()) {
        redo_stack.push_back(current_state);

        current_state = undo_stack.back();
        undo_stack.pop_back();

        return current_state;
    }

    return std::nullopt;
}

std::optional<stg::strategy_history::entry> stg::strategy_history::redo() {
    if (has_next_state()) {
        undo_stack.push_back(current_state);

        current_state = redo_stack.back();
        redo_stack.pop_back();

        return current_state;
    }

    return std::nullopt;
}

bool stg::strategy_history::has_prevoius_state() {
    return !undo_stack.empty();
}

bool stg::strategy_history::has_next_state() {
    return !redo_stack.empty();
}

stg::strategy_history::strategy_history(stg::strategy_history::entry current_state)
        : current_state(std::move(current_state)) {}
