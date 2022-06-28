// const char* time_stamp = "$Date: 2007-05-28 14:06:56 +1200 (Mon, 28 May 2007) $\n";
// const char* getpot_id = "$Id: GetPot.h 1677 2007-05-28 02:06:56Z bianrr $\n";

//  -*- c++ -*-
//  GetPot 0.999                                      Feb/17/2002
//
//  WEBSITE: http://getpot.sourceforge.net
//
//  This library is  free software; you can redistribute  it and/or modify
//  it  under  the terms  of  the GNU  Lesser  General  Public License  as
//  published by the  Free Software Foundation; either version  2.1 of the
//  License, or (at your option) any later version.
//
//  This library  is distributed in the  hope that it will  be useful, but
//  WITHOUT   ANY  WARRANTY;   without  even   the  implied   warranty  of
//  MERCHANTABILITY  or FITNESS  FOR A  PARTICULAR PURPOSE.   See  the GNU
//  Lesser General Public License for more details.
//
//  You  should have  received a  copy of  the GNU  Lesser  General Public
//  License along  with this library; if  not, write to  the Free Software
//  Foundation, Inc.,  59 Temple Place,  Suite 330, Boston,  MA 02111-1307
//  USA
//
//  (C) 2001 Frank R. Schaefer
//==========================================================================
//<<:BEGIN-FRAME FRAMEWORK/code.c++>>
#ifndef __GETPOT_H__
#define __GETPOT_H__

#if defined(WIN32) || defined(SOLARIS_RAW)
#define strtok_r(a, b, c) strtok(a, b)
#endif // WINDOWS or SOLARIS

extern "C" {
#include <ctype.h>
#include <stdio.h>
}
#include <string>
#include <string.h>
#include <vector>
#include <fstream>
#include <stdarg.h>

using namespace std;

class GetPot {
public:
    // (*) constructors
    inline GetPot();
    inline GetPot(const GetPot&);
    inline GetPot(int argc_, char* argv_[]);
    inline GetPot(const char* FileName);

    // (*) destructor
    inline ~GetPot();

    // (*) assignment operator
//    inline GetPot& operator=(const GetPot&);

    // (*) direct access to command line arguments
    inline const char*  operator[](unsigned Idx) const;
    inline int          get(unsigned Idx, int           Default) const;
    inline double       get(unsigned Idx, const double& Default) const;
    inline const char*  get(unsigned Idx, const char*   Default) const;
    inline unsigned     size() const;

    // (*) flags
    inline bool   options_contain(const char* FlagList) const;
    inline bool   argument_contains(unsigned Idx, const char* FlagList) const;

    // (*) variables
    //     -- scalar values
    inline int          operator()(const char* VarName, int           Default) const;
    inline double       operator()(const char* VarName, const double& Default) const;
    inline const char*  operator()(const char* VarName, const char*   Default) const;
    //     -- vectors
    inline int          operator()(const char* VarName, int Default, unsigned Idx) const;
    inline double       operator()(const char* VarName, const double& Default, unsigned Idx) const;
    inline const char*  operator()(const char* VarName, const char* Default, unsigned Idx) const;
    inline unsigned     vector_variable_size(const char* VarName) const;

    // (*) cursor oriented functions
    //     -- enable/disable search for an option in loop
    inline void         disable_loop() { search_loop_f = false; }
    inline void         enable_loop()  { search_loop_f = true; }
    //     -- reset cursor to '1'
    inline bool         search_failed() const { return search_failed_f; }
    inline void         reset_cursor();
    inline void         init_multiple_occurrence();

