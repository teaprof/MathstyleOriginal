#ifndef TEDITABLEFORMULA_H
#define TEDITABLEFORMULA_H

#include <algorithm>
#include <vector>
#include <cstdint>

#include "Base/arithmetic.h"

class TEditableFormula
{
public:
    TEditableFormula(std::shared_ptr<TNumeric> Numeric) : numeric_(Numeric) {}
    void setEditableChild(std::shared_ptr<const TNumeric>& child) {
        editables_.push_back(child);
    }
    bool isEditable(std::shared_ptr<const TNumeric>& child) {
        auto v = isEditable(numeric_, child);
        if(v.has_value() && v.value() == true)
            return true;
        return false;
    }
    std::optional<bool> isEditableNonRecursive(std::shared_ptr<const TNumeric> element) {
        if(std::find(editables_.begin(), editables_.end(), element) != editables_.end()) {
            return true;
        }
        return nullopt;
    }
    std::optional<bool> isEditable(std::shared_ptr<const TNumeric> root, std::shared_ptr<const TNumeric> element) {
        if(root == element) {
            if(isEditableNonRecursive(root)) {
                return true;
            };
        }
        for(const auto& childen : root->operands) {
            if(isEditable(childen, element)) {
                return true;
            }
        }
        return nullopt;
    }
    bool replace(std::shared_ptr<TNumeric> oldN, std::shared_ptr<TNumeric>& newN) {
        if(numeric_ == oldN) {
            numeric_ = newN;            
            return true;
        }
        /// \todo: clear editables_ that are not contained in newN
        /// \todo: update editables_
        return replace(numeric_, oldN, newN);
    }

    bool replace(std::shared_ptr<TNumeric> root, const std::shared_ptr<TNumeric> oldN, std::shared_ptr<TNumeric> newN) {
        for(size_t n = 0; n < root->operands.size(); n++) {
            if(root->operands[n] == oldN) {
                root->operands[n] = newN;
                return true;
            }
        }
        for(size_t n = 0; n < root->operands.size(); n++) {
            if(replace(root->operands[n], oldN, newN)) {
                return true;
            }
        }
        return false;
    }

    void setMouseXY(int mouse_x, int mouse_y) {
        this->mouse_x = mouse_x;
        this->mouse_y = mouse_y;
    }

    int mouse_x;
    int mouse_y;
    std::shared_ptr<TNumeric> selected{nullptr};
    std::shared_ptr<TNumeric> active{nullptr};
private:
    std::vector<std::shared_ptr<const TNumeric>> editables_;
    std::shared_ptr<TNumeric> numeric_;
};

#endif // TEDITABLEFORMULA_H
