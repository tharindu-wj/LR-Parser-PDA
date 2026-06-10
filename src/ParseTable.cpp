//
// Created by thari on 8/06/2026.
//

#include "ParseTable.h"
#include <iostream>

// short text for an action entry
static std::string actionText(const ActionEntry &entry) {
    if (entry.kind == ActionEntry::Kind::Shift) {
        return "s" + std::to_string(entry.value);
    }
    if (entry.kind == ActionEntry::Kind::Reduce) {
        return "r" + std::to_string(entry.value);
    }
    if (entry.kind == ActionEntry::Kind::Accept) {
        return "acc";
    }
    return "err";
}

void ParseTable::setAction(int state, const std::string &terminal, const ActionEntry &entry) {
    std::pair<int, std::string> key = std::make_pair(state, terminal);
    std::map<std::pair<int, std::string>, ActionEntry>::iterator found = action_.find(key);

    // no entry
    if (found == action_.end()) {
        action_[key] = entry;
        return;
    }

    // same action already there
    ActionEntry existing = found->second;
    if (existing.kind == entry.kind && existing.value == entry.value) {
        return;
    }

    // different action already there
    // conflict scenario
    std::string description = "State " + std::to_string(state) + " on '" + terminal + "': "
                              + actionText(existing) + " vs " + actionText(entry);
    conflicts_.push_back(description);

    // resolution: prefer SHIFT over REDUCE
    bool existingIsShift = (existing.kind == ActionEntry::Kind::Shift);
    bool entryIsShift = (entry.kind == ActionEntry::Kind::Shift);

    if (existingIsShift && !entryIsShift) {
    } else if (!existingIsShift && entryIsShift) {
        action_[key] = entry;
    } else if (entry.kind == ActionEntry::Kind::Reduce &&
        existing.kind == ActionEntry::Kind::Reduce &&
        entry.value < existing.value) {
        action_[key] = entry;
    }
}

void ParseTable::build(const Grammar &grammar, const FirstFollow &firstFollow, const LrAutomaton &automaton) {
    const std::vector<Production> &augmentedProductions = automaton.getAugmentedProductions();
    const std::map<std::pair<int, std::string>, int> &transitions = automaton.getTransitions();

    // shifts and gotos from automaton transitions
    for (const std::pair<const std::pair<int, std::string>, int> &entry : transitions) {
        int fromState = entry.first.first;
        const std::string &symbol = entry.first.second;
        int toState = entry.second;

        if (grammar.isNonTerminal(symbol)) {
            goto_[std::make_pair(fromState, symbol)] = toState;
        } else {
            ActionEntry shift;
            shift.kind = ActionEntry::Kind::Shift;
            shift.value = toState;
            setAction(fromState, symbol, shift);
        }
    }

    // reduces and accept from complete items in each state
    for (std::size_t state = 0; state < automaton.stateCount(); ++state) {
        std::vector<int> completed = automaton.reducibleProductions((int) state);
        for (std::size_t c = 0; c < completed.size(); ++c) {
            int productionIndex = completed[c];

            if (productionIndex == 0) {
                ActionEntry accept;
                accept.kind = ActionEntry::Kind::Accept;
                accept.value = 0;
                setAction((int) state, "$", accept);
                continue;
            }

            const std::string &leftSide = augmentedProductions[productionIndex].leftHandSide;
            const std::set<std::string> &followSet = firstFollow.followOf(leftSide);
            for (const std::string &terminal : followSet) {
                ActionEntry reduce;
                reduce.kind = ActionEntry::Kind::Reduce;
                reduce.value = productionIndex;
                setAction((int) state, terminal, reduce);
            }
        }
    }
}

ActionEntry ParseTable::action(int state, const std::string &terminal) const {
    std::map<std::pair<int, std::string>, ActionEntry>::const_iterator found =
        action_.find(std::make_pair(state, terminal));
    if (found == action_.end()) {
        return ActionEntry();
    }
    return found->second;
}

int ParseTable::gotoState(int state, const std::string &nonTerminal) const {
    std::map<std::pair<int, std::string>, int>::const_iterator found =
        goto_.find(std::make_pair(state, nonTerminal));
    if (found == goto_.end()) {
        return -1;
    }
    return found->second;
}

bool ParseTable::hasConflicts() const {
    return !conflicts_.empty();
}

void ParseTable::print() const {
    std::cout << "\nACTION table:\n";
    for (const std::pair<const std::pair<int, std::string>, ActionEntry> &entry : action_) {
        std::cout << "  state " << entry.first.first << ", " << entry.first.second
            << " : " << actionText(entry.second) << "\n";
    }

    std::cout << "\nGOTO table:\n";
    for (const std::pair<const std::pair<int, std::string>, int> &entry : goto_) {
        std::cout << "  state " << entry.first.first << ", " << entry.first.second
            << " : " << entry.second << "\n";
    }

    std::cout << "\nConflicts: ";
    if (conflicts_.empty()) {
        std::cout << "none\n";
    } else {
        std::cout << conflicts_.size() << "\n";
        for (std::size_t i = 0; i < conflicts_.size(); ++i) {
            std::cout << "  " << conflicts_[i] << "\n";
        }
    }
}