#ifndef TEDITABLEFORMULA_H
#define TEDITABLEFORMULA_H

#include <algorithm>
#include <vector>
#include <cstdint>

#include "Base/formulaplotter.h"

class TEditableFormula : public TFormulaPlotter
{
public:
    TEditableFormula(std::shared_ptr<TNumeric> Numeric, std::shared_ptr<TPaintCanvas> canvas) : TFormulaPlotter(canvas), numeric_(Numeric) {}
    void addEditable(std::shared_ptr<const TNumeric> child) {
        editables_.push_back(child);
    }
    bool isEditable(std::shared_ptr<const TNumeric> child) const {
        auto v = isEditable(numeric_, child);
        if(v.has_value() && v.value() == true)
            return true;
        return false;
    }
    bool isEditableNonRecursive(std::shared_ptr<const TNumeric> element) const {
        if(std::find(editables_.begin(), editables_.end(), element) != editables_.end()) {
            return true;
        }
        return false;
    }
    std::optional<bool> isEditable(std::shared_ptr<const TNumeric> root, std::shared_ptr<const TNumeric> element) const {
        //returns
        //  nullopt if element doesn't belong to the tree defined by the root element
        //  false if the path from the root element to element doesn't contain editables
        //  true otherwise
        bool root_is_editable = isEditableNonRecursive(root);
        if(root == element) {
            if(root_is_editable) {
                return true;
            };
            return false;
        }
        for(const auto& childen : root->operands) {
            auto v = isEditable(childen, element);
            if(v.has_value()) {
                return v.value() || root_is_editable;
            }
        }
        return nullopt;
    }
    bool replace(std::shared_ptr<TNumeric> oldN, std::shared_ptr<TNumeric> newN) {
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

    std::shared_ptr<TNumeric> underMouse() {
        return nullptr;
    }


    int mouse_x;
    int mouse_y;
    std::shared_ptr<TNumeric> selected{nullptr};
    std::shared_ptr<TNumeric> active{nullptr};


    virtual QColor getFontColor(std::shared_ptr<TNumeric> N) const {
        if(isEditable(N)) {
            return Qt::red;
            //return canvas_->EditableColor;
        }
        return canvas_->FormulaColor;
    }
    virtual std::optional<QColor> getBorderColor(std::shared_ptr<TNumeric> N) const {
        (void)N;
        return std::nullopt;
    }
private:
    std::vector<std::shared_ptr<const TNumeric>> editables_;
    std::shared_ptr<TNumeric> numeric_;
};

#endif // TEDITABLEFORMULA_H