    //     -- search for a certain option and set cursor to position
    inline bool         search(const char* option);
    inline bool         search(unsigned No, const char* P, ...);
    //     -- get argument at cursor++
    inline int          next(int           Default);
    inline double       next(const double& Default);
    inline const char*  next(const char*   Default);
    //     -- search for option and get argument at cursor++
    inline int          follow(int           Default, const char* Option);
    inline double       follow(const double& Default, const char* Option);
    inline const char*  follow(const char*   Default, const char* Option);
    //     -- search for one of the given options and get argument that follows it
    inline int          follow(int           Default, unsigned No, const char* Option, ...);
    inline double       follow(const double& Default, unsigned No, const char* Option, ...);
    inline const char*  follow(const char*   Default, unsigned No, const char* Option, ...);
    //     -- directly followed arguments
    inline int          direct_follow(int           Default, const char* Option);
    inline double       direct_follow(const double& Default, const char* Option);
    inline const char*  direct_follow(const char*   Default, const char* Option);

    // (*) nominus arguments
    inline void            reset_nominus_cursor();
    inline vector<string>  nominus_vector() const;
    inline unsigned        nominus_size() const  { return idx_nominus.size(); }
    inline const char*     next_nominus();

    // (*) output
    inline int print() const;

private:
    // (*) vector of command line arguments stored as strings
    vector<string>  argv;

    // (*) variables structure
    struct variable {
        // (*) constructors, destructors, assignment operator
        variable();
        variable(const variable&);
        variable(const char* Name, const char* Value);
        ~variable();
        variable& operator=(const variable& Other);

        // (*) get a specific element in the string vector
        const string*  get_element(unsigned Idx) const;

        // (*) data memebers
        string         name;      // identifier of variable
        vector<string> value;     // value of variable stored in vector
        string         original;  // value of variable as given on command line
    };

    // (*) vector of identified variables
    //     (arguments of the form "variable=value")
    vector<variable> variables;
    const variable*  find_variable(const char*) const;

    // (*) cursor oriented functions (nect(), follow(), etc.):
    //     pointer to actual position to be parsed.
    unsigned         cursor;
    int              nominus_cursor;
    bool             search_failed_f; // in case a search failed
    //                                // next() functions block.
    vector<unsigned> idx_nominus;     // indecies of 'no minus' arguments

    // (*) helper to find directly followed arguments
    const char*     match_starting_string(const char* StartString);

    // (*) specify if search should start at beginning after
    //     reaching end of array (default 'true')
    bool search_loop_f;

    // (*) helper to search for flags
    bool   check_flags(const string& Str, const char* FlagList) const;

    // (*) helper to parse input files
    inline void          skip_whitespace(std::istream& istr);
    inline const string  get_next_token(std::istream& istr);
    inline const string  get_string(std::istream& istr);
    inline void          parse_argv(vector<string>& ARGV);
    inline void          read_in_stream(std::istream& istr);
    inline void          read_in_file(const char* FileName);
    inline string        process_section_label(const string& Section,
                                               vector<string>& section_stack);
    inline int           convert_to_type(const string& String, int Default) const;
    inline double        convert_to_type(const string& String, double Default) const;
};

//<<:BEGIN-FUNCTIONS>>

//<<BEGIN init/code.c++>>
///////////////////////////////////////////////////////////////////////////////
// (*) constructors, destructor, assignment operator
//.............................................................................
//
inline
GetPot::GetPot()
    : cursor(0), nominus_cursor(-1), search_failed_f(true), search_loop_f(true)

{ }

inline
GetPot::GetPot(int argc_, char *argv_[])
    : cursor(0), nominus_cursor(-1), search_failed_f(false),
    search_loop_f(true)

{
    // make an internal copy of the argument list:
    argv.push_back(string(argv_[0]));
    for(int i=1; i<argc_; i++) {
        // 1) plain copy of argument list
        argv.push_back(string(argv_[i]));
    }
    parse_argv(argv);
}

inline
GetPot::GetPot(const char* FileName)
    : cursor(0), nominus_cursor(-1), search_failed_f(false), search_loop_f(true)
{
    read_in_file(FileName);
    argv.push_back(string(FileName));
    parse_argv(argv);
}

inline
GetPot::GetPot(const GetPot& Other)
{ GetPot::operator=(Other); }

