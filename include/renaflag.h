#ifndef _RENAFLAG_H_
#define _RENAFLAG_H_

#include<string>
#include<stdint.h>
#include<vector>
#include<string.h>
#include<iostream>
#include<algorithm>

#define F_ROOT ""
#define F_BOOL 1
#define F_NOTBOOL 0

#define ARG_OK 0
#define ARG_REDEFINED -1
#define ARG_UNACCEPTEDARG -2
#define ARG_BOUNDED -3
#define ARG_UNDEFINEDROOT -4

#define P_TITILEONLY 0
#define P_UNDEFINED -1
#define P_VALUEAFTERBOOL -2
#define P_NOVALUEAFTERNOTBOOL -3

#define IS_SLCHAR( x ) ( ( ( x >= 'a' ) && ( x <= 'z' ) ) || ( ( x >= 'A' ) && ( x <= 'Z' ) ) ) // Is Standard Latin Char

typedef int errno_t;

namespace rena{

    typedef struct {
        std::string argname;
        std::string parent;
        bool type;

        char* value;

        bool __is_parsed;

        void __clear();
    }               arg_t;

    class renaflag{

        public:
            renaflag() { arglist.clear(); preset(); }
            
            void preset();

        public:
            errno_t addargs( std::string , std::string , bool );
            errno_t parse( int& , char**& , bool );

            void print_arglist();
            
            bool is_titleonly();
            char* get_errorarg_in_parse();

            char* getorignal( std::string , std::string );


        public:
            int32_t  geti32( std::string , std::string );
            uint32_t getu32( std::string , std::string );

        private:
            std::vector <arg_t> arglist;

            bool is_arg_legal( std::string );

    }; // class renaflag

} // namespace rena

#endif