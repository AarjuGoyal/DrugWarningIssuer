//
//  main.cpp
//  HW3
//
//  Created by Aarju Goyal on 11/15/19.
//  Copyright © 2019 Aarju Goyal. All rights reserved.
//
//

#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;
#include <string.h>
#include <vector>
#include <queue>
#include <math.h>
#include <ctype.h>
#include <map>

int number_of_queries;
int KB_length;

int new_variable = 0;
map<char,int> variable_use;

void trim(string& s)
{
    size_t p = s.find_first_not_of(" \t");
    s.erase(0, p);
    
    p = s.find_last_not_of(" \t");
    if (string::npos != p)
        s.erase(p+1);
}


struct arguments
{
    string name;
    int type = 0;
    //Wiil set
    //type = 1 for variables
    //type = 0 for constants
    //type = 2 nothing
    arguments(string arg_name)
    {
        name = arg_name;
        char first_ch = name[0];
        if(islower(first_ch))
        {
            type = 0;
//            cout<<"\nthis is a variable";
        }
        else
        {
            type = 1;
//            cout<<"\nthis is a constant";
        }

    }
    arguments()
    {
        name = "";
        type = 2;
    }
    void print_argument_name()
    {
        cout<<" "<<name;
    }
    bool isEqual(arguments other_arg)
    {
        if(name == other_arg.name)
            return true;
        else
            return false;
    }
    bool isVariable()
    {
        if(type == 0)
            return true;
        else
            return false;
    }
    void assign_argument(arguments arg_name)
    {
        name = arg_name.name;
        type = arg_name.type;
    }
} typedef arguments;
class atomic_literal
{
    string name;
    bool negated = false;
    vector<arguments> literal_arguments;
    int argument_length = 0;
public:
    atomic_literal(string full_literal)
    {
        unsigned long opening_parantheses;
        unsigned long closing_parantheses;
        if(full_literal[0] == '~')
        {
            cout<<"\nliteral is negative";
            negated = true;
            full_literal = full_literal.substr(1,full_literal.length() - 1);
        }
        opening_parantheses = full_literal.find('(');
        closing_parantheses = full_literal.find(')');
        name = full_literal.substr(0,opening_parantheses);
//        cout<<"Literal name is "<<name<<endl;
        
        string argument_bunch = full_literal.substr(opening_parantheses+1, closing_parantheses - opening_parantheses - 1);
        //cout<<" The argument bunch is "<<argument_bunch<<endl;
        unsigned long comma_finder = 0;
        string new_argument;
        comma_finder = argument_bunch.find(',');
        
        while(true)
        {
            if(comma_finder!= string::npos)
            {
                new_argument = argument_bunch.substr(0,comma_finder);
//                cout<<"\nThe next argument is "<<new_argument;
                trim(new_argument);
                arguments lit_arg(new_argument);
                literal_arguments.push_back(lit_arg);
                argument_bunch = argument_bunch.substr(comma_finder+1,argument_bunch.length() - comma_finder - 1);
                //cout<<" argument bunch now is "<<argument_bunch;
                comma_finder = argument_bunch.find(',');
            }
            else
            {
//                cout<<"\nLast argument is "<<argument_bunch;
                trim(argument_bunch);
                arguments lit_arg(argument_bunch);
                literal_arguments.push_back(lit_arg);
                break;
            }
            
        }
        
        argument_length = int(literal_arguments.size());
        
    }
    atomic_literal()
    {
        name = "";
        negated = false;
        argument_length = 0;
    }
    void print_literal_name()
    {
        cout<<name;
    }
    void assign_literal(atomic_literal new_literal)
    {
        name = new_literal.get_literal_name();
        negated = new_literal.is_negated();
        literal_arguments = new_literal.get_arguments();
        argument_length = new_literal.get_argument_length();
        
    }
    void assign_arguments(vector<arguments> args)
    {
        literal_arguments = args;
        argument_length = int(literal_arguments.size());
    }
    void print_literal()
    {
        if(negated)
        {
            cout<<"~"<<name;
            for(vector<arguments>::iterator it = literal_arguments.begin(); it!= literal_arguments.end(); ++it)
            {
                it->print_argument_name();
            }
        }
        else
        {
            cout<<name;
            for(vector<arguments>::iterator it = literal_arguments.begin(); it!= literal_arguments.end(); ++it)
            {
                it->print_argument_name();
            }
        }
    }
    string get_literal_name() const
    {
        return name;
    }
    bool same_literal(atomic_literal other_literal)
    {
        if (this->name == other_literal.get_literal_name())
            return true;
        else
            return false;
    }
    void negate()
    {
        negated = !negated;
    }
    bool is_negated()
    {
        return negated;
    }
    vector<arguments> get_arguments()
    {
        return literal_arguments;
    }
    int get_argument_length()
    {
        return argument_length;
    }
    bool isEqual(atomic_literal other_literal)
    {
        if (this->name == other_literal.get_literal_name() && this->negated == other_literal.is_negated())
        {
            vector<arguments> other_lit_argumnets = other_literal.get_arguments();
            if(argument_length == other_lit_argumnets.size())
            {
                for(int i = 0; i<argument_length; i++)
                {
                    if(literal_arguments[i].isVariable() && other_lit_argumnets[i].isVariable())
                    {
                        continue;
                    }
                    else if(!literal_arguments[i].isEqual(other_lit_argumnets[i]))
                    {
                        return false;
                    }
                }
                return true;
            }
            else
                return false;
        }
        
        else
            return false;
    }
    
    
} typedef atomic_literal;
bool operator ==(const arguments& right, const arguments& left)
{
    return left.name == right.name;
}
bool operator ==(const atomic_literal& right, const atomic_literal& left)
{
    return right.get_literal_name() == left.get_literal_name();
}
struct substitution
{
    //given the expression x/y
    arguments substitutor; //this is x
    arguments substitutee; //this is y
    bool failure;
    bool nil;
    substitution()
    {
        failure = true;
        nil = true;
    }
    
} typedef substitution;
struct full_substitution
{
    vector<substitution> list_sub;
    bool fail = false;
}typedef full_substitution;
atomic_literal Apply_substitution(vector<substitution> subs, atomic_literal appl_lit)
{
    //atomic_literal subs_lit;
    arguments substitutee;
    vector<arguments> literal_Arguments = appl_lit.get_arguments();
    vector<arguments>::iterator arg_inside_lit;
    for(vector<substitution>::iterator sub_it = subs.begin(); sub_it != subs.end(); ++sub_it)
    {
        substitutee = sub_it->substitutee;
        arg_inside_lit = find(literal_Arguments.begin(), literal_Arguments.end(), substitutee);
        while (arg_inside_lit != literal_Arguments.end())
        {
            cout<<" The substitutee is found at position ";
            cout << arg_inside_lit - literal_Arguments.begin() + 1 << "\n" ;
            arg_inside_lit->assign_argument(sub_it->substitutor);
            arg_inside_lit = find(arg_inside_lit, literal_Arguments.end(), substitutee);
        }
    }
    appl_lit.assign_arguments(literal_Arguments);
    appl_lit.print_literal();
    return appl_lit;
}
substitution unify(arguments in_x, arguments in_y)
{
    substitution theta;
    //cout<<"\nThe arguments are ";
    //in_x.print_argument_name();
    //cout<<" and ";
    //in_y.print_argument_name();
    if(in_x.isEqual(in_y))
    {
//        cout<<"\nUnification is not false and returned because both x and y are equal ";
        theta.failure = false;
        return theta;
    }
    else if(in_x.isVariable())
    {
        theta.substitutor = in_y;
        theta.substitutee = in_x;
        theta.failure = false;
        theta.nil = false;
//        cout<<"\nUnification is not false and not nil returned because x is a variable ";
        return theta;
    }
    else if(in_y.isVariable())
    {
        theta.substitutor = in_x;
        theta.substitutee = in_y;
        theta.failure = false;
        theta.nil = false;
//        cout<<"\nUnification is not false and not nil returned because y is a variable ";
        return theta;
    }
    else
    {
//        cout<<"\nUnification is false and nil";
        return theta;
    }
}
full_substitution unify_literal(atomic_literal x, atomic_literal y)
{
    substitution new_substitution;
    full_substitution lit_substitution;
    if(x.get_literal_name() == y.get_literal_name())
    {
        if(x.get_argument_length() == y.get_argument_length())
        {
            vector<arguments> x_arguments = x.get_arguments();
            vector<arguments> y_arguments = y.get_arguments();
            for(int i=0; i<x_arguments.size(); i++)
            {
                new_substitution = unify(x_arguments[i],y_arguments[i]);
                if(new_substitution.failure)
                {
                    lit_substitution.fail = true;
                    return lit_substitution;
                }
                else if(!new_substitution.nil)
                {
//                    cout<<"\nA new substitution is found ";
//                    cout<<"\nThe substitutor is ";
//                    new_substitution.substitutor.print_argument_name();
//                    cout<<"\nThe substitutee is ";
//                    new_substitution.substitutee.print_argument_name();
                    for(int j=i+1; j<x_arguments.size(); j++)
                    {
                        if(x_arguments[j].isEqual(new_substitution.substitutee))
                            x_arguments[i] = new_substitution.substitutor;
                        if(y_arguments[j].isEqual(new_substitution.substitutee))
                            y_arguments[j] = new_substitution.substitutor;
                    }
                    lit_substitution.list_sub.push_back(new_substitution);
                }
            }
            return lit_substitution;
        }
        else
            return lit_substitution;
    }
    return lit_substitution;
    
    
}
class clause
{
    vector<atomic_literal> literal_list;
public:
    void print_clauses()
    {
        for(vector<atomic_literal>::iterator it=literal_list.begin(); it!=literal_list.end(); ++it)
        {
            it->print_literal();
            cout<<" | ";
        }
    }
    void add_literal(atomic_literal new_literal)
    {
        literal_list.push_back(new_literal);
    }
    vector<atomic_literal> get_literals()
    {
        return literal_list;
    }
    void standardize_clause()
    {
        int Clause_Variables[26] = {0};
        map<string,int> Use_Clause;
        for(vector<atomic_literal>::iterator lit=literal_list.begin(); lit!=literal_list.end(); ++lit)
        {
            
            vector<arguments> literal_args = lit->get_arguments();
            for(vector<arguments>::iterator arg = literal_args.begin(); arg!= literal_args.end(); ++arg)
            {
                if(arg->isVariable())
                {
                    string variable_value = arg->name;
                    
                    char var_first_letter = (arg->name)[0];
//                    cout<<"The value of Clause Variable for the particualr variable is "<<Clause_Variables[var_first_letter-97];
//                    if(Clause_Variables[var_first_letter-97] == 0)
//                    {
//                        Clause_Variables[var_first_letter-97]++;
//                        cout<<"\nVariable "<<var_first_letter<<" seen for the first time in the clause ";
//                        new_variable++;
//                        arg->name = "x" + to_string(new_variable);
//                    }
//                    else
//                    {
//                        cout<<"\nVariable "<<var_first_letter<<" not seen for the first time in the clause ";
//                        arg->name = "x" + to_string(new_variable);
//                    }
                    
                    if(variable_use[var_first_letter-97] == 0)//first time in KB
                    {
                        variable_use[var_first_letter-97] = variable_use[var_first_letter-97] + 1;
                        Clause_Variables[var_first_letter-97] = 1;
                    }
                    else
                    {
                        if(Use_Clause.count(variable_value) > 0)
                        {
                            
                        }
                        else
                        {
                            variable_use [var_first_letter-97] = variable_use[var_first_letter-97] + 1;
                            //Seen for the first time in this clause
//                            if(Clause_Variables[var_first_letter-97] == 0)
//                            {
//                                Clause_Variables[var_first_letter-97] = 1;
//                                //                            cout<<"\nVariable "<<var_first_letter<<" seen for the first time in the clause ";
//
//
//                            }
                            Use_Clause[variable_value] = variable_use [var_first_letter-97];
                            
                        }
                        
                       
                    }
                    arg->name = var_first_letter + to_string(Use_Clause[variable_value]);
                    
                    
                    
//                    cout<<" The value of variable use is among the whole KB is "<<variable_use[var_first_letter-97];
                    //arg->name = var_first_letter + to_string(variable_use[var_first_letter-97]);
                }
            }
            lit->assign_arguments(literal_args);
            
        }
    }
    bool resolve_possible(clause other_clause)
    {
//        cout<<"\ninside resolve possible considering ";
//        print_clauses();
//        cout<<" and ";
//        other_clause.print_clauses();
        vector<atomic_literal> first_list;
        vector<atomic_literal> second_list = other_clause.get_literals();
        
//        vector<atomic_literal> first_list(this->literal_list);
//        vector<atomic_literal> second_list(other_clause.get_literals());
        first_list.assign(literal_list.begin(),literal_list.end());
//        second_list.assign(other_clause.get_literals().begin(),other_clause.get_literals().end());
        for(vector<atomic_literal>::iterator lit=first_list.begin(); lit!=first_list.end(); ++lit)
        {
            for(vector<atomic_literal>::iterator slit=second_list.begin(); slit!=second_list.end(); ++slit)
            {
//                cout<<"\nconsidering the literal ";
//                lit->print_literal_name();
//                cout<<" and ";
//                slit->print_literal_name();
                
                if(lit->get_literal_name() == slit->get_literal_name() && lit->is_negated() != slit->is_negated())
                {
                    cout<<"\nresolve is possible exiting now";

                    return true;
                }
            }
        }
        cout<<"\nresolve is not possible exiting now";
        return false;
    }
    bool isEqual(clause new_clause)
    {
        vector<atomic_literal> new_clause_literals = new_clause.get_literals();
        vector<atomic_literal>::iterator sit;
        int flag = 0; //flag to check if something actually matched. flag = 0 means that no match found flag = 1 mean match found
        if(literal_list.size() == new_clause_literals.size())
        {
            for(int i=0; i<literal_list.size(); i++)
            {
//                cout<<"\nFind the similar literal for ";
//                literal_list[i].print_literal();
                flag=0;
                sit = find(new_clause_literals.begin(), new_clause_literals.end(), literal_list[i]);
                if(sit == new_clause_literals.end())
                {
                    //cout<<"\nDidn't find anything ";
                    return false;
                }
                else
                {
                    while(sit != new_clause_literals.end())
                    {
                        //cout<<"\nFound something at "<<sit - new_clause_literals.begin() + 1;
                        
                        if(literal_list[i].isEqual(*sit))
                        {
                            //cout<<"\nThey are the same";
                            flag = 1;
                            break;
                        }
                        else
                        {
//                            cout<<"\nThey are not the same";
                            sit = find(sit+1, new_clause_literals.end(), literal_list[i]);
                        }
                        
                        
                    }
                    if(flag == 0)
                        return false;
                }
            }
            return true;
        }
        else
        {
            return false;
        }
        return false;
    }
} typedef clause;
struct Knowledge_base
{
    vector<clause> facts;
    void add_fact(clause new_clause)
    {
        facts.push_back(new_clause);
    }
    
} typedef Knowledge_base;
Knowledge_base My_KB;
bool resolution_inference(string query)
{
    trim(query);
    atomic_literal query_literal(query);
    query_literal.negate();
    
    clause alpha;//formed by negating the query and converting it into clause form
    alpha.add_literal(query_literal);
    cout<<"\nThe not alpha is ";
    alpha.print_clauses();
    queue<clause> new_list;
    clock_t begin = clock();
    new_list.push(alpha);
    
    while(!new_list.empty())
    {
        
        vector<clause> Knowledge_base = My_KB.facts;
        clause inference_clause = new_list.front(); // This returns the clause at the front that is popped
        new_list.pop();
        
        //Comparing the new clause to see if it is resovable with any knowledge base clause
        clause knowledge_base_clause;
        for(vector<clause>::iterator it=Knowledge_base.begin(); it!=Knowledge_base.end(); ++it)
        {
            knowledge_base_clause = *it;
            vector<atomic_literal> first_list;
            vector<atomic_literal> second_list;
            first_list = inference_clause.get_literals();
            second_list = knowledge_base_clause.get_literals();
            for(vector<atomic_literal>::iterator lit=first_list.begin(); lit!=first_list.end(); ++lit)
            {
                for(vector<atomic_literal>::iterator slit=second_list.begin(); slit!=second_list.end(); ++slit)
                {
                    if(lit->get_literal_name() == slit->get_literal_name() && lit->is_negated() != slit->is_negated())
                    {
                        cout<<"\nresolve is possible between ";
                        it->print_clauses();
                        cout<<" and ";
                        inference_clause.print_clauses();
                        cout<<"\nThe literals chosen is ";
                        lit->print_literal_name();
                        cout<<" and ";
                        slit->print_literal_name();
                        full_substitution sub_between_clauses;
                        sub_between_clauses = unify_literal(*lit,*slit);
                        if(!sub_between_clauses.fail)
                        {
                           // cout<<"\nNew substitution found ";
                            vector<substitution> sub_claues = sub_between_clauses.list_sub;
                            clause new_added_clause;
                            for(vector<atomic_literal>::iterator nc_lt = first_list.begin(); nc_lt != lit; ++nc_lt)
                            {
                                atomic_literal subst_literal;
                                subst_literal.assign_literal(Apply_substitution(sub_claues,*nc_lt));
                                new_added_clause.add_literal(subst_literal);
                            }
                            for(vector<atomic_literal>::iterator nc_lt = lit+1; nc_lt != first_list.end(); ++nc_lt)
                            {
                                atomic_literal subst_literal;
                                subst_literal.assign_literal(Apply_substitution(sub_claues,*nc_lt));
                                new_added_clause.add_literal(subst_literal);
                            }
                            for(vector<atomic_literal>::iterator nc_lt = second_list.begin(); nc_lt != slit; ++nc_lt)
                            {
                                atomic_literal subst_literal;
                                subst_literal.assign_literal(Apply_substitution(sub_claues,*nc_lt));
                                new_added_clause.add_literal(subst_literal);
                            }
                            for(vector<atomic_literal>::iterator nc_lt = slit+1; nc_lt != second_list.end(); ++nc_lt)
                            {
                                atomic_literal subst_literal;
                                subst_literal.assign_literal(Apply_substitution(sub_claues,*nc_lt));
                                new_added_clause.add_literal(subst_literal);
                            }
                            if(new_added_clause.get_literals().empty())
                            {
                                cout<<" Recieved empty clause ";
                                return true;
                            }
                            
                            
                            
                            new_added_clause.standardize_clause();
                            
                            
                            cout<<"\nChecking if new already exists in KB";
                            int flag = 1; //1 means good to add
                            for(vector<clause>::iterator check_it = Knowledge_base.begin(); check_it != Knowledge_base.end(); check_it++)
                            {
                                if(new_added_clause.isEqual(*check_it))
                                {
                                    cout<<"\nThe clause that was equal is ";
                                    check_it->print_clauses();
                                    flag = 0;
                                    break;
                                }
                            }
                            //for(vector<atomic_literal>)
                            if(flag == 1)
                            {
                                cout<<"\nNew clause found ";
                                new_added_clause.print_clauses();
                                new_list.push(new_added_clause);
                                My_KB.add_fact(new_added_clause);
                            }
                            
                        }
                        
                    }
                }
                
            }
        }
        
        clock_t end = clock();
        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        cout<<"Elapsed seconds are "<<elapsed_secs;
        if(elapsed_secs > 5) 
            break;
    }
    return false;
    
}

