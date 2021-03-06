//
// Created by Dmitry Khrykin on 2020-01-30.
//

#ifndef STRATEGR_SELECTION_H
#define STRATEGR_SELECTION_H

#include <vector>
#include <functional>
#include <optional>

#include "strategy.h"
#include "notifiableonchange.h"

namespace stg {
    using index_t = strategy::time_slot_index_t;
    using grouped_selection_element = std::vector<index_t>;
    using grouped_selection = std::vector<grouped_selection_element>;

    struct selection : public std::vector<index_t>,
                       public notifiable_on_change {
        explicit selection(const stg::strategy &strategy);

        void toggle_at(index_t slot_index);
        void deselect_all();
        void select_all();
        void fill(index_t from_index, index_t to_index);

        bool is_continuous() const;

        bool has_selected(index_t slot_index);

        void reload();

        bool is_clicked() const;
        void set_is_clicked(bool is_clicked);

        const grouped_selection &grouped();

        void on_change_event() override;
    private:
        const stg::strategy &strategy;

        grouped_selection _grouped;

        std::function<void(int)> get_slot_height;

        bool _is_clicked = false;
    };
}

#endif //STRATEGR_SELECTION_H
