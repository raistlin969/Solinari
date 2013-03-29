#pragma once
#include <string>
//========================================================================
// Actor.h - Defines the Actor class
//
// Part of the GameCode4 Application
//
// GameCode4 is the sample application that encapsulates much of the source code
// discussed in "Game Coding Complete - 4th Edition" by Mike McShaffry and David
// "Rez" Graham, published by Charles River Media. 
// ISBN-10: 1133776574 | ISBN-13: 978-1133776574
//
// If this source code has found it's way to you, and you think it has helped you
// in any way, do the authors a favor and buy a new copy of the book - there are 
// detailed explanations in it that compliment this code well. Buy a copy at Amazon.com
// by clicking here: 
//    http://www.amazon.com/gp/product/1133776574/ref=olp_product_details?ie=UTF8&me=&seller=
//
// There's a companion web site at http://www.mcshaffry.com/GameCode/
// 
// The source code is managed and maintained through Google Code: 
//    http://code.google.com/p/gamecode4/
//
// (c) Copyright 2012 Michael L. McShaffry and David Graham
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser GPL v3
// as published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See 
// http://www.gnu.org/licenses/lgpl-3.0.txt for more details.
//
// You should have received a copy of the GNU Lesser GPL v3
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
//========================================================================


class XMLElement;
typedef std::string ActorType;

class Actor
{
public:
  typedef std::map<ComponentId, StrongActorComponentPtr> ActorComponents;

private:
  ActorId _id;
  ActorComponents _components;
  ActorType _type;

  std::string _resource;  //xml file from which this actor was initialized

public:
  explicit Actor(ActorId id);
  ~Actor();
  bool Init(XMLElement* data);
  void PostInit();
  void Destroy();
  void Update(int delta);

  //editor functions
  std::string ToXML();

  ActorId Id() const { return _id; }
  ActorType Type() const {return _type; }

  //template function for retrieving components
  template<class ComponentType>
  weak_ptr<ComponentType> Component(ComponentId id)
  {
    ActorComponents::iterator it = _components.find(id);
    if(it != _components.end())
    {
      StrongActorComponentPtr base(it->second);
      shared_ptr<ComponentType> sub(static_pointer_cast<ComponentType>(base));  //cast to subclass pointer
      weak_ptr<ComponentType> weak_sub(sub);  //convert strong pointer to weak
      return weak_sub;
    }
    else
    {
      return weak_ptr<ComponentType>();
    }
  }

  template<class CompnentType>
  weak_ptr<CompnentType> Component(const char* name)
  {
    ComponentId id = ActorCompnent::GetIdFromName(name);
    ActorComponents::iterator it = _compnents.find(id);
    if(it != _components.end())
    {
      StrongActorCompnentPtr base(it->second);
      shared_ptr<ComponentType> sub(static_pointer_cast<ComponentType>(base));
      weak_ptr<ComponentType> weak_sub(sub);
      return weak_sub;
    }
    else
    {
      return weak_ptr<ComponentType>();
    }
  }

  const ActorComponents* Components() { return &_components; }

  void AddComponent(StrongActorComponentPtr component);
};