//Not using the follwing function
void resolve(vector<clause> Knowledge_base)
{
    //vector<atomic_literal> KB_literal;
//    KB_literal = KB.literal_list;
    full_substitution sub_between_clauses;
    
    
    for(vector<clause>::iterator it=Knowledge_base.begin(); it!=Knowledge_base.end(); ++it)
    {
        for(vector<clause>::iterator st=Knowledge_base.begin(); st!=Knowledge_base.end(); ++st)
        {
            if(it->resolve_possible(*st))
            {
                cout<<"\nresolve is possible between ";
                it->print_clauses();
                cout<<" and ";
                st->print_clauses();
                vector<atomic_literal> first_list;
                vector<atomic_literal> second_list;
                first_list = it->get_literals();
                second_list = st->get_literals();
                for(vector<atomic_literal>::iterator lit=first_list.begin(); lit!=first_list.end(); ++lit)
                {
                    for(vector<atomic_literal>::iterator slit=second_list.begin(); slit!=second_list.end(); ++slit)
                    {
                        if(lit->get_literal_name() == slit->get_literal_name() && lit->is_negated() != slit->is_negated())
                        {
                            cout<<"\nThe literals chosen is ";
                            lit->print_literal_name();
                            cout<<" and ";
                            slit->print_literal_name();
                            sub_between_clauses = unify_literal(*lit,*slit);
                            if(!sub_between_clauses.fail)
                            {
                                cout<<"\nNew substitution found ";
                                vector<substitution> sub_claues = sub_between_clauses.list_sub;
                                clause new_added_clause;
                                for(vector<atomic_literal>::iterator nc_lt = first_list.begin(); nc_lt != lit; ++nc_lt)
                                {
                                    atomic_literal subst_literal;
                                    subst_literal.assign_literal(Apply_substitution(sub_claues,*nc_lt));
                                    new_added_clause.add_literal(subst_literal);
                                }
                                for(vector<atomic_literal>::iterator nc_lt = lit+1; nc_lt != first_list.end(); ++nc_lt)
                                {
                                    atomic_literal subst_literal;
                                    subst_literal.assign_literal(Apply_substitution(sub_claues,*nc_lt));
                                    new_added_clause.add_literal(subst_literal);
                                }
                                for(vector<atomic_literal>::iterator nc_lt = second_list.begin(); nc_lt != slit; ++nc_lt)
                                {
                                    atomic_literal subst_literal;
                                    subst_literal.assign_literal(Apply_substitution(sub_claues,*nc_lt));
                                    new_added_clause.add_literal(subst_literal);
                                }
                                for(vector<atomic_literal>::iterator nc_lt = slit+1; nc_lt != second_list.end(); ++nc_lt)
                                {
                                    atomic_literal subst_literal;
                                    subst_literal.assign_literal(Apply_substitution(sub_claues,*nc_lt));
                                    new_added_clause.add_literal(subst_literal);
                                }
                                cout<<" New clause added ";
                                new_added_clause.print_clauses();
                                Knowledge_base.push_back(new_added_clause);
                                    
                            }
                                
                            
                        }
                    }
                }
            }
        }
    }
}
vector<clause> extract_predicates(vector<string> KB_fact)
{
    // word variable to store word
    string premise;
    string implication;
    vector<string> facts_without_implications(KB_length);
    vector<clause> Knowledge_base;
    
    int first_delimiter = 0;
    unsigned long next_delimiter;
    int begin_and_delimiter = 0;
    unsigned long next_and_delimiter;
    string conjunction_literals, literal;
    string fact;
    for(int i=0; i<KB_length; i++)
    {
        clause new_clause;
        fact = KB_fact[i];
        next_delimiter = fact.find("=>");
        if(next_delimiter != string::npos)
        {
            //        cout<<"\n next delimiter is "<<next_delimiter;
            premise = fact.substr(first_delimiter,next_delimiter-first_delimiter);
            implication = fact.substr(next_delimiter-first_delimiter+2, fact.length());
            //        cout<<"\nThe premise is "<<premise;
            cout<<"\nThe implication is "<<implication;
            
            
            conjunction_literals = premise;
            while(1)
            {
                next_and_delimiter = conjunction_literals.find('&');
                if(next_and_delimiter != string::npos)
                {
                    literal = conjunction_literals.substr(begin_and_delimiter, next_and_delimiter - begin_and_delimiter);
//                    cout<<"\nLiteral obtained is "<<literal;
                    trim(literal);
                    atomic_literal new_literal(literal);
                    new_literal.negate();
                    new_clause.add_literal(new_literal);
                    
                    //facts_without_implications[i] = facts_without_implications[i] + literal + "| ";
                    conjunction_literals = conjunction_literals.substr(next_and_delimiter + 2, conjunction_literals.length() - next_and_delimiter);
                    
                    //                cout<<"\n Conjuction literal left now is "<<conjunction_literals;
                    
                }
                else
                    break;
            }
//            cout<<"\nLast Literal obtained is "<<conjunction_literals;
            trim(conjunction_literals);
            atomic_literal new_literal(conjunction_literals);
            new_literal.negate();
            new_clause.add_literal(new_literal);
            
            trim(implication);
            atomic_literal implication_literal(implication);
            
            new_clause.add_literal(implication_literal);
            facts_without_implications[i] = facts_without_implications[i] + conjunction_literals + '|';
            facts_without_implications[i] = facts_without_implications[i] + implication;
            
        }
        else
        {
            cout<<"\nThere is only a single literal "<<fact;
            atomic_literal new_literal(fact);
            new_clause.add_literal(new_literal);
            facts_without_implications[i] = fact;
        }
        cout<<"\nNew clause is ";
        new_clause.standardize_clause();
        new_clause.print_clauses();
        Knowledge_base.push_back(new_clause);
    }
    cout<<"\nFinal facts are "<<endl;
    
    for(vector<clause>::iterator it = Knowledge_base.begin(); it!= Knowledge_base.end(); ++it)
    {
        it->print_clauses();
        cout<<endl;
    }
    return Knowledge_base;
//    for(int i=0; i<KB_length; i++)
//    {
//        cout<<facts_without_implications[i]<<endl;
//    }
    
    
    
    // making a string stream
//
//    while(next_delimiter= str.find('&'))
//    {
//        literal = str.substr(first_delimiter,next_delimiter-first_delimiter);
//        first_delimiter = next_delimiter;
//        cout<<literal;
//    }
    
    
}
int main(int argc, const char * argv[]) {
    // insert code here...
    cout<<"hey there";
    ifstream input_file("input.txt");
    if (!input_file.is_open())
    {
        cout<<"Can't open file";
        exit(1);
    }
    
    
    input_file>>number_of_queries;
    
    vector<string> query(number_of_queries);
    for (int i=0; i<number_of_queries; i++)
    {
        input_file>>query[i];
        cout<<"\nThe query is "<<query[i];
    }
    
    
    input_file>>KB_length;
    vector<string> KB_fact(KB_length);
    
    string str;
    getline(input_file,str);
    for (int i =0; i<KB_length; i++)
    {
        getline(input_file,str);
        KB_fact[i] = str;
//        cout<<"\nThe fact going is "<<KB_fact[i];
//        cout<<"\nThe fact going is "<<str;
        
    }
    input_file.close();
    My_KB.facts = extract_predicates(KB_fact);
//    clause new_clause;
//
//    atomic_literal lit1("~Take(x,Warfarin)");
//    atomic_literal lit2("~Take(Timolol,NSAIDs)");
//
//    new_clause.add_literal(lit1);
//    new_clause.add_literal(lit2);
//    Knowledge_base[0].print_clauses();
//    new_clause.print_clauses();
//    if(new_clause.isEqual(Knowledge_base[0]))
//    {
//        cout<<"Works";
//    }
//    else
//    {
//        cout<<"not really";
//    }

    vector<bool> final_answers(number_of_queries);
    for (int i=0; i<number_of_queries; i++)
    {
        cout<<"\nThe query is "<<query[i];
        if(resolution_inference(query[i]))
        {
            cout<<"\nTRUE";
            final_answers[i] = true;
        }

        else
        {
            cout<<"\nFALSE";
            final_answers[i] = false;
        }

    }
    ofstream output_file("output.txt");
    if(output_file.is_open())
    {
        for(int i=0; i<number_of_queries; i++)
        {
            if(final_answers[i])
                output_file<<"TRUE"<<endl;
            else
                output_file<<"FALSE"<<endl;
        }
    }
    else{
        cout<<"Can't open file";
    }
    output_file.close();
    return 0;
    
    
}