inline
GetPot::~GetPot()
{ }
//<<END init/code.c++ BEGIN parse_argv/code.c++>>
inline void
GetPot::parse_argv(vector<string>& ARGV)
{
    // build internal databases:
    //
    //   1) array with no-minus arguments (usually used as filenames)
    //   2) variable assignments:
    //             'variable name' '=' number | string
    //
    for(unsigned i=0; i<argv.size(); i++) {
        // 1) separate array for nominus arguments
        if( argv[i].length() > 0 && argv[i][0] != '-' && i != 0 )
            idx_nominus.push_back(unsigned(i));

        // 2) variables: does argv[i] contain a '=' operator ?
        for(const char* p = argv[i].c_str(); *p ; p++) {
            if( *p == '=' ) {
                // set terminating 'zero' to treat first part as single string
                // => argv[i] (from start to 'p') = Name of variable
                //    p+1     (until terminating zero) = value of variable
                char* o = (char*)p++;
                *o = '\0';
                variables.push_back(variable(argv[i].c_str(), p));
                *o = '=';
                break;
            }
        }
    }
}
//<<END parse_argv/code.c++ BEGIN parse_file/code.c++>>
inline void
GetPot::read_in_file(const char* FileName)
{
    ifstream  i(FileName);
    if( ! i ) return;
    // argv[0] == the filename of the file that was read in
    read_in_stream(i);
}

inline void
GetPot::read_in_stream(std::istream& istr)
{
    vector<string>  brute_tokens;
    while(istr) {
        skip_whitespace(istr);
        const string Token = get_next_token(istr);
        if( Token.length() == 0 || Token[0] == EOF) break;
        brute_tokens.push_back(Token);
    }

    // -- reduce expressions of token1'='token2 to a single
    //    string 'token1=token2'
    // -- copy everything into 'argv'
    // -- arguments preceded by something like '[' name ']' (section)
    //    produce a second copy of each argument with a prefix '[name]argument'
    unsigned i1 = 0;
    unsigned i2 = 1;
    unsigned i3 = 2;
    string          section("");
    vector<string>  section_stack;

    while( i1 < brute_tokens.size() ) {
        // 1) detect sections '[name]'
        const string& SRef = brute_tokens[i1];
        // note: java.lang.String: substring(a,b) = from a to b-1
        //        C++ string:       substr(a,b)    = from a to a + b
        if( SRef.length() > 1 && SRef[0] == '[' && SRef[SRef.length()-1] == ']' ) {
            string SName = SRef.substr(1,SRef.length()-2);
            section = process_section_label(SName, section_stack);
            i1=i2; i2=i3; i3++;
            continue;
        }
        // 1) concatinate 'abcdef' '=' 'efgasdef' to 'abcdef=efgasdef'
        else if( i2 < brute_tokens.size() && brute_tokens[i2] == "=" ) {
            if( i3 >= brute_tokens.size() )
                argv.push_back(section + brute_tokens[i1] + brute_tokens[i2]);
            else
                argv.push_back(section + brute_tokens[i1]
                               + brute_tokens[i2] + brute_tokens[i3]);
            i1 = i3+1; i2 = i3+2; i3 = i3+3;
            continue;
        }
        else {
            argv.push_back(section + SRef);
            i1=i2; i2=i3; i3++;
        }
    }
}

inline void
GetPot::skip_whitespace(std::istream& istr)
{
    int tmp = istr.get();
    do {
        while( isspace(tmp) ) {
            tmp = istr.get();
            if( ! istr ) return;
        }
        // found a non whitespace
        if( tmp == '#' ) {
            // comment => skip until end of line
            while( tmp != '\n' ) {
                if( ! istr ) { istr.unget(); return; }
                tmp = istr.get();
            }
            continue;
        }
        else {
            istr.unget();
            return;
        }
    } while( istr );
}

