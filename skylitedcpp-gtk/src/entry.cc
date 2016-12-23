/*
 * Copyright © 2004-2010 Jens Oknelid, paskharen@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * In addition, as a special exception, compiling, linking, and/or
 * using OpenSSL with this program is allowed.
 */

#include "entry.hh"
#include <dcpp/stdinc.h>
#include <dcpp/Util.h>
#include "wulformanager.hh"

using namespace std;

Entry::Entry(const EntryType type, const string &ui, const string &id):
    xml(NULL),
    type(type),
    id(dcpp::Util::toString(type) + ":" + id)
{
    // Load the GtkBuilder XML file, if applicable
    if (!ui.empty())
    {
        string file = WulforManager::get()->getPath() + "/ui/" + ui;
        GError *error = NULL;
        xml = gtk_builder_new();
        gtk_builder_add_from_file(xml,file.c_str(),&error);
        if (error)
        {
            g_print("GTKBUILDER ERROR file => %s ,\n => %s",file.c_str(),error->message);
            gtk_main_quit();
        }
    }
}

Entry::~Entry()
{

}

const Entry::EntryType Entry::getType()
{
    return type;
}

const string& Entry::getID()
{
    return id;
}

void Entry::remove()
{
    removeChildren();
    WulforManager::get()->deleteEntry_gui(this);
}

/*
 * Generates a unique ID to allow for duplicate entries
 */
string Entry::generateID()
{
    return dcpp::Util::toString((long)this);
}

GtkWidget *Entry::getWidget(const string &name)
{
    dcassert(xml && !name.empty());
    GtkWidget *widget = GTK_WIDGET(gtk_builder_get_object(xml,name.c_str()));
    dcassert(widget);
    return widget;
}

void Entry::addChild(Entry *entry)
{
    children.insert(make_pair(entry->getID(), entry));
    WulforManager::get()->insertEntry_gui(entry);
}

Entry *Entry::getChild(const EntryType childType, const string &childId)
{
    string id = dcpp::Util::toString(childType) + ":" + childId;
    auto it = children.find(id);

    if (it == children.end())
        return NULL;
    else
        return it->second;
}

void Entry::removeChild(const EntryType childType, const string &childId)
{
    Entry *entry = getChild(childType, childId);
    removeChild(entry);
}

void Entry::removeChild(Entry *entry)
{
    if (entry)
    {
        entry->removeChildren();
        children.erase(entry->getID());
        WulforManager::get()->deleteEntry_gui(entry);
    }
}

void Entry::removeChildren()
{
    while (!children.empty())
        removeChild(children.begin()->second);
}
