#pragma once

#ifndef STABLE_HEADER
#define STABLE_HEADER

class SymbolTable
{
public:
    void dec_var(std::string identifier);
    void dec_arr(std::string identifier, int arr_size);

    void modify_var(std::string identifier, int newvalue);
    void modify_arr(std::string identifier, int index, int newvalue);

    const int &lookup_var(std::string varname) const;
    const int &lookup_arr(std::string arr_name, int index) const;

    bool var_exists(std::string varname) const;
    bool arr_exists(std::string arrname) const;

private:
    std::unordered_map<std::string, int> vars;
    std::unordered_map<std::string, std::vector<int>> arrays;
};

class ScopedTable
{
public:
    void dec_var(std::string identifier);
    void dec_arr(std::string identifier, int arr_size);
    void modify_var(std::string identifier, int newvalue);
    void modify_arr(std::string identifier, int index, int newvalue);
    const int &lookup_var(std::string varname) const;
    const int &lookup_arr(std::string arr_name, int index) const;
    void addScope();
    void removeScope();
    void back_to_global();
    ScopedTable();

private:
    int top;
    std::vector<SymbolTable> scopes;
};

#include "ScopedTable.inl"

#endif