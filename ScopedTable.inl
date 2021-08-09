#ifndef STABLE_SOURCE
#define STABLE_SOURCE

inline void SymbolTable::dec_var(std::string identifier)
{
    vars.insert({identifier, 0});
}
inline void SymbolTable::dec_arr(std::string identifier, int arr_size)
{
    arrays.insert({identifier, std::vector<int>(arr_size, 0)});
}

inline void SymbolTable::modify_var(std::string identifier, int newvalue)
{
    std::unordered_map<std::string, int>::iterator got = vars.find(identifier);

    if (got != vars.end())
    {
        got->second = newvalue;
    }
    else
        throw std::invalid_argument("variable cannot be found");
}
inline void SymbolTable::modify_arr(std::string identifier, int index, int newvalue)
{
    std::unordered_map<std::string, std::vector<int>>::iterator got = arrays.find(identifier);

    if (got != arrays.end() && index < got->second.size())
    {
        got->second[index] = newvalue;
    }
    else
        throw std::invalid_argument("cannot find the value at given index or array is not declared");
}

inline const int &SymbolTable::lookup_var(std::string varname) const
{
    std::unordered_map<std::string, int>::const_iterator got = vars.find(varname);

    if (got != vars.end())
    {
        return got->second;
    }
    else
        throw std::invalid_argument("variable cannot be found");
}
inline const int &SymbolTable::lookup_arr(std::string arr_name, int index) const
{
    std::unordered_map<std::string, std::vector<int>>::const_iterator got = arrays.find(arr_name);

    if (got != arrays.end() && index < got->second.size())
    {
        return got->second[index];
    }
    else
        throw std::invalid_argument("cannot find the value at given index or array is not declared");
}

inline bool SymbolTable::var_exists(std::string varname) const
{
    std::unordered_map<std::string, int>::const_iterator got = vars.find(varname);

    if (got != vars.end())
        return true;
    else
        return false;
}

inline bool SymbolTable::arr_exists(std::string arrname) const
{
    std::unordered_map<std::string, std::vector<int>>::const_iterator got = arrays.find(arrname);

    if (got != arrays.end())
        return true;
    else
        return false;
}
//SCOPED TABLE

inline void ScopedTable::dec_var(std::string identifier)
{
    scopes[top].dec_var(identifier);
}
inline void ScopedTable::dec_arr(std::string identifier, int arr_size)
{
    scopes[top].dec_arr(identifier, arr_size);
}

inline void ScopedTable::modify_var(std::string identifier, int newvalue)
{
    for (int i = 0; i <= top; ++i)
    {
        if (scopes[i].var_exists(identifier))
        {
            scopes[i].modify_var(identifier, newvalue);
            return;
        }
    }

    throw std::invalid_argument("something went wrong");
}
inline void ScopedTable::modify_arr(std::string identifier, int index, int newvalue)
{
    for (int i = 0; i <= top; ++i)
    {
        if (scopes[i].arr_exists(identifier))
        {
            scopes[i].modify_arr(identifier, index, newvalue);
            return;
        }
    }

    throw std::invalid_argument("something went wrong");
}

inline const int &ScopedTable::lookup_var(std::string varname) const
{
    for (int i = 0; i <= top; ++i)
    {
        if (scopes[i].var_exists(varname))
        {
            return scopes[i].lookup_var(varname);
        }
    }

    throw std::invalid_argument("something went wrong");
}

inline const int &ScopedTable::lookup_arr(std::string arr_name, int index) const
{
    for (int i = 0; i <= top; ++i)
    {
        if (scopes[i].arr_exists(arr_name))
        {
            return scopes[i].lookup_arr(arr_name, index);
        }
    }

    throw std::invalid_argument("something went wrong");
}

inline void ScopedTable::addScope()
{
    SymbolTable _new;
    scopes.push_back(_new);

    ++top;
}

inline void ScopedTable::removeScope()
{
    --top;
}

inline void ScopedTable::back_to_global()
{
    top = 0;
}

inline ScopedTable::ScopedTable() : top(0)
{
    SymbolTable global_scope;
    scopes.push_back(global_scope);
}


#endif