inline const string
GetPot::get_next_token(std::istream& istr)
    // get next concatinates string token. consider quotes that embrace
    // whitespaces
{
    string token;
    // count number of backslashes to ensure that backslashed quotes
    // are not considered as quotes. Even number of backslashes
    // => quote active.
    unsigned backslash_count = 0;
    while( istr ) {
        int tmp = istr.get();
        if( isspace(tmp) || tmp == '#') { istr.unget(); return token; }
        // unbackslashed quotes => it's a string
        if( tmp == '\'' ) {
            if( backslash_count%2==0 )
            { token += get_string(istr); continue; }
            backslash_count=0;
        }
        else if( tmp == '\\' ) backslash_count++;
        else                   backslash_count=0;
        token += tmp;
    }
    return token;
}

inline const string
GetPot::get_string(std::istream& istr)
    // parse input until next matching '
{
    string str;
    unsigned backslash_count = 0;
    while( istr ) {
        int tmp = istr.get();
        if( tmp == '\'' ) {
            // even backslash count => quote is not backslashed
            // => string is terminated
            if( backslash_count%2==0 ) return str;
            backslash_count=0;
        }
        else if( tmp == '\\' ) backslash_count++;
        else                   backslash_count=0;
        str += tmp;
    }
    return str;
}

inline string
GetPot::process_section_label(const string& Section,
                              vector<string>& section_stack)
{
    string sname = Section;
    //  1) subsection of actual section ('./' prefix)
    if( sname.length() >= 2 && sname.substr(0, 2) == "./" ) {
        sname = sname.substr(2, sname.length()-1);
    }
    //  2) subsection of parent section ('../' prefix)
    else if( sname.length() >= 3 && sname.substr(0, 3) == "../" ) {
        do {
            if( section_stack.end() != section_stack.begin() )
                section_stack.pop_back();
            sname = sname.substr(3, sname.length()-3);
        } while( sname.substr(0, 3) == "../" );
    }
    // 3) subsection of the root-section
    else {
        section_stack.erase(section_stack.begin(), section_stack.end());
        // [] => back to root section
    }

    if( sname != "" ) {
        // parse section name for 'slashes'
        unsigned i=0;
        while( i < sname.length() ) {
            if( sname[i] == '/' ) {
                section_stack.push_back(sname.substr(0,i));
                if( i+1 < sname.length()-1 )
                    sname = sname.substr(i+1, sname.length());
                i = 0;
            }
            else
                i++;
        }
        section_stack.push_back(sname);
    }
    string section = "";
    if( section_stack.size() != 0 )
        for(vector<string>::const_iterator it = section_stack.begin();
            it != section_stack.end();
            it++ ) {
            section += *it + "/";
        }
    return section;
}
//<<END parse_file/code.c++ BEGIN convert_to_type/code.c++>>
// convert string to DOUBLE, if not possible return Default
inline double
GetPot::convert_to_type(const string& String, double Default) const
{
    double tmp;
    if( sscanf(String.c_str(),"%lf", &tmp) != 1 ) return Default;
    return tmp;
}

// convert string to INT, if not possible return Default
inline int
GetPot::convert_to_type(const string& String, int Default) const
{
    int tmp;
    if( sscanf(String.c_str(),"%i", &tmp) != 1 ) return Default;
    return tmp;
}
//<<END convert_to_type/code.c++ BEGIN search/code.c++>>
//////////////////////////////////////////////////////////////////////////////
// (*) cursor oriented functions
//.............................................................................
//     -- search for a certain argument and set cursor to position
inline bool
GetPot::search(const char* Option)
{
    unsigned  OldCursor = cursor;
    if( OldCursor >= argv.size() ) OldCursor = argv.size() - 1;
    search_failed_f = true;

    // (*) first loop from cursor position until end
    unsigned  c = cursor;
    for(; c < argv.size(); c++) {
        if( argv[c] == Option )
        { cursor = c; search_failed_f = false; return true; }
    }
    if( ! search_loop_f ) return false;

    // (*) second loop from 0 to old cursor position
    for(c = 1; c < OldCursor; c++) {
        if( argv[c] == Option )
        { cursor = c; search_failed_f = false; return true; }
    }
    // in case nothing is found the cursor stays where it was
    return false;
}


