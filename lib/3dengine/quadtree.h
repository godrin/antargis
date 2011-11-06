/*
 * Copyright (c) 2005 by David Kamphausen. All rights reserved.
 *
 * quadtree.h
 * by David Kamphausen (david.kamphausen@web.de)
 *
 * The "Antargis" project, including all files needed to compile it,
 * is free software; you can redistribute it and/or use it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program.
 */

#ifndef QUADTREE_H
#define QUADTREE_H

#include <ag_geometry.h>
#include <rk_debug.h>

#include <set>

/**
   sadly enough C++ still doesn't support templating in out of object files...

   This is a simple quad-tree, that's based upon some "unknown object", that must contain
   these functions:
 * getRect()
 * any more ???
   The Rectangles are defined with AGRect2.

   QuadTree doesn't own objects - you have to deleete them yourself

 */
template<class T>
class QuadTree
{
  // ignore this node-structure - it works ;-)
  struct Node
  {
    std::set
    <T*> ts;
    std::list<Node*> children;
    AGRect2 r;

    Node(AGRect2 R):r(R)
    {}

    ~Node()
      {
        for(typename std::list<Node*>::iterator i=children.begin();i!=children.end();++i)
          checkedDelete(*i);
      }

    size_t size() const
    {
      size_t s=ts.size();
      for(typename std::list<Node*>::const_iterator i=children.begin();i!=children.end();i++)
        s+=(*i)->size();
      return s;
    }
    void getAll(std::list<T*> &l)
      {
        std::copy(ts.begin(),ts.end(),std::back_inserter(l));
        for(typename std::list<Node*>::iterator i=children.begin();i!=children.end();i++)
          (*i)->getAll(l);
      }

    void get(const AGRect2 &pr,std::list<T*> &l)
      {
        typename std::set<T*>::iterator i=ts.begin();
        for(;i!=ts.end();i++)
          l.push_back(*i);

        typename std::list<Node*>::iterator j=children.begin();
        for(;j!=children.end();j++)
          if((*j)->r.collide(pr))
            (*j)->get(pr,l);
      }

    void insert(T* t)
      {
        if(children.size())
          {
            typename std::list<Node*>::iterator i=children.begin();
            for(;i!=children.end();i++)
              if((*i)->r.contains(t->getRect()))
                {
                  (*i)->insert(t);
                  return;
                }
          }
        ts.insert(t);
        if(ts.size()>2 && children.size()==0 && r.w()>3 && r.h()>3)
          split();
      }

    void clear()
      {
        typename std::list<Node*>::iterator i=children.begin();
        for(;i!=children.end();i++)
          {
            (*i)->clear();
            checkedDelete(*i);
          }
        children.clear();

        ts.clear();

      }

    void split()
      {
        std::set
        <T*> ot=ts;
        ts.clear();
        std::list<AGRect2> rs=r.split();
        std::list<AGRect2>::iterator i=rs.begin();
        for(;i!=rs.end();i++)
          {
            children.push_back(new Node(*i));
          }
        typename std::set
        <T*>::iterator j=ot.begin();
        for(;j!=ot.end();j++)
          insert(*j);
      }

    bool remove
    (T* t)
      {
        if(ts.find(t)!=ts.end())
          {
            ts.erase(t);
            return true;
          }
        if(children.size())
          {
            typename std::list<Node*>::iterator i=children.begin();
            for(;i!=children.end();i++)
              {
                if((*i)->r.contains(t->getRect()))
                  return (*i)->remove(t);
              }
          }
        return false;
      }

  };

  Node *root;

public:

  /**
   * A quadtree tiles big rectangles into smaller ones. Elements get inserted into the smallest rectangle that contains an element.
   * So you have a starting-rectangle
   * @param r Starting rectangle, that gets tiled. This should contain everything that you want to store here. But don't make this
   *  rectangle too big, because otherwise must be tiled several times before even one object can be inserted.
   */

  QuadTree(AGRect2 r)
    {
      root=new Node(r);
    }
  ~QuadTree()
    {
      checkedDelete(root);
    }

  /**
       insert an object
       @param t the object you want to insert
   */
  void insert(T* t)
    {
      root->insert(t);
    }

  /// get all the objects - should be used at all (only testing purpose)
  std::list<T*> getAll()
  {
    std::list<T*> l;
    root->getAll(l);
    return l;
  }

  /// this one you should call - get all objects, that intersect (or are included in) this rectangle
  std::list<T*> get(const AGRect2 &r)
  {
    std::list<T*> l;
    root->get(r,l);
    return l;
  }

  /// remove some object
  bool remove
  (T* t)
    {
      if(t)
        return root->remove(t);
      return false;
    }

  /// empty this tree!
  void clear()
    {
      root->clear();
    }
  size_t size() const
  {
    return root->size();
  }
};

#endif
