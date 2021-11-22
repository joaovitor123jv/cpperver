#ifndef __CERVER_DFA
#define __CERVER_DFA

#include <iostream>
#include <string>
#include <unordered_set>
#include <unordered_map>

class DFA
{
private:
    std::unordered_set<char> alphabet;

    std::unordered_set<std::string> states;
    std::unordered_set<std::string> final_states;

    std::string initial_state;

    std::unordered_map<std::string, std::unordered_map<char, std::string>> transactions;
    std::unordered_map<std::string, std::string> alphabet_symbols_groups;
    std::unordered_map<std::string, std::unordered_set<std::string>> states_groups;

public:
    DFA() {}

    /**
     * @brief 
     * Adds a sequence of characters to the alphabet.
     * 
     * @param alphabet A std::string with each character being a symbol to add to alphabet
     * @return true on success
     * @return false if cannot add symbol to alphabet
     */
    bool addToAlphabet(std::string alphabet)
    {
        if (alphabet.length() <= 0)
            return false;

        for (char symbol : alphabet)
            this->alphabet.insert(symbol);

        return true;
    }

    /**
     * @brief 
     * Adds a sequence of characters to the alphabet, and remember the added
     * symbols as a group of name `group_name`
     * 
     * Usage examples:
     *  - DFA().addToAlphabet('0123456789', 'numbers')
     *  - DFA().addToAlphabet('abcdefghijklmnopqrstuvwxyz', 'a-z')
     * 
     * @param alphabet A std::string with each character being a symbol to add to alphabet
     * @param group_name A std::string that represents the group name of the alphabet symbols
     * @return true on success
     * @return false on failures
     */
    bool addToAlphabet(std::string alphabet, std::string group_name)
    {
        if (alphabet.length() <= 0)
            return false;

        if (group_name.length() <= 0)
            return false;

        for (char symbol : alphabet)
            this->alphabet.insert(symbol);

        this->alphabet_symbols_groups[group_name] = alphabet;

        return true;
    }

    /**
     * @brief Set a existing DFA state as a final state
     * 
     * @param state_name The state name to be defined as final
     * @return true Successfully set the requested state as a final state
     * @return false Cannot set the requested state as a final state
     */
    bool addFinalState(std::string state_name)
    {
        if (state_name.length() == 0)
            return false;

        if (this->states.find(state_name) == this->states.end())
            return false;

        this->final_states.insert(state_name);
        return true;
    }

    /**
     * @brief Set the Initial State object
     * 
     * @param state_name 
     * @return true 
     * @return false 
     */
    bool setInitialState(std::string state_name)
    {
        if (state_name.length() == 0)
            return false;

        if (this->states.find(state_name) == this->states.end())
            return false;

        this->initial_state = state_name;
        return true;
    }

    /**
     * @brief Inserts a new state to the current DFA
     * 
     * @param state_name 
     * @return true 
     * @return false 
     */
    bool addState(std::string state_name)
    {
        if (state_name.length() == 0)
            return false;

        this->states.insert(state_name);
        return true;
    }

    /**
     * @brief Creates a transaction, this defines the behavior of the DFA.
     * From `source`, when `symbol` is received, go to `target`
     * 
     * @param source The source DFA state
     * @param symbol The symbol that will trigger the change at this step
     * @param target The target DFA state
     * @return true 
     * @return false 
     */
    bool addTransaction(std::string source, char symbol, std::string target)
    {
        if (source.length() <= 0)
            return false;

        if (target.length() <= 0)
            return false;

        if (this->states.find(source) == this->states.end())
            return false;

        if (this->states.find(target) == this->states.end())
            return false;

        if (this->alphabet.find(symbol) == this->alphabet.end())
            return false;

        this->transactions[source][symbol] = target;
        return true;
    }

    /**
     * @brief Presents the current DFA
     * - Alphabet
     * - States
     * - Final states
     * - Initial state
     * - Transactions
     * 
     */
    void print()
    {
        std::cout << "------ DFA ------" << std::endl
                  << "Alphabet:" << std::endl;

        for (char symbol : this->alphabet)
            std::cout << "\t- " << symbol << std::endl;

        std::cout << std::endl
                  << std::endl
                  << "States:" << std::endl;

        for (std::string state_name : this->states)
            std::cout << "\t- " << state_name << std::endl;

        std::cout << std::endl
                  << std::endl
                  << "Final States:" << std::endl;

        for (std::string state_name : this->final_states)
            std::cout << "\t- " << state_name << std::endl;

        std::cout << std::endl
                  << std::endl
                  << "Initial State: " << this->initial_state << std::endl
                  << std::endl
                  << "Transactions:" << std::endl;

        for (auto const &first_level_map : this->transactions)
        {
            std::cout << "\tFrom " << first_level_map.first << ":" << std::endl;
            for (auto const &targeted_state : first_level_map.second)
                std::cout << "\t\t* Given " << targeted_state.first << " --> " << targeted_state.second << std::endl;
        }
    }

    /**
     * @brief Presents the current DFA configured groups, if any
     * Shows groups of alphabet symbols and states
     */
    void print_groups()
    {
        std::cout << "------ DFA groups ------" << std::endl
                  << "Alphabet symbol groups:" << std::endl;

        for (auto const &item : this->alphabet_symbols_groups)
            std::cout << "\t - " << item.first << std::endl
                      << "\t\t Elements: " << item.second << std::endl;

        std::cout << std::endl
                  << std::endl
                  << "States groups:" << std::endl;
        for (auto const &group : this->states_groups)
        {
            std::cout << "\t - " << group.first << ":" << std::endl;
            for (std::string state_name : group.second)
                std::cout << "\t\t - " << state_name << std::endl;
        }
    }
};

#endif