inline bool
GetPot::search(unsigned No, const char* P, ...)
{
    if( No == 0 ) return false;
    if( search(P) == true ) return true;

    // start interpreting variable argument list
    va_list ap;
    va_start(ap, P);
    for(unsigned i=1; i<No; i++) {
        char* Opt = va_arg(ap, char *);
        if( search(Opt) == true )
        { va_end(ap); return true; }
    }
    va_end(ap);
    return false;
}

inline void
GetPot::reset_cursor()
{ search_failed_f = false; cursor = 0; }

inline void
GetPot::init_multiple_occurrence()
{ disable_loop(); reset_cursor(); }
//<<END search/code.c++ BEGIN get/code.c++>>
///////////////////////////////////////////////////////////////////////////////
// (*) direct access to command line arguments
//.............................................................................
//
inline const char*
GetPot::operator[](unsigned idx) const
{ return idx<argv.size() ? argv[idx].c_str() : 0; }

inline int
GetPot::get(unsigned Idx, int Default) const
{
    if( Idx >= argv.size() ) return Default;
    return convert_to_type(argv[Idx], Default);
}

inline double
GetPot::get(unsigned Idx, const double& Default) const
{
    if( Idx >= argv.size() ) return Default;
    return convert_to_type(argv[Idx], Default);
}

inline const char*
GetPot::get(unsigned Idx, const char* Default) const
{
    if( Idx >= argv.size() ) return Default;
    else                     return argv[Idx].c_str();
}

inline unsigned
GetPot::size() const
{ return argv.size(); }
//<<END get/code.c++ BEGIN next/code.c++>>
//     -- next() function group
inline int
GetPot::next(int Default)
{
    if( search_failed_f ) return Default;
    cursor++;
    if( cursor >= argv.size() )
    { cursor = argv.size(); return Default; }
    return convert_to_type(argv[cursor], Default);
}

inline double
GetPot::next(const double& Default)
{
    if( search_failed_f ) return Default;
    cursor++;
    if( cursor >= argv.size() )
    { cursor = argv.size(); return Default; }
    return convert_to_type(argv[cursor], Default);
}

inline const char*
GetPot::next(const char* Default)
{
    if( search_failed_f ) return Default;
    cursor++;
    if( cursor >= argv.size() )
    { cursor = argv.size(); return Default; }
    return argv[cursor].c_str();
}
//<<END next/code.c++ BEGIN follow/code.c++>>
//     -- follow() function group
//        distinct option to be searched for
inline int
GetPot::follow(int Default, const char* Option)
{
    if( search(Option) == false ) return Default;
    return next(Default);
}

inline double
GetPot::follow(const double& Default, const char* Option)
{
    if( search(Option) == false ) return Default;
    return next(Default);
}

inline const char*
GetPot::follow(const char* Default, const char* Option)
{
    if( search(Option) == false ) return Default;
    return next(Default);
}

//     -- second follow() function group
//        multiple option to be searched for
inline int
GetPot::follow(int Default, unsigned No, const char* P, ...)
{
    if( No == 0 ) return Default;
    if( search(P) == true ) return next(Default);

    va_list ap;
    va_start(ap, P);
    for(unsigned i=1; i<No; i++) {
        char* Opt = va_arg(ap, char *);
        if( search(Opt) == true ) {
            va_end(ap);
            return next(Default);
        }
    }
    va_end(ap);
    return Default;
}

inline double
GetPot::follow(const double& Default, unsigned No, const char* P, ...)
{
    if( No == 0 ) return Default;
    if( search(P) == true ) return next(Default);

    va_list ap;
    va_start(ap, P);
    for(unsigned i=1; i<No; i++) {
        char* Opt = va_arg(ap, char *);
        if( search(Opt) == true ) {
            va_end(ap);
            return next(Default);
        }
    }
    va_end(ap);
    return Default;
}

