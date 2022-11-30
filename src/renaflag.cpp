#include"renaflag.h"

void rena::arg_t::__clear(){
    argname.clear();
    parent.clear();
    type = F_NOTBOOL;
    strcpy( value , "" );
    __is_parsed = false;
    return;
}

errno_t rena::renaflag::addargs( std::string __arg , std::string __parent , bool __type = F_NOTBOOL ){
    arg_t temp;
    //temp.__clear();
    temp.argname = __arg;
    temp.parent = __parent;
    temp.type = __type;
    // get new arg to temp
    
    if ( !is_arg_legal( temp.argname ) ) // unaccepted arg name
    {
        std::cerr << "[renaflag runtime] error -> func. addargs: illegal arg name \"" << temp.argname << "\"" << std::endl;
        return ARG_UNACCEPTEDARG;
    }

    std::transform( temp.argname.begin() , temp.argname.end() , temp.argname.begin() , ::tolower );
    std::transform( temp.parent.begin() , temp.parent.end() , temp.parent.begin() , ::tolower );

    if ( temp.parent != F_ROOT ) // not root arg, search for parent arg for it
    {
        int parentarg_pos = -1;
        for ( int i = 0 ; i < arglist.size() ; i++ )
        {
            if ( ( arglist[i].parent == F_ROOT ) && ( arglist[i].argname == temp.parent ) ) // found legal root parent arg
            {
                parentarg_pos = i;
                break;
            }

            if ( ( arglist[i].argname == temp.parent ) && ( arglist[i].parent != F_ROOT ) ) // argname same, but parent arg isn't root arg
            {
                std::cerr << "[renaflag runtime] error -> func. addargs: parent arg \"" << temp.parent << "\" for arg \"" << temp.argname << "\" isn't root arg" << std::endl;
                return ARG_BOUNDED;
            }
        }

        if ( parentarg_pos == -1 ) // parent arg not found
        {
            std::cerr << "[renaflag runtime] error -> func. addargs: parent arg \"" << temp.parent << "\" for arg \"" << temp.argname << "\" not defined" << std::endl;
            return ARG_UNDEFINEDROOT;
        }

        for ( int i = parentarg_pos + 1 ; i < arglist.size() ; i++ )
        {
            if ( ( arglist[i].parent == temp.parent ) && ( arglist[i].argname == temp.argname ) ) // sub arg renamed
            {
                std::cerr << "[renaflag runtime] error -> func. addargs: sub arg \"" << temp.argname << "\" under root arg \"" << temp.parent << "\" renamed" << std::endl;
                return ARG_REDEFINED;
            }
        }

        arglist.push_back( temp );
    }
    else
    {
        for ( int i = 0 ; i < arglist.size() ; i++ )
        {
            if ( ( arglist[i].parent == F_ROOT ) && ( arglist[i].argname == temp.argname ) ) // root arg renamed
            {
                std::cerr << "[renaflag runtime] error -> func. addargs: root arg \"" << temp.argname << "\" renamed" << std::endl;
                return ARG_REDEFINED;
            }
        }

        arglist.push_back( temp );
    }

    return ARG_OK;
}

void rena::renaflag::print_arglist(){
    for ( int i = 0 ; i < arglist.size() ; i++ )
    {
        std::cout << arglist[i].argname << " ";
        if ( arglist[i].parent == F_ROOT )
        {
            std::cout << "ROOT_ARG" << " ";
        }
        else
        {
            std::cout << arglist[i].parent << " ";
        }
        std::cout << arglist[i].type << std::endl;
    }
}

bool rena::renaflag::is_arg_legal( std::string __arg ){
    if ( __arg.size() == 0 )
    {
        return false;
    }

    for ( int i = 0 ; i < __arg.size() ; i++ )
    {
        if ( !IS_SLCHAR( __arg[i] ) )
        {
            return false;
        }
    }
    return true;
}