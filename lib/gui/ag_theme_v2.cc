/*
 * Copyright (c) 2008 by David Kamphausen. All rights reserved.
 *
 * ag_theme_v2.cc
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

#include "ag_theme_v2.h"

#include "rk_debug.h"
#include "rk_tools.h"

#include "ag_config.h"
#include "ag_xml.h"

#include <iostream>
using namespace std;

std::map<AGString, AGThemeV2 *> AGThemeV2::gThemes;

AGThemeV2::AGThemeV2(const std::string &pFilename, const std::string &pTheme)
    : mFilename(pFilename), mDocument(pFilename), mTheme(pTheme) {
  Node root = mDocument.root();

  for (Node::iterator ti = root.begin(); ti != root.end(); ti++) {
    const Node &node = **ti;

    if (mTheme == "")
      if (node.getName() == "default")
        mTheme = node.get("name");

    if (node.getName() == "theme" && node.get("name") == pTheme) {
      for (Node::const_iterator ci = node.begin(); ci != node.end(); ci++) {
        if ((*ci)->getName() == "colors")
          parseColors(*ci);
        if ((*ci)->getName() == "gradients")
          parseGradients(*ci);
        if ((*ci)->getName() == "images")
          parseImages(*ci);
        if ((*ci)->getName() == "fonts")
          parseFonts(*ci);
      }
    }
  }
}

AGThemeV2::~AGThemeV2() {}

AGThemeV2 *AGThemeV2::getTheme(const std::string &pFilename,
                               const std::string &pTheme) {
  AGString filename = pFilename;
  if (filename == "")
    filename = "data/theme_v2.xml";
  AGString name = filename + pTheme;
  std::map<AGString, AGThemeV2 *>::const_iterator i = gThemes.find(name);
  if (i != gThemes.end())
    return i->second;
  AGThemeV2 *p = new AGThemeV2(filename, pTheme);
  gThemes.insert(std::make_pair(name, p));
  return p;
}

bool AGThemeV2::checkColor(const AGString &p) const {
  return true;
  // FIXME: return p.matches("/#[0-9A-Fa-f]{6}/");
}

void AGThemeV2::parseColors(const Node &pNode) {
  for (Node::const_iterator i = pNode.begin(); i != pNode.end(); i++) {
    const Node &c = **i;

    if (c.getName() == "color") {

      AGString name = c.get("name");
      AGString color = c.get("color");

      if (name.length() > 0 && checkColor(color))
        mColors.insert(std::make_pair(name, AGColor(color)));
      else
        cdebug("Problem with color name:" << name << " color:" << color);
    }
  }
  mDefaultColor = getColor("default");
}

void AGThemeV2::parseGradients(const Node &pNode) {
  for (Node::const_iterator i = pNode.begin(); i != pNode.end(); i++) {
    const Node &c = **i;

    if (c.getName() == "gradient") {
      AGString name = c.get("name");
      size_t numColors = 4, k;
      AGString colors[numColors];
      if (name.length() == 0) {
        cdebug("Problem with name:" << name);
        continue;
      }

      for (k = 0; k < numColors; k++) {
        if (!checkColor(c.get(AGString("color") + AGString(k)))) {
          cdebug("Problem with color"
                 << k << ":" << c.get(AGString("color") + AGString(k)));
          break;
        }
      }
      if (k == numColors)
        mGradients.insert(std::make_pair(
            name, AGGradient(colors[0], colors[1], colors[2], colors[3])));
    }
  }
  mDefaultGradient = getGradient("default");
}
void AGThemeV2::parseFonts(const Node &pNode) {
  for (Node::const_iterator i = pNode.begin(); i != pNode.end(); i++) {
    const Node &c = **i;

    if (c.getName() == "font") {
      AGString name = c.get("name");
      // TODO: add checks (?)

      AGFont f(c.get("file"), c.get("size").toInt());
      f.setColor(AGColor(c.get("color")));
      f.setEmbossed(c.get("embossed") == "true");
      f.setInset(c.get("inset") == "true");
      if (c.get("style") == "bold")
        f.setStyle(AGFont::BOLD);
      f.setBorderColor(AGColor(c.get("borderColor")));
      f.setBorder(c.get("borderWidth").toInt());

      mFonts.insert(std::make_pair(name, f));
    }
  }
  mDefaultFont = getFont("default");
}
void AGThemeV2::parseImages(const Node &pNode) {
  for (Node::const_iterator i = pNode.begin(); i != pNode.end(); i++) {
    const Node &c = **i;

    if (c.getName() == "image") {
      AGString name = c.get("name");
      AGString file = c.get("file");
      if (!fileExists(file)) {
        cdebug("Problem: File " << file << " does not exist!");
        continue;
      }
      if (name.length() == 0) {
        cdebug("Problem with name in image-tage with file:" << file);
        continue;
      }
      mSurfaceNames.insert(std::make_pair(name, file));
      mSurfaces.insert(std::make_pair(name, AGSurface::load(file)));
    }
  }
  mDefaultSurface = getSurface("default");
}
AGGradient AGThemeV2::getGradient(const AGString &pName) const {
  Gradients::const_iterator i = mGradients.find(pName);
  if (i == mGradients.end())
    return mDefaultGradient;
  return i->second;
}

AGFont AGThemeV2::getFont(const AGString &pName) const {
  Fonts::const_iterator i = mFonts.find(pName);
  if (i == mFonts.end())
    return mDefaultFont;
  return i->second;
}
AGColor AGThemeV2::getColor(const AGString &pName) const {
  Colors::const_iterator i = mColors.find(pName);
  if (i == mColors.end())
    return mDefaultColor;
  return i->second;
}

int AGThemeV2::getInt(const AGString &pName) const {
  Integers::const_iterator i = mIntegers.find(pName);
  if (i == mIntegers.end())
    return mDefaultInteger;
  return i->second;
}

AGSurface AGThemeV2::getSurface(const AGString &pName) const {
  Surfaces::const_iterator i = mSurfaces.find(pName);
  if (i == mSurfaces.end())
    return mDefaultSurface;
  return i->second;
}

std::string AGThemeV2::getSurfaceName(const AGString &pName) const {
  SurfaceNames::const_iterator i = mSurfaceNames.find(pName);
  if (i == mSurfaceNames.end())
    return "";
  return i->second;
}

Node *getStateNode(const Document &pDoc, const AGString &mTheme,
                   const AGString &pClass, const AGString &pState) {
  const Node &root = pDoc.root();

  for (Node::const_iterator i = root.begin(); i != root.end(); i++) {
    if ((*i)->getName() == "theme" && (*i)->get("name") == mTheme) {
      for (Node::const_iterator j = (*i)->begin(); j != (*i)->end(); j++) {
        if ((*j)->getName() == pClass) {
          for (Node::const_iterator k = (*j)->begin(); k != (*j)->end(); k++) {
            if ((*k)->getName() == pState)
              return *k;
          }
        }
      }
    }
  }
  return 0;
}

AGThemeItem AGThemeV2::getItem(const AGString &pClass,
                               const AGString &pState) const {
  // FIXME: insert XPath here !
  Node *stateNode = getStateNode(mDocument, mTheme, pClass, pState);
  assert(stateNode);
  return AGThemeItem(this, stateNode);
}
AGThemeItem AGThemeV2::getItem(const AGString &pClass, const AGString &pState,
                               const AGString &pElement) const {
  // FIXME: insert XPath here !
  Node *stateNode = getStateNode(mDocument, mTheme, pClass, pState);
  assert(stateNode);
  Node *nextNode = 0;
  for (Node::iterator i = stateNode->begin(); i != stateNode->end(); i++)
    if ((*i)->getName() == pElement)
      nextNode = *i;
  assert(nextNode);
  return AGThemeItem(this, nextNode);
}

bool AGThemeV2::hasSurface(const AGString &pName) const {
  // FIXME
  return true;
}
bool AGThemeV2::hasColor(const AGString &pName) const {

  // FIXME
  return true;
}

////////////////////////////////////////////////////////
// AGThemeItem
////////////////////////////////////////////////////////
AGThemeItem::AGThemeItem(const AGThemeV2 *pTheme, Node *pNode)
    : mTheme(pTheme), mNode(pNode) {}

AGBorder AGThemeItem::getBorder() const {
  return AGBorder(mTheme->getSurface(mNode->get("image")),
                  mTheme->getSurfaceName(mNode->get("image")));
}
AGBackground AGThemeItem::getBackground() const {
  throw std::runtime_error("not implemented");
}
AGFont AGThemeItem::getFont() const {
  throw std::runtime_error("not implemented");
}
