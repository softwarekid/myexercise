/***************************************************************************
 *   Copyright (C) 2008 by Tyler Shaub   *
 *   tyler.shaub@gmail.com   *
 *                                                                         *
 *   See COPYING	*
 ***************************************************************************/

#include <iostream>
#include <map>
#include <boost/unordered_map.hpp>
#include <vector>

#include "TypeManager.hpp"

#include "TypeInfo.hpp"
#include "MemberFunctions.hpp"

namespace Flex_
{

    class TypeInfoPtr
    {
    public:
        TypeInfoPtr( std::type_info const& ref ) : ptr( &ref ) {}

        bool operator==( TypeInfoPtr const& rhs ) const
        {
            return ( *ptr ) == ( *rhs.ptr );
        }
        bool operator!=( TypeInfoPtr const& rhs ) const
        {
            return ( *ptr ) != ( *rhs.ptr );
        }
        bool operator<( TypeInfoPtr const& rhs ) const
        {
            return ptr->before( *rhs.ptr ) != false;
        }

        operator std::type_info const* () const
        {
            return ptr;
        }

    private:
        std::type_info const* ptr;
    };


    typedef boost::unordered_map<std::string, TypeId> TypeNames;
    typedef boost::unordered_map<TypeInfoPtr, TypeId, boost::hash<std::type_info const*> > AllTypes;

    struct TypeManager::Impl
    {
        Impl() : nextTypeId_( INVALID_TYPE ), infos_(), typeNames_(), allTypes_()
        {	}

        TypeInfo& checkedGetInfo( TypeId id );

        TypeId nextTypeId_;

        typedef std::vector<TypeInfo> TypeInfoArray;
        TypeInfoArray infos_;

        TypeNames typeNames_;

        AllTypes allTypes_;
    };

    TypeManager::TypeManager() : impl_( new Impl() )
    {
    }

    void TypeManager::Shutdown()
    {
        impl_.reset();
    }

    TypeManager& TypeManager::GetInst()
    {
        static TypeManager inst;
        return inst;
    }

    TypeInfo& TypeManager::Impl::checkedGetInfo( TypeId id )
    {
        assert( id != INVALID_TYPE && "Invalid Type lookup!" ); // explicitly invalid
        assert( static_cast<size_t>( id ) < infos_.size() );	// implicitly invalid
        return infos_[id];
    }

    TypeInfo TypeManager::CreateTypeInfo()
    {
        return TypeInfo( ++impl_->nextTypeId_ );
    }

    int TypeManager::TotalTypesCreated() const
    {
        return impl_->nextTypeId_;
    }

    void TypeManager::AddTypeImplementation( TypeId baseId, TypeId derivedId )
    {
        impl_->checkedGetInfo( baseId ).AddImplementation( derivedId );
    }

    void TypeManager::SetMemberFunctions( TypeId typeId, MemberFunctions const& functions )
    {
        MemberFunctions withBaseFuncs = functions;

        TypeInfo& info = impl_->checkedGetInfo( typeId );

        /* doh, see assert in TypeInfo::SetFunctions
        // add base type functions
        for(TypeInfo::TypeSet::const_iterator i = info.GetInterfaces().begin(); i != info.GetInterfaces().end(); ++i)
        {
        	MemberFunctions::FunctionRange baseFns = impl_->checkedGetInfo(*i).GetFunctions().GetAllFunctions();
        	for(MemberFunctions::FunctionIter f = baseFns.first; f != baseFns.second; ++f)
        	{
        		if(!withBaseFuncs.HasFunction(f->first))
        			withBaseFuncs.AddFunction(f->first, f->second);
        	}

        }

        // vice versa: check for derived classes that implement this class's functions, add them
        // this is more work.. gonna require copies and special-casing
        for(TypeInfo::TypeSet::const_iterator i = info.GetImplementations().begin(); i != info.GetImplementations().end(); ++i)
        {
        	TypeInfo & derivedInfo = impl_->checkedGetInfo(*i);
        	MemberFunctions::FunctionRange myFns = withBaseFuncs.GetAllFunctions();
        	for(MemberFunctions::FunctionIter f = myFns.first; f != myFns.second; ++f)
        	{
        		// ack, i just realized the problem here..
        		// TODO: fix this.
        	}
        }
        */

        info.SetFunctions( withBaseFuncs );
    }

    void TypeManager::AddTypeAlias( TypeId typeId, std::string const& aliasName )
    {
        impl_->typeNames_[aliasName] = typeId;
    }

    TypeInfo const& TypeManager::GetInfoFromId( TypeId typeId ) const
    {
        return impl_->checkedGetInfo( typeId );
    }

    TypeId TypeManager::GetIdFromStdInfo( std::type_info const& info ) const
    {
        AllTypes::const_iterator i = impl_->allTypes_.find( TypeInfoPtr( info ) );
        return i == impl_->allTypes_.end() ? INVALID_TYPE : i->second;
    }

    TypeId TypeManager::GetIdFromName( const std::string& name ) const
    {
        TypeNames::const_iterator i = impl_->typeNames_.find( name );
        return i == impl_->typeNames_.end() ? INVALID_TYPE : i->second;
    }


    TypeId TypeManager::registerType( std::type_info const& typeId, TypeInfo info )
    {
        const TypeId id = info.GetId();

        assert( id <= impl_->nextTypeId_ ); // no funny business! can't make up your own ids!

        // grow list
        if( impl_->infos_.size() <= static_cast<size_t>( id ) )
            impl_->infos_.resize( id + 1, TypeInfo( INVALID_TYPE ) );

        impl_->infos_[id] = info;

        impl_->allTypes_[ typeId ] = id;

        assert( impl_->typeNames_.find( info.GetName() ) == impl_->typeNames_.end() \
                && "Re-registering type not supported" );

        impl_->typeNames_[info.GetName()] = id;

        return info.GetId();
    }

}