inline const char*
GetPot::follow(const char* Default, unsigned No, const char* P, ...)
{
    if( No == 0 ) return Default;
    if( search(P) == true ) return next(Default);

    va_list ap;
    va_start(ap, P);
    for(unsigned i=1; i<No; i++) {
        char* Opt = va_arg(ap, char *);
        if( search(Opt) == true ) {
            va_end(ap);
            return next(Default);
        }
    }
    va_end(ap);
    return Default;
}
//<<END follow/code.c++ BEGIN direct_follow/code.c++>>
///////////////////////////////////////////////////////////////////////////////
// (*) directly connected options
//.............................................................................
//
inline int
GetPot::direct_follow(int Default, const char* Option)
{
    const char* FollowStr = match_starting_string(Option);
    if( FollowStr == 0 )                    return Default;
    if( ++cursor >= argv.size() ) cursor = argv.size();
    return convert_to_type(FollowStr, Default);
}

inline double
GetPot::direct_follow(const double& Default, const char* Option)
{
    const char* FollowStr = match_starting_string(Option);
    if( FollowStr == 0 )                   return Default;
    if( ++cursor >= argv.size() ) cursor = argv.size();
    return convert_to_type(FollowStr, Default);
}

inline const char*
GetPot::direct_follow(const char* Default, const char* Option)
{
    if( search_failed_f ) return Default;
    const char* FollowStr = match_starting_string(Option);
    if( FollowStr == 0 )          return Default;
    if( ++cursor >= argv.size() ) cursor = argv.size();
    return FollowStr;
}

inline const char*
GetPot::match_starting_string(const char* StartString)
    // pointer  to the place where the string after
    //          the match inside the found argument starts.
    // 0        no argument matches the starting string.
{
    const unsigned N = strlen(StartString);
    unsigned OldCursor = cursor;
    if( OldCursor >= argv.size() ) OldCursor = argv.size() - 1;
    search_failed_f = true;

    // (*) first loop from cursor position until end
    unsigned c = cursor;
    for(; c < argv.size(); c++) {
        if( strncmp(StartString, argv[c].c_str(), N) == 0)
        { cursor = c; search_failed_f = false; return &(argv[c].c_str()[N]); }
    }

    if( ! search_loop_f ) return 0;

    // (*) second loop from 0 to old cursor position
    for(c = 1; c < OldCursor; c++) {
        if( strncmp(StartString, argv[c].c_str(), N) == 0)
        { cursor = c; search_failed_f = false; return &(argv[c].c_str()[N]); }
    }
    return 0;
}
//<<END direct_follow/code.c++ BEGIN flags/code.c++>>
///////////////////////////////////////////////////////////////////////////////
// (*) search for flags
//.............................................................................
//
inline bool
GetPot::options_contain(const char* FlagList) const
{
    // go through all arguments that start with a '-', but not more than one
    for(vector<string>::const_iterator it = argv.begin();
        it != argv.end();
        it++) {
        const string& Str = *it;
        if( Str.length() >= 2 && Str[0] == '-' && Str[1] != '-' )
            if( check_flags(Str, FlagList) ) return true;
    }
    return false;
}

inline bool
GetPot::argument_contains(unsigned Idx, const char* FlagList) const
{
    if( Idx >= argv.size() ) return false;
    return check_flags(argv[Idx], FlagList);
}

inline bool
GetPot::check_flags(const string& Str, const char* FlagList) const
{
    for(const char* p=FlagList; *p != '\0' ; p++)
        if( Str.find(*p) != Str.npos ) return true; // found something
    return false;
}
//<<END flags/code.c++ BEGIN nominus/code.c++>>
///////////////////////////////////////////////////////////////////////////////
// (*) nominus arguments
inline vector<string>
GetPot::nominus_vector() const
    // return vector of nominus arguments
{
    vector<string> nv;
    for(vector<unsigned>::const_iterator it = idx_nominus.begin();
        it != idx_nominus.end();
        it++) {
        nv.push_back(argv[*it]);
    }
    return nv;
}

inline const char*
GetPot::next_nominus()
{
    if( nominus_cursor < int(idx_nominus.size()) - 1 )
        return argv[idx_nominus[++nominus_cursor]].c_str();
    return 0;
}

inline void
GetPot::reset_nominus_cursor()
{ nominus_cursor = -1; }
//<<END nominus/code.c++ BEGIN variables/code.c++>>
///////////////////////////////////////////////////////////////////////////////
// (*) variables
//.............................................................................
//
inline int
GetPot::operator()(const char* VarName, int Default) const
{
    const variable*  sv = find_variable(VarName);
    if( sv == 0 ) return Default;
    return convert_to_type(sv->original, Default);
}

inline double
GetPot::operator()(const char* VarName, const double& Default) const
{
    const variable*  sv = find_variable(VarName);
    if( sv == 0 ) return Default;
    return convert_to_type(sv->original, Default);
}

inline const char*
GetPot::operator()(const char* VarName, const char* Default) const
{
    const variable*  sv = find_variable(VarName);
    if( sv == 0 ) return Default;
    return sv->original.c_str();
}

inline int
GetPot::operator()(const char* VarName, int Default, unsigned Idx) const
{
    const variable* sv = find_variable(VarName);
    if( sv == 0 ) return Default;
    const string*  element = sv->get_element(Idx);
    if( element == 0 ) return Default;
    return convert_to_type(*element, Default);
}

inline double
GetPot::operator()(const char* VarName, const double& Default, unsigned Idx) const
{
    const variable* sv = find_variable(VarName);
    if( sv == 0 ) return Default;
    const string*  element = sv->get_element(Idx);
    if( element == 0 ) return Default;
    return convert_to_type(*element, Default);
}

inline const char*
GetPot::operator()(const char* VarName, const char* Default, unsigned Idx) const
{
    const variable*  sv = find_variable(VarName);
    if( sv == 0 ) return Default;
    const string* element = sv->get_element(Idx);
    if( element == 0 )  return Default;
    return element->c_str();
}

inline unsigned
GetPot::vector_variable_size(const char* VarName) const
{
    const variable*  sv = find_variable(VarName);
    if( sv == 0 ) return 0;
    return sv->value.size();
}

inline const GetPot::variable*
GetPot::find_variable(const char* Name) const
{
    for(vector<variable>::const_iterator it = variables.begin();
        it != variables.end();
        it++)
        if( (*it).name == Name ) return &(*it);
    return 0;
}
//<<END variables/code.c++ BEGIN print/code.c++>>
///////////////////////////////////////////////////////////////////////////////
// (*) ouput (basically for debugging reasons
//.............................................................................
//
inline int
GetPot::print() const
{
    cout << "argc        = " << argv.size() << endl;
    for(unsigned i=1; i<argv.size() ; i++) {
        cout << "[" << argv[i] << "]" << endl;
    }
    cout << endl;
    return 1;
}
//<<END print/code.c++ BEGIN variable_class/code.c++>>
///////////////////////////////////////////////////////////////////////////////
// (*) variable class
//.............................................................................
//
inline
GetPot::variable::variable()
{}

inline
GetPot::variable::variable(const variable& Other)
{
#ifdef WIN32
    operator=(Other);
#else
    GetPot::variable::operator=(Other);
#endif
}


inline
GetPot::variable::variable(const char* Name, const char* Value)
    : name(Name)
{
    original = string(Value);
    // separate string by white space delimiters using 'strtok'
    // thread safe usage of strtok (no static members)
    char* spt = 0;
    // make a copy of the 'Value'
    char* copy = new char[strlen(Value)+1];
    strcpy(copy, Value);
    char* follow_token = strtok_r(copy, " \t", &spt);
    while(follow_token != 0) {
        value.push_back(string(follow_token));
        follow_token = strtok_r(NULL, " \t", &spt);
    }
}

inline const string*
GetPot::variable::get_element(unsigned Idx) const
{ if( Idx >= value.size() ) return 0; else return &(value[Idx]); }

inline
GetPot::variable::~variable()
{}

inline GetPot::variable&
GetPot::variable::operator=(const GetPot::variable& Other)
{
    name     = Other.name;
    value    = Other.value;
    original = Other.original;
    return *this;
}

//<<END variable_class/code.c++ >>
//<<:EPILOG>>

#endif // __GETPOT_